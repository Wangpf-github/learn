//export PATH=/home/wang/hi3559av_buitroot/buildroot-env/output/host/bin:$PATH
//export PKG_CONFIG_PATH=/home/linux/git/buildroot-env/output/host/lib/pkgconfig/:$PATH
//编译：aarch64-buildroot-linux-gnu-gcc led_light.c -shared -fPIC -o libled.so `pkg-config --cflags --libs glib-2.0 gobject-2.0 libgpiod` -I.

#include <led_light.h>

#define LED_LIGHT_GET_PRIVATE(object) G_TYPE_INSTANCE_GET_PRIVATE((object), TYPE_LED_LIGHT, LedLightPrivate)
typedef struct gpiod_line GpiodLine;

typedef struct _LedLightPrivate
{
    GpiodLine *gpio;
    gint state;
    gint pattern;
    gint BlinkDefaultState;
    gint BlinkCount;      //周期数量
    gint BlinkInterval;   //周期：单位ms
    gint BlinkDuration;   //闪烁初始状态持续时间
}LedLightPrivate;

G_DEFINE_TYPE_WITH_CODE (LedLight, led_light, G_TYPE_OBJECT, G_ADD_PRIVATE (LedLight))

LedLightPrivate *priv;
gint gpioset_ret;
gint default_flag;
gint opposite_flag;  //判断闪烁次数标志位
guint default_id;
guint opposite_id;
guint opposite_continue_id;  //event source ID

enum
{
    PROPERTY_0,
    PROPERTY_GPIO,
    PROPERTY_STATE,
    PRPERTY_PATTERN,
    PROPERTY_BLINK_DEFAULT_STATE,
    PROPERTY_BLINK_COUNT,
    PROPERTY_BLINK_INTERVAL,
    PROPERTY_BLINK_DURATION,
    N_PROPERTY
};

void judge_led_state(gpointer data);
void judge_led_pattern(gpointer data);

static void led_light_dispose (GObject *gobject)
{
        LedLight *self = LED_LIGHT (gobject);
        priv = LED_LIGHT_GET_PRIVATE (self);

        G_OBJECT_CLASS (led_light_parent_class)->dispose (gobject);
}
 
static void led_light_finalize (GObject *gobject)
{       
        LedLight *self = LED_LIGHT (gobject);
        priv = LED_LIGHT_GET_PRIVATE (self);

        G_OBJECT_CLASS (led_light_parent_class)->finalize (gobject);
}

static void led_light_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    LedLight *self = LED_LIGHT(object);
    priv = LED_LIGHT_GET_PRIVATE(self);

    switch (property_id)
    {
    case PROPERTY_GPIO:
        priv->gpio = (GpiodLine *) g_value_get_pointer(value);
        break;
    case PROPERTY_STATE:
        priv->state = g_value_get_int(value);
        break;
    case PRPERTY_PATTERN:
        priv->pattern = g_value_get_int(value);
        break;
    case PROPERTY_BLINK_DEFAULT_STATE:
        priv->BlinkDefaultState = g_value_get_int(value);
        break;
    case PROPERTY_BLINK_COUNT:
        priv->BlinkCount = g_value_get_int(value);
        break;
    case PROPERTY_BLINK_INTERVAL:
        priv->BlinkInterval = g_value_get_int(value);
        break;
    case PROPERTY_BLINK_DURATION:
        priv->BlinkDuration = g_value_get_int(value);
        break;
    
    default:
        break;
    }
}

static void led_light_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    LedLight *self = LED_LIGHT(object);
    priv = LED_LIGHT_GET_PRIVATE(self);

    switch (property_id)
    {
    case PROPERTY_GPIO:
        g_value_set_pointer(value, priv->gpio);
        break;
    case PROPERTY_STATE:
        g_value_set_int(value, priv->state);
        break;
    case PRPERTY_PATTERN:
        g_value_set_int(value, priv->pattern);
        break;
    case PROPERTY_BLINK_DEFAULT_STATE:
        g_value_set_int(value, priv->BlinkDefaultState);
        break;
    case PROPERTY_BLINK_COUNT:
        g_value_set_int(value, priv->BlinkCount);
        break;
    case PROPERTY_BLINK_INTERVAL:
        g_value_set_int(value, priv->BlinkInterval);
        break;
    case PROPERTY_BLINK_DURATION:
        g_value_set_int(value, priv->BlinkDuration);
        break;
    
    default:
        break;
    }
}

static void led_light_class_init(LedLightClass *lclass)
{
    GObjectClass *base_class = G_OBJECT_CLASS(lclass);
    base_class->get_property = led_light_get_property;
    base_class->set_property = led_light_set_property;
    base_class->dispose = led_light_dispose;
    base_class->finalize = led_light_finalize;

    GParamSpec *properties[N_PROPERTY] = {NULL, };
    properties[PROPERTY_GPIO] = g_param_spec_pointer("gpio", NULL, NULL, G_PARAM_READWRITE);
    properties[PROPERTY_STATE] = g_param_spec_int("state", NULL, NULL, 0, G_MAXINT32, 0, G_PARAM_READWRITE);
    properties[PRPERTY_PATTERN] = g_param_spec_int("pattern", NULL, NULL, 0, G_MAXINT32, 0, G_PARAM_READWRITE);
    properties[PROPERTY_BLINK_DEFAULT_STATE] = g_param_spec_int("BlinkDefaultState", NULL, NULL, 0, G_MAXINT32, 0, G_PARAM_READWRITE);
    properties[PROPERTY_BLINK_COUNT] = g_param_spec_int("BlinkCount", NULL, NULL, 0, G_MAXINT32, 0, G_PARAM_READWRITE);
    properties[PROPERTY_BLINK_INTERVAL] = g_param_spec_int("BlinkInterval", NULL, NULL, 0, G_MAXINT32, 0, G_PARAM_READWRITE);
    properties[PROPERTY_BLINK_DURATION] = g_param_spec_int("BlinkDuration", NULL, NULL, 0, G_MAXINT32, 0, G_PARAM_READWRITE);

    g_object_class_install_properties(base_class, N_PROPERTY, properties);

    g_signal_new("blink-end",
			     G_TYPE_FROM_CLASS(lclass),
			     G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			     0,
			     NULL,
			     NULL,
			     g_cclosure_marshal_VOID__VOID,
			     G_TYPE_NONE,
			     0);

}

static void led_light_init(LedLight *self)
{
    g_signal_connect(self, "notify::state", G_CALLBACK(judge_led_state), self);
    g_signal_connect(self, "notify::pattern", G_CALLBACK(judge_led_pattern), self);
}

void judge_led_state(gpointer data)
{
    if(priv->state == LED_STATE_OFF)   //判断LED状态
    {
        gpioset_ret = gpiod_line_set_value(priv->gpio, LED_STATE_OFF);
        if (gpioset_ret < 0)
        {
            perror("gpiod_line_set_value");
            return;
        }
    }
    else if(priv->state == LED_STATE_ON)
    {
        gpioset_ret = gpiod_line_set_value(priv->gpio, LED_STATE_ON);
        if (gpioset_ret < 0)
        {
            perror("gpiod_line_set_value");
            return;
        }
    }
    else
    {
        g_print("LED state param wrong!\n");
        return ;
    }
    return ;
}

gboolean blink_default_start(gpointer data)
{
    LedPattern led_pattern;
    LedState led_state;
    led_pattern = LED_PATTERN_NONE;

    if(gpioset_ret < 0)  //判断gpio设置是否成功
    {
        return FALSE;        
    }

    if(default_flag++ == priv->BlinkCount)  //闪烁正常完成
    {
        led_state = priv->BlinkDefaultState;
        g_object_set(G_OBJECT(data), "state", led_state, NULL);
        g_object_set(G_OBJECT(data), "pattern", led_pattern, NULL);
        g_signal_emit_by_name(data, "blink-end");
        return FALSE;
    }
    
    if (priv->pattern == LED_PATTERN_NONE)  //闪烁被停止闪烁信号打断
    {
        g_object_set(G_OBJECT(data), "state", priv->state, NULL);
        g_signal_emit_by_name(data, "blink-end");
        return FALSE;
    }

    led_state = priv->BlinkDefaultState;
    g_object_set(G_OBJECT(data), "state", led_state, NULL);

    return TRUE;
}

gboolean blink_opposite_continue(gpointer data)
{
    LedState led_state;

    if(gpioset_ret < 0)  //判断gpio设置是否成功
    {
        return FALSE;        
    }

    if(opposite_flag++ == priv->BlinkCount)
    {
        return FALSE;
    }

    if (priv->pattern == LED_PATTERN_NONE)   
    {
        g_object_set(G_OBJECT(data), "state", priv->BlinkDefaultState, NULL);
        return FALSE;
    }
    
    if (priv->BlinkDefaultState == LED_STATE_OFF)
    {
        led_state = LED_STATE_ON;
        g_object_set(G_OBJECT(data), "state", led_state, NULL);
    }
    else if(priv->BlinkDefaultState == LED_STATE_ON)
    {
        led_state = LED_STATE_OFF;
        g_object_set(G_OBJECT(data), "state", led_state, NULL);
    }
    else
    {
        g_print("BlinkDefaultState param wrong!\n");
        return FALSE;
    }

    return TRUE;
}

gboolean blink_opposite_start(gpointer data)
{
    LedState led_state;

    if(gpioset_ret < 0)  //判断gpio设置是否成功
    {
        return FALSE;        
    }

    opposite_flag++;
    
    if (priv->BlinkDefaultState == LED_STATE_OFF)
    {
        led_state = LED_STATE_ON;
        g_object_set(G_OBJECT(data), "state", led_state, NULL);
    }
    else if(priv->BlinkDefaultState == LED_STATE_ON)
    {
        led_state = LED_STATE_OFF;
        g_object_set(G_OBJECT(data), "state", led_state, NULL);
    }
    else
    {
        g_print("BlinkDefaultState param wrong!\n");
        return FALSE;
    }

    opposite_continue_id = g_timeout_add(priv->BlinkInterval, blink_opposite_continue, data);

    return FALSE;
}

void judge_led_pattern(gpointer data)
{
    LedState led_state;
    GSource *d_source, *o_source;
    if(priv->pattern == LED_PATTERN_BLINK)   //判断闪烁模式
    {
        if (priv->BlinkInterval <= priv->BlinkDuration)
        {
            g_print("BlinkDuration should less then BlinkInterval!\n");
            return ;
        }
        /* 检测闪烁过程中时候被另一个闪烁打断，如打断执行新的闪烁周期 */
        d_source = g_main_context_find_source_by_id (NULL, default_id);
        if(d_source)
        {
            g_source_destroy(d_source);
            o_source = g_main_context_find_source_by_id (NULL, opposite_id);
            if(o_source)
            {
                g_source_destroy(o_source);
            }
            o_source = g_main_context_find_source_by_id (NULL, opposite_continue_id);
            if(o_source)
            {
                g_source_destroy(o_source);
            }
            g_signal_emit_by_name(data, "blink-end");
        }
        /* 第一次设定默认状态 */
        led_state = priv->BlinkDefaultState;
        g_object_set(G_OBJECT(data), "state", led_state, NULL);
        
        default_flag = 1;
        opposite_flag = 0;

        default_id = g_timeout_add(priv->BlinkInterval, blink_default_start, data);
        opposite_id = g_timeout_add(priv->BlinkDuration, blink_opposite_start, data);
    }
    else if(priv->pattern == LED_PATTERN_NONE)
    {

    }
    else
    {
        g_print("LED pattern param wrong!\n");
        return ;
    }

    return ;
}