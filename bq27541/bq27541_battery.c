//export PATH=/home/wang/hi3559av_buitroot/buildroot-env/output/host/bin:$PATH
//export PKG_CONFIG_PATH=/home/linux/git/buildroot-env/output/host/lib/pkgconfig/:$PATH
//编译：aarch64-buildroot-linux-gnu-gcc bq27541_battery.c bq27541_test.c -o battery_test `pkg-config --cflags --libs glib-2.0 gobject-2.0`
#include "bq27541_battery.h"

#define BQ27541_BATTERY_GET_PRIVATE(object) G_TYPE_INSTANCE_GET_PRIVATE((object), BQ27541_TYPE_BATTERY, Bq27541BatteryPrivate)

typedef struct _Bq27541BatteryPrivate
{
    GString *path;
    gint status;
    gint capacity;     
    gint CapacityAlertMax;
    gint CapacityAlertMin;
    gint temp;             //单位0.1℃（摄氏度）
    gint TempAlertMax;     //单位0.1℃（摄氏度）
    gint TempAlertMin;     //单位0.1℃（摄氏度）

}Bq27541BatteryPrivate;

G_DEFINE_TYPE_WITH_CODE (Bq27541Battery, bq27541_battery, G_TYPE_OBJECT, G_ADD_PRIVATE (Bq27541Battery))

Bq27541BatteryPrivate *priv;

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
    N_PROPERTY
};

gboolean battery_monitor(gpointer data);
gint get_param_state(gpointer data);
void get_param_temp();
gint get_param_capacity();

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
        priv->status = get_param_state(self);
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
    properties[PROPERTY_CAPACITYALERTMAX] = g_param_spec_int("CapacityAlertMax", "CapacityAlertMax", NULL, 0, G_MAXINT32, 0, G_PARAM_READWRITE);
    properties[PROPERTY_CAPACITYALERTMIN] = g_param_spec_int("CapacityAlertMin", "CapacityAlertMin", NULL, 0, G_MAXINT32, 0, G_PARAM_READWRITE);
    properties[PROPERTY_TEMP] = g_param_spec_int("temp", "Batterytemperature", NULL, 0, G_MAXINT32, 0, G_PARAM_READWRITE);
    properties[PROPERTY_TEMPALERTMAX] = g_param_spec_int("TempAlertMax", "Battery temperature max", NULL, 0, G_MAXINT32, 0, G_PARAM_READWRITE);
    properties[PROPERTY_TEMPALERTMIN] = g_param_spec_int("TempAlertMin", "Battery temperature max", NULL, 0, G_MAXINT32, 0, G_PARAM_READWRITE);

    g_object_class_install_properties(base_class, N_PROPERTY, properties);

    g_signal_new("buttery_alert",
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
    g_timeout_add(1000, battery_monitor, self);
}

gboolean battery_monitor(gpointer data)
{
    int battery_capacity, battery_temp;
    
    g_object_get(G_OBJECT(data), "capacity", &battery_capacity, NULL);
    if(battery_capacity > priv->CapacityAlertMax)
    {
        g_signal_emit_by_name(data, "buttery_alert", BATTERY_CAPACITY_MAX);
    }
    else if(battery_capacity < priv->CapacityAlertMin)
    {
        g_signal_emit_by_name(data, "buttery_alert", BATTERY_CAPACITY_MIN);
    }

    g_object_get(G_OBJECT(data), "temp", &battery_temp, NULL);
    if(battery_temp > priv->TempAlertMax)
    {
        g_signal_emit_by_name(data, "buttery_alert", BATTERY_TEMP_MAX);
    }
    else if(battery_temp < priv->TempAlertMin)
    {
        g_signal_emit_by_name(data, "buttery_alert", BATTERY_TEMP_MIN);
    }

    return TRUE;
}

gint get_param_state(gpointer data)
{
    gint i;
    BatteryStatus tmp;
    gchar buf[20];
    gchar path_buf[100];
    gint bq27541_capacity;
    glong bq27541_current;
    gint bq27541_length;
    gchar bq27541_status[15];

    sprintf(path_buf, "%s/current_now", priv->path->str);
    int current_fd = open(path_buf, O_RDONLY);
    if(current_fd < 0)
    {
    	printf("open dev current_now error\n");
	    return -1;
    }

    sprintf(path_buf, "%s/status", priv->path->str);
    int status_fd = open(path_buf, O_RDONLY);
    if(status_fd < 0)
    {
        tmp = BATTERY_UNKNOWN;
    	printf("open dev status error\n");
    }

    memset(buf, 0x0, 20);
    read(current_fd, buf, 9);
    bq27541_current = atoi(buf);
    bq27541_current = bq27541_current/1000;

    memset(buf, 0x0, 20);
    memset(bq27541_status, 0x0, 10);
    read(status_fd, buf, 15);
    bq27541_length = strlen(buf) - 1;
    for(i = 0; i < bq27541_length; i++)
    {
        bq27541_status[i] = buf[i];	
    }

    if(strcmp(bq27541_status, "Charging") == 0)
    {
        tmp = BATTERY_CHARGING;
    }
    else if(strcmp(bq27541_status, "Discharging") == 0)
    {
        tmp = BATTERY_DISCHARGING;
    }
    else if(strcmp(bq27541_status, "Full") == 0)
    {
        tmp = BATTERY_FULL;
    }

    if((bq27541_current == 0) && (get_param_capacity() < 99))
    {
        tmp = BATTERY_NOT_CHARGING;
    }

    close(current_fd);
    close(status_fd);
    return tmp;
}

void get_param_temp()
{
    gchar buf[20];
    gchar path_buf[100];
    gint bq27541_temperature;

    sprintf(path_buf, "%s/temp", priv->path->str);
    gint temp_fd = open(path_buf, O_RDONLY);
    if(temp_fd < 0)
    {
    	printf("open dev temp error\n");
	    return;
    }

    memset(buf, 0x0, 20);
    read(temp_fd, buf, 4);
    bq27541_temperature = atoi(buf);
    priv->temp = bq27541_temperature;

    close(temp_fd);
    return;
}

gint get_param_capacity()
{
    gchar buf[20];
    gchar path_buf[100];
    gint bq27541_capacity;

    sprintf(path_buf, "%s/capacity", priv->path->str);
    gint capacity_fd = open(path_buf, O_RDONLY);
    if(capacity_fd < 0)
    {
    	printf("open dev capacity error\n");
	    return -1;
    }

    memset(buf, 0x0, 20);
    read(capacity_fd, buf, 4);
    bq27541_capacity = atoi(buf);
    priv->capacity = bq27541_capacity;

    close(capacity_fd);
    return bq27541_capacity;
}
