//export PATH=/home/wang/hi3559av_buitroot/buildroot-env/output/host/bin:$PATH
//export PKG_CONFIG_PATH=/home/linux/git/buildroot-env/output/host/lib/pkgconfig/:$PATH
//编译库：aarch64-buildroot-linux-gnu-gcc bq27541_battery.c -shared -fPIC -o libbq27541.so `pkg-config --cflags --libs glib-2.0 gobject-2.0` -I/home/wang/git_file/learn/bq27541
#include <bq27541_battery.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#define BQ27541_BATTERY_GET_PRIVATE(object) G_TYPE_INSTANCE_GET_PRIVATE((object), TYPE_BQ27541_BATTERY, Bq27541BatteryPrivate)
#define CRITICAL_CAPACITY (5)
#define LOW_CAPACITY (10)

struct _Bq27541BatteryPrivate
{
    GString *path;
    gint status;
    gint capacity;     
    gint CapacityAlertMax;
    gint CapacityAlertMin;
    gint temp;             //单位0.1℃（摄氏度）
    gint TempAlertMax;     //单位0.1℃（摄氏度）
    gint TempAlertMin;     //单位0.1℃（摄氏度）
    gint CapacityLevel;
};

G_DEFINE_TYPE_WITH_CODE (Bq27541Battery, bq27541_battery, G_TYPE_OBJECT, G_ADD_PRIVATE (Bq27541Battery))

Bq27541BatteryPrivate *priv;
gint capacity_flag;
gint temp_flag;
gint level_flag;

enum
{
    PROPERTY_0,
    PROPERTY_PATH,
    PROPERTY_BATTERYSTATUS,
    PROPERTY_CAPACITY,
    PROPERTY_CAPACITYALERTMAX,
    PROPERTY_CAPACITYALERTMIN,
    PROPERTY_TEMP,
    PROPERTY_TEMPALERTMAX,
    PROPERTY_TEMPALERTMIN,
    PROPERTY_CAPACITYLEVEL,
    N_PROPERTY
};

enum FLAG
{
    FLAG_0,
    FLAG_1,
    FLAG_2
};

gboolean battery_monitor(gpointer data);
gint get_param_state(gpointer data);
void get_param_temp();
gint get_param_capacity();
gint get_param_current();
gint get_param_level();

static void bq27541_battery_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    Bq27541Battery *self = BQ27541_BATTERY(object);
    priv = BQ27541_BATTERY_GET_PRIVATE(self);

    switch (property_id)
    {
    case PROPERTY_PATH:
        if(priv->path)
            g_string_free(priv->path, TRUE);
        priv->path = g_string_new(g_value_get_string(value));
        break;
    case PROPERTY_BATTERYSTATUS:
        priv->status = g_value_get_int(value);
        break;
    case PROPERTY_CAPACITY:
        priv->capacity = g_value_get_int(value);
        break;
    case PROPERTY_CAPACITYALERTMAX:
        priv->CapacityAlertMax = g_value_get_int(value);
        break;
    case PROPERTY_CAPACITYALERTMIN:
        priv->CapacityAlertMin = g_value_get_int(value);
        break;
    case PROPERTY_TEMP:
        priv->temp = g_value_get_int(value);
        break;
    case PROPERTY_TEMPALERTMAX:
        priv->TempAlertMax = g_value_get_int(value);
        break;
    case PROPERTY_TEMPALERTMIN:
        priv->TempAlertMin = g_value_get_int(value);
        break;
    case PROPERTY_CAPACITYLEVEL:
        priv->CapacityLevel = g_value_get_int(value);
        break;

    default:
        break;
    }
}

static void bq27541_battery_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    Bq27541Battery *self = BQ27541_BATTERY(object);
    priv = BQ27541_BATTERY_GET_PRIVATE(self);

    switch (property_id)
    {
    case PROPERTY_PATH:
        g_value_set_string(value, priv->path->str);
        break;
    case PROPERTY_BATTERYSTATUS:
        get_param_state(self);
        g_value_set_int(value, priv->status);
        break;
    case PROPERTY_CAPACITY:
        get_param_capacity();
        g_value_set_int(value, priv->capacity);
        break;
    case PROPERTY_CAPACITYALERTMAX:
        g_value_set_int(value, priv->CapacityAlertMax);
        break;
    case PROPERTY_CAPACITYALERTMIN:
        g_value_set_int(value, priv->CapacityAlertMin);
        break;
    case PROPERTY_TEMP:
        get_param_temp();
        g_value_set_int(value, priv->temp);
        break;
    case PROPERTY_TEMPALERTMAX:
        g_value_set_int(value, priv->TempAlertMax);
        break;
    case PROPERTY_TEMPALERTMIN:
        g_value_set_int(value, priv->TempAlertMin);
        break;
    case PROPERTY_CAPACITYLEVEL:
        get_param_level();
        g_value_set_int(value, priv->CapacityLevel);
        break;

    default:
        break;
    }

}

static void bq27541_battery_class_init(Bq27541BatteryClass *bclass)
{
    GObjectClass *base_class = G_OBJECT_CLASS(bclass);
    base_class->get_property = bq27541_battery_get_property;
    base_class->set_property = bq27541_battery_set_property;

    GParamSpec *properties[N_PROPERTY] = {NULL, };
    properties[PROPERTY_PATH] = g_param_spec_string("path", "path", "sysfs path", NULL, G_PARAM_READWRITE);
    properties[PROPERTY_BATTERYSTATUS] = g_param_spec_int("status", NULL, NULL, 0, G_MAXINT32, 0,  G_PARAM_READWRITE);
    properties[PROPERTY_CAPACITY] = g_param_spec_int("capacity", "capacity", NULL, 0, G_MAXINT32, 0, G_PARAM_READWRITE);
    properties[PROPERTY_CAPACITYALERTMAX] = g_param_spec_int("CapacityAlertMax", "CapacityAlertMax", NULL, 0, G_MAXINT32, 98, G_PARAM_READWRITE);
    properties[PROPERTY_CAPACITYALERTMIN] = g_param_spec_int("CapacityAlertMin", "CapacityAlertMin", NULL, 0, G_MAXINT32, 10, G_PARAM_READWRITE);
    properties[PROPERTY_TEMP] = g_param_spec_int("temp", "Batterytemperature", NULL, 0, G_MAXINT32, 0, G_PARAM_READWRITE);
    properties[PROPERTY_TEMPALERTMAX] = g_param_spec_int("TempAlertMax", "Battery temperature max", NULL, 0, G_MAXINT32, 450, G_PARAM_READWRITE);
    properties[PROPERTY_TEMPALERTMIN] = g_param_spec_int("TempAlertMin", "Battery temperature min", NULL, 0, G_MAXINT32, 0, G_PARAM_READWRITE);
    properties[PROPERTY_CAPACITYLEVEL] = g_param_spec_int("CapacityLevel", "Capacity Level", NULL, 0, G_MAXINT32, 0, G_PARAM_READWRITE);

    g_object_class_install_properties(base_class, N_PROPERTY, properties);

    g_signal_new("battery-alert",
			     G_TYPE_FROM_CLASS(bclass),
			     G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			     0,
			     NULL,
			     NULL,
			     g_cclosure_marshal_VOID__INT,
			     G_TYPE_NONE,
			     1,
                 G_TYPE_INT);

    g_signal_new("battery-level",
			     G_TYPE_FROM_CLASS(bclass),
			     G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			     0,
			     NULL,
			     NULL,
			     g_cclosure_marshal_VOID__INT,
			     G_TYPE_NONE,
			     1,
                 G_TYPE_INT);
}

static void bq27541_battery_init(Bq27541Battery *self)
{
    capacity_flag = 1;
    temp_flag = 1;
    level_flag = 2;
    g_timeout_add(1000, battery_monitor, self);
}

gboolean battery_monitor(gpointer data)
{
    gint battery_capacity, battery_temp, battery_current;

    battery_current = get_param_current();
    g_object_get(G_OBJECT(data), "capacity", &battery_capacity, NULL);
    g_object_get(G_OBJECT(data), "temp", &battery_temp, NULL);
    if(battery_current == 65535 && battery_capacity == -1 && battery_temp == 65535)
        return TRUE;

    /* 电量检测 */
    if(battery_capacity >= priv->CapacityAlertMax)
    {
        if(capacity_flag == FLAG_1)
        {
            g_signal_emit_by_name(data, "battery-alert", BATTERY_ALERT_CAPACITY_MAX);
            capacity_flag = FLAG_0;
        }
    }
    else if(battery_capacity <= priv->CapacityAlertMin)
    {
        if(capacity_flag == FLAG_1)
        {
            g_signal_emit_by_name(data, "battery-alert", BATTERY_ALERT_CAPACITY_MIN);
            capacity_flag = FLAG_0;
        }
    }
    else
    {
        capacity_flag = FLAG_1;   //标志为1表示正常范围，0为超限范围
    }
    
    /* 温度检测 */
    if(battery_temp >= priv->TempAlertMax)
    {
        if(temp_flag == FLAG_1)
        {
            g_signal_emit_by_name(data, "battery-alert", BATTERY_ALERT_TEMP_MAX);
            temp_flag = FLAG_0;
        }
    }
    else if(battery_temp <= priv->TempAlertMin)
    {
        if(temp_flag == FLAG_1)
        {
            g_signal_emit_by_name(data, "battery-alert", BATTERY_ALERT_TEMP_MIN);
            temp_flag = FLAG_0;
        }
    }
    else
    {
        temp_flag = FLAG_1;
    }
    
    /* 电量level检测 */
    if(battery_capacity <= LOW_CAPACITY && battery_capacity > CRITICAL_CAPACITY && battery_current < 0)
    {
        if(level_flag == FLAG_2)
        {
            g_signal_emit_by_name(data, "battery-level", BATTERY_CAPACITY_LEVEL_LOW);
            level_flag = FLAG_1;
        }
    }
    else if(battery_capacity <= CRITICAL_CAPACITY && battery_current < 0)
    {
        if(level_flag > 0)
        {
            g_signal_emit_by_name(data, "battery-level", BATTERY_CAPACITY_LEVEL_CRITICAL);
            level_flag = FLAG_0;
        }
    }
    else if(battery_capacity > LOW_CAPACITY || battery_current >= 0)
    {
        level_flag = FLAG_2;   //标志位，2为正常范围，1为低电压提示，0为低电压报警
    }

    return TRUE;
}

//* 获取状态，读取错误返回BATTERY_STATUS_UNKNOWN */
gint get_param_state(gpointer data)
{
    BatteryStatus bq27541_state;
    gchar buf[20] = {0};
    gchar path_buf[100] = {0};
    gint bq27541_capacity;
    gint bq27541_current;

    bq27541_current = get_param_current();
    bq27541_capacity = get_param_capacity();

    sprintf(path_buf, "%s/status", priv->path->str);
    gint status_fd = open(path_buf, O_RDONLY);
    if(status_fd < 0)
    {
        priv->status = BATTERY_STATUS_UNKNOWN;
    	printf("[BQ27541]Open dev status error\n");
        return BATTERY_STATUS_UNKNOWN;
    }

    gint ret = read(status_fd, buf, 15);
    if(ret < 0)
    {
        priv->status = BATTERY_STATUS_UNKNOWN;
    	printf("[BQ27541]Read dev status error\n");
        close(status_fd);
        return BATTERY_STATUS_UNKNOWN;
    }

    if(strncmp(buf, "Charging", 8) == 0)
    {
        bq27541_state = BATTERY_STATUS_CHARGING;
    }
    else if(strncmp(buf, "Discharging", 11) == 0)
    {
        bq27541_state = BATTERY_STATUS_DISCHARGING;
    }
    else if(strncmp(buf, "Full", 4) == 0)
    {
        bq27541_state = BATTERY_STATUS_FULL;
    }

    if((bq27541_current == 0) && (bq27541_capacity < 100))
    {
        bq27541_state = BATTERY_STATUS_NOT_CHARGING;
    }

    priv->status = bq27541_state;
    close(status_fd);

    return bq27541_state;
}

/* 获取电流值，读取错误返回65535 */
gint get_param_current()
{
    gint ret;
    gchar buf[20] = {0};
    gchar path_buf[100] = {0};
    gint bq27541_current;

    sprintf(path_buf, "%s/current_now", priv->path->str);
    gint current_fd = open(path_buf, O_RDONLY);
    if(current_fd < 0)
    {
    	printf("[BQ27541]Open dev current_now error\n");
	    return 65535;
    }
    
    ret = read(current_fd, buf, 9);
    if(ret < 0)
    {
        close(current_fd);
    	printf("[BQ27541]Read dev current_now error\n");
        return 65535;
    }
    bq27541_current = atoi(buf);
    bq27541_current = bq27541_current/1000;

    close(current_fd);
    return bq27541_current;
}

/* 获取温度，读取错误返回65535 */
void get_param_temp()
{
    gint ret;
    gchar buf[20] = {0};
    gchar path_buf[100] = {0};
    gint bq27541_temperature;

    sprintf(path_buf, "%s/temp", priv->path->str);
    gint temp_fd = open(path_buf, O_RDONLY);
    if(temp_fd < 0)
    {
        priv->temp = 65535;
    	printf("[BQ27541]Open dev temp error\n");
	    return;
    }

    ret = read(temp_fd, buf, 5);
    if(ret < 0)
    {
        priv->temp = 65535;
        printf("[BQ27541]Read dev temp error\n");
        close(temp_fd);
	    return;
    }
    bq27541_temperature = atoi(buf);
    priv->temp = bq27541_temperature;

    close(temp_fd);
    return;
}

/* 获取电量值，读取错误返回-1 */
gint get_param_capacity()
{
    gchar buf[20] = {0};
    gchar path_buf[100] = {0};
    gint bq27541_capacity;

    sprintf(path_buf, "%s/capacity", priv->path->str);
    gint capacity_fd = open(path_buf, O_RDONLY);
    if(capacity_fd < 0)
    {
        priv->capacity = -1;
    	printf("[BQ27541]Open dev capacity error\n");
	    return -1;
    }

    int ret = read(capacity_fd, buf, 4);
    if(ret < 0)
    {
        priv->capacity = -1;
        printf("[BQ27541]Read dev capacity error\n");
        close(capacity_fd);
        return -1;
    }
    bq27541_capacity = atoi(buf);
    priv->capacity = bq27541_capacity;

    close(capacity_fd);
    return bq27541_capacity;
}

/* 获取电量级别，读取错误返回BATTERY_CAPACITY_LEVEL_UNKNOWN */
gint get_param_level()
{
    gint level, ret;
    gint length;
    gchar buf[20] = {0}; 
    gchar path_buf[100] = {0};

    gint bq27541_capacity;

    bq27541_capacity = get_param_capacity();

    sprintf(path_buf, "%s/capacity_level", priv->path->str);
    gint level_fd = open(path_buf, O_RDONLY);
    if(level_fd < 0)
    {
        priv->CapacityLevel = BATTERY_CAPACITY_LEVEL_UNKNOWN;
    	printf("[BQ27541]Open dev capacity_level error\n");
	    return BATTERY_CAPACITY_LEVEL_UNKNOWN;
    }

    ret = read(level_fd, buf, 10);
    if(ret < 0)
    {
        priv->CapacityLevel = BATTERY_CAPACITY_LEVEL_UNKNOWN;
        printf("[BQ27541]Read dev capacity_level error\n");
        close(level_fd);
        return BATTERY_CAPACITY_LEVEL_UNKNOWN;
    }

    if(strncmp(buf, "Full", 4) == 0)
    {
        level = BATTERY_CAPACITY_LEVEL_FULL;
    }
    else if(strncmp(buf, "Normal", 6) == 0)
    {
        level = BATTERY_CAPACITY_LEVEL_NORMAL;
    }
    
    if(bq27541_capacity <= LOW_CAPACITY && bq27541_capacity > CRITICAL_CAPACITY)
    {
        level = BATTERY_CAPACITY_LEVEL_LOW;
    }
    else if(bq27541_capacity <= CRITICAL_CAPACITY)
    {
        level = BATTERY_CAPACITY_LEVEL_CRITICAL;
    }

    priv->CapacityLevel = level;

    close(level_fd);
    return level;
}