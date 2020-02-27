//export PATH=/home/wang/hi3559av_buitroot/buildroot-env/output/host/bin:$PATH
//export PKG_CONFIG_PATH=/home/linux/git/buildroot-env/output/host/lib/pkgconfig/:$PATH
//编译：aarch64-buildroot-linux-gnu-gcc push_button.c button_test.c -o button_test -lpthread `pkg-config --cflags --libs glib-2.0 gobject-2.0 libgpiod` -I/home/wang/hi3559av_buitroot/buildroot-env/output/build/libgpiod-1.4.1/include

#include "push_button.h"

#define PUSH_BUTTON_GET_PRIVATE(object) G_TYPE_INSTANCE_GET_PRIVATE((object), PUSH_TYPE_BUTTON, PushButtonPrivate)

typedef struct _PushButtonPrivate
{
    GpiodLine *gpio;
    gint state;
    gint ReleaseTimeout;
    gint PressTimeout;
}PushButtonPrivate;

G_DEFINE_TYPE_WITH_CODE (PushButton, push_button, G_TYPE_OBJECT, G_ADD_PRIVATE (PushButton))

static PushButtonPrivate *priv;
static gint timeout_count;
static GThread *button_thread;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

enum
{
    PROPERTY_0,
    PROPERTY_GPIO,
    PROPERTY_STATE,
    PROPERTY_RELEASETIMEOUT,
    PROPERTY_PRESSTIMEOUT,
    N_PROPERTY
};

gpointer button_monitor(gpointer data);
void delay_time(int delay);

static void push_button_dispose (GObject *gobject)
{
        PushButton *self = PUSH_BUTTON (gobject);
        priv = PUSH_BUTTON_GET_PRIVATE (self);

        G_OBJECT_CLASS (push_button_parent_class)->dispose (gobject);
}
 
static void push_button_finalize (GObject *gobject)
{       
        PushButton *self = PUSH_BUTTON (gobject);
        priv = PUSH_BUTTON_GET_PRIVATE (self);
        pthread_mutex_destroy(&mutex);
        g_thread_unref(button_thread);
        g_thread_join(button_thread);

        G_OBJECT_CLASS (push_button_parent_class)->finalize (gobject);
}

static void push_button_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    PushButton *self = PUSH_BUTTON(object);
    priv = PUSH_BUTTON_GET_PRIVATE(self);

    switch (property_id)
    {
    case PROPERTY_GPIO:
        priv->gpio = (GpiodLine *) g_value_get_pointer(value);
        break;
    case PROPERTY_STATE:
        priv->state = g_value_get_int(value);
        break;
    case PROPERTY_RELEASETIMEOUT:
        priv->ReleaseTimeout = g_value_get_int(value);
        break;
    case PROPERTY_PRESSTIMEOUT:
        priv->PressTimeout = g_value_get_int(value);
        break;
    
    default:
        break;
    }
}

static void push_button_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    PushButton *self = PUSH_BUTTON(object);
    priv = PUSH_BUTTON_GET_PRIVATE(self);

    switch (property_id)
    {
    case PROPERTY_GPIO:
        g_value_set_pointer(value, priv->gpio);
        break;
    case PROPERTY_STATE:
        g_value_set_int(value, priv->state);
        break;
    case PROPERTY_RELEASETIMEOUT:
        g_value_set_int(value, priv->ReleaseTimeout);
        break;
    case PROPERTY_PRESSTIMEOUT:
        g_value_set_int(value, priv->PressTimeout);
        break;
    
    default:
        break;
    }
}

static void push_button_class_init(PushButtonClass *pclass)
{
    GObjectClass *base_class = G_OBJECT_CLASS(pclass);
    base_class->get_property = push_button_get_property;
    base_class->set_property = push_button_set_property;

    GParamSpec *properties[N_PROPERTY] = {NULL, };
    properties[PROPERTY_GPIO] = g_param_spec_pointer("gpio", NULL, NULL, G_PARAM_READWRITE);
    properties[PROPERTY_STATE] = g_param_spec_int("state", NULL, NULL, 0, G_MAXINT32, 0, G_PARAM_READWRITE);
    properties[PROPERTY_RELEASETIMEOUT] = g_param_spec_int("ReleaseTimeout", NULL, NULL, -2, G_MAXINT32, 0, G_PARAM_READWRITE);
    properties[PROPERTY_PRESSTIMEOUT] = g_param_spec_int("PressTimeout", NULL, NULL, -2, G_MAXINT32, 0, G_PARAM_READWRITE);

    g_object_class_install_properties(base_class, N_PROPERTY, properties);

    g_signal_new("button_event",
			     G_TYPE_FROM_CLASS(pclass),
			     G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			     0,
			     NULL,
			     NULL,
			     NULL,
			     G_TYPE_NONE,
			     2,
                 G_TYPE_INT,
                 G_TYPE_INT);
}

static void push_button_init(PushButton *self)
{
    button_thread = g_thread_new("monitor", button_monitor, self);
}

gpointer release_timeout_start(gpointer data)   //松开状态计时
{
    gpointer foo = NULL;
    gint release = 0;    //松开状态计时时间，毫秒
    g_signal_emit_by_name(data, "button_event", BUTTON_EVENT_RELEASE, timeout_count);
    if (priv->ReleaseTimeout < 1)
    {
        printf("Press timeout function close!\n");
        g_thread_exit(NULL);
    }

    while (1)
    {
        delay_time(48);
        if(priv->state == BUTTON_STATE_PRESSED)
            break;
        
        release += 50;
        if(release % (priv->ReleaseTimeout * 1000) == 0)
        {
            pthread_mutex_lock(&mutex);
            timeout_count++;
            g_signal_emit_by_name(data, "button_event", BUTTON_EVENT_RELEASE, timeout_count);
            pthread_mutex_unlock(&mutex);
        }
    }
    g_thread_exit(NULL);
}

gpointer press_timeout_start(gpointer data)    //按下状态计时
{
    gpointer foo = NULL;
    gint press = 0;    //按下状态计时时间，毫秒
    g_signal_emit_by_name(data, "button_event", BUTTON_EVENT_PRESS, timeout_count);
    if (priv->PressTimeout < 1)
    {
        printf("Press timeout function close!\n");
        g_thread_exit(NULL);
    }

    while (1)
    {
        delay_time(48);
        if(priv->state == BUTTON_STATE_RELEASED)
            break;
        
        press += 50;
        if(press % (priv->PressTimeout * 1000) == 0)
        {
            pthread_mutex_lock(&mutex);
            timeout_count++;
            g_signal_emit_by_name(data, "button_event", BUTTON_EVENT_PRESS, timeout_count);
            pthread_mutex_unlock(&mutex);
        }
    }
    g_thread_exit(NULL);
}

gpointer button_monitor(gpointer data)
{
    gint button_fd;
    int ret ;
    ButtonState button_state;
    struct gpiod_line_event evt;   //事件发生后，存储事件结果的结构体
    
    while (1)
    {
        gpiod_line_request_both_edges_events(priv->gpio, NULL);

        ret = gpiod_line_event_wait(priv->gpio, NULL);
        if(ret < 0)
        {
            perror("gpiod_line_event_wait");
            return NULL;
        }
        if(ret == 1)
        {
            button_fd = gpiod_line_event_get_fd(priv->gpio);
            
            ret = gpiod_line_event_read_fd(button_fd, &evt);
            if(ret < 0)
            {
                perror("gpiod_line_event_read_fd");
                return NULL;
            }
            if(ret == 0)
            {
                if(evt.event_type == GPIOD_LINE_EVENT_RISING_EDGE)
                {
                    button_state = BUTTON_STATE_RELEASED;
                    g_object_set(G_OBJECT(data), "state", button_state, NULL);
                    timeout_count = 0;
                    g_thread_new("func", release_timeout_start, data); 
                }
                else
                {
                    button_state = BUTTON_STATE_PRESSED;
                    g_object_set(G_OBJECT(data), "state", button_state, NULL);
                    timeout_count = 0;
                    g_thread_new("func", press_timeout_start, data); 
                }
            }
        }
        delay_time(100);
    }
}

void delay_time(int delay)
{
    usleep(delay * 1000);
    return ;
}
