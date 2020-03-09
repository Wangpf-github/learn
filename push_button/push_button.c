//export PATH=/home/wang/hi3559av_buitroot/buildroot-env/output/host/bin:$PATH
//export PKG_CONFIG_PATH=/home/linux/git/buildroot-env/output/host/lib/pkgconfig/:$PATH
//编译：aarch64-buildroot-linux-gnu-gcc push_button.c -shared -fPIC -o libpush_button.so -lpthread `pkg-config --cflags --libs glib-2.0 gobject-2.0 libgpiod` -I. -I/home/wang/hi3559av_buitroot/buildroot-env/output/build/libglib2-2.60.7/glib/

#include <push_button.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <gpoll.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define PUSH_BUTTON_GET_PRIVATE(object) G_TYPE_INSTANCE_GET_PRIVATE((object), TYPE_PUSH_BUTTON, PushButtonPrivate)

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
static gint event_fd;

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

static void push_button_dispose(GObject *gobject)
{
    PushButton *self = PUSH_BUTTON(gobject);
    priv = PUSH_BUTTON_GET_PRIVATE(self);

    G_OBJECT_CLASS(push_button_parent_class)->dispose(gobject);
}
 
static void push_button_finalize(GObject *gobject)
{       
    PushButton *self = PUSH_BUTTON(gobject);
    priv = PUSH_BUTTON_GET_PRIVATE(self);
    write(event_fd, "thread flag event", 18);
    close(event_fd);

    G_OBJECT_CLASS(push_button_parent_class)->finalize(gobject);
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
    base_class->dispose = push_button_dispose;
    base_class->finalize = push_button_finalize;

    GParamSpec *properties[N_PROPERTY] = {NULL, };
    properties[PROPERTY_GPIO] = g_param_spec_pointer("gpio", NULL, NULL, G_PARAM_READWRITE);
    properties[PROPERTY_STATE] = g_param_spec_int("state", NULL, NULL, 0, G_MAXINT32, 0, G_PARAM_READWRITE);
    properties[PROPERTY_RELEASETIMEOUT] = g_param_spec_int("ReleaseTimeout", NULL, NULL, -2, G_MAXINT32, 0, G_PARAM_READWRITE);
    properties[PROPERTY_PRESSTIMEOUT] = g_param_spec_int("PressTimeout", NULL, NULL, -2, G_MAXINT32, 0, G_PARAM_READWRITE);

    g_object_class_install_properties(base_class, N_PROPERTY, properties);

    g_signal_new("button-event",
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
    g_thread_new("monitor", button_monitor, self);
}

gboolean release_timeout_start(gpointer data)   //松开状态计时
{
    timeout_count++;
    g_signal_emit_by_name(data, "button-event", BUTTON_EVENT_RELEASE, timeout_count);

    return TRUE;
}

gboolean press_timeout_start(gpointer data)    //按下状态计时
{
    timeout_count++;
    g_signal_emit_by_name(data, "button-event", BUTTON_EVENT_PRESS, timeout_count);
    
    return TRUE;
}

gpointer button_monitor(gpointer data)
{
    gint button_fd;
    static GMutex button_mutex;
    int ret, poll_ret;
    gint last_event = 0;
    guint press_id = 0;
    guint release_id = 0;
    ButtonState button_state;
    struct gpiod_line_event evt;   //事件发生后，存储事件结果的结构体
    GPollFD fds[2] = {0};

    g_mutex_init(&button_mutex);
    event_fd = eventfd(0, EFD_SEMAPHORE);
    fds[0].fd = event_fd;
    fds[0].events = G_IO_IN | G_IO_HUP | G_IO_ERR;

    while (1)
    {
        g_mutex_lock(&button_mutex);
        gpiod_line_request_both_edges_events(priv->gpio, NULL);
        button_fd = gpiod_line_event_get_fd(priv->gpio);
        g_mutex_unlock(&button_mutex);
        fds[1].fd = button_fd;
        fds[1].events = G_IO_IN | G_IO_HUP | G_IO_ERR;

        poll_ret = g_poll(fds, 2, -1);   //阻塞轮询两个fd
        if(poll_ret > 0)
        {
            if(fds[1].revents & G_IO_IN)
            {
                ret = gpiod_line_event_read_fd(button_fd, &evt);
                if(ret < 0)
                {
                    perror("gpiod_line_event_read_fd");
                    continue;
                }
                if(ret == 0)    //读取寄存器正确
                {
                    if(evt.event_type == GPIOD_LINE_EVENT_RISING_EDGE)   //检测到上升沿
                    {
                        if(last_event == GPIOD_LINE_EVENT_RISING_EDGE)   //防抖检测
                            continue;

                        last_event = GPIOD_LINE_EVENT_RISING_EDGE;
                        button_state = BUTTON_STATE_RELEASED;
                        g_object_set(G_OBJECT(data), "state", button_state, NULL);
                        
                        if(press_id > 0)    //关闭按下状态超时检测
                        {
                            g_source_remove(press_id);
                        }

                        g_mutex_lock(&button_mutex);
                        timeout_count = 0;
                        g_signal_emit_by_name(data, "button-event", BUTTON_EVENT_RELEASE, timeout_count);
                        g_mutex_unlock(&button_mutex);

                        if (priv->ReleaseTimeout < 1)
                        {
                            g_print("Release timeout function close!\n");
                            continue;
                        }
                        release_id = g_timeout_add(priv->ReleaseTimeout * 1000, release_timeout_start, data); 
                    }
                    else     //检测到下降沿
                    {
                        if(last_event == GPIOD_LINE_EVENT_FALLING_EDGE)   //防抖检测
                            continue;

                        last_event = GPIOD_LINE_EVENT_FALLING_EDGE;
                        button_state = BUTTON_STATE_PRESSED;
                        g_object_set(G_OBJECT(data), "state", button_state, NULL);

                        if(release_id > 0)    //关闭松开状态超时检测
                        {
                            g_source_remove(release_id);
                        }

                        g_mutex_lock(&button_mutex);
                        timeout_count = 0;
                        g_signal_emit_by_name(data, "button-event", BUTTON_EVENT_PRESS, timeout_count);
                        g_mutex_unlock(&button_mutex);

                        if (priv->PressTimeout < 1)
                        {
                            g_print("Press timeout function close!\n");
                            continue;
                        }
                        press_id = g_timeout_add(priv->PressTimeout * 1000, press_timeout_start, data); 
                    }
                }
            }
            else if(fds[0].revents & G_IO_IN)   //检测到event_fd有变化，结束循环，线程退出
            {
                break;
            }
        }
        if(poll_ret < 1)
        {
            continue;
        }
    }

    g_print("Button monitor thread exit!\n");
    g_mutex_clear(&button_mutex);
    g_thread_exit(NULL);
}
