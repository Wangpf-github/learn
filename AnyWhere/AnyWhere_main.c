#include "AnyWhere_GTK.h"

G_DEFINE_TYPE(AwGtk, aw_gtk, G_TYPE_OBJECT);

#define AW_GTK_GET_PRIVATE(object) G_TYPE_INSTANCE_GET_PRIVATE((object), AW_TYPE_GTK, AwGtkPrivate)

typedef struct _AwGtkPrivate
{
    guint interface;
    guint interface_select;
}AwGtkPrivate;

enum
{
    PROPERTY_0,
    PROPERTY_INTERFACE,
    PROPERTY_INTERFACE_SELECT,
    N_PROPERTY
};

static void aw_gtk_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    AwGtk *self = AW_GTK(object);
    AwGtkPrivate *priv = AW_GTK_GET_PRIVATE(self);

    switch (property_id)
    {
    case PROPERTY_INTERFACE:
        priv->interface = g_value_get_uint(value);
        break;
    case PROPERTY_INTERFACE_SELECT:
        priv->interface_select = g_value_get_uint(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void aw_gtk_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    AwGtk *self = AW_GTK(object);
    AwGtkPrivate *priv = AW_GTK_GET_PRIVATE(self);

    switch (property_id)
    {
    case PROPERTY_INTERFACE:
        g_value_set_uint(value, priv->interface);
        break;
    case PROPERTY_INTERFACE_SELECT:
        g_value_set_uint(value, priv->interface_select);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void aw_gtk_class_init(AwGtkClass *kclass)
{
    g_type_class_add_private(kclass, sizeof(AwGtkPrivate));
    GObjectClass *base_class = G_OBJECT_CLASS(kclass);
    base_class->get_property = aw_gtk_get_property;
    base_class->set_property = aw_gtk_set_property;

    GParamSpec *properties[N_PROPERTY] = {NULL, };
    properties[PROPERTY_INTERFACE] = g_param_spec_uint("interface", "Interface", "Which interface to show", 0, G_MAXUINT32, 0, G_PARAM_READWRITE);
    properties[PROPERTY_INTERFACE_SELECT] = g_param_spec_uint("interface-select", "Interface-select", "Which row to highlight", 0, G_MAXUINT32, 0, G_PARAM_READWRITE);

    g_object_class_install_properties(base_class, N_PROPERTY, properties);
}

static void aw_gtk_init(AwGtk *self)
{

}

static void judge_interface()
{
    guint value;
    g_object_get(GOBJECT(entry), "interface", &value, NULL);

    switch (value)
    {
    case :
        ;
        break;
    case JPEG_ENTRY:
        show_jpeg();
        break;
    case JPEG_STATE:
        show_jpeg_state();
        break;
    case JPEG_SETTING_1:
        show_jpeg_setting1();
        break;
    case JPEG_SETTING_SHOOT:
        show_jpeg_shoot_setting();
        break;
    case MP4_ENTRY:
        show_mp4();
        break;
    case MP4_STATE:
        show_mp4_state();
        break;
    case MP4_SETTING_1:
        show_mp4_setting1();
        break;
    case MP4_SETTING_2:
        show_mp4_setting2();
        break;
    case MP4_SETTING_SEPARATE_1:
        show_mp4_setting_separate1();
        break;
    case MP4_SETTING_SEPARATE_2:
        show_mp4_setting_separate2();
        break;
    case LIVE_ENTRY:
        show_live();
        break;
    case LIVE_STATE:
        show_live_state();
        break;
    case LIVE_SETTING_1:
        show_live_setting1();
        break;
    case LIVE_SETTING_2:
        show_live_setting2();
        break;
    case LIVE_SETTING_RESOLUTION:
        show_live_setting_resolution();
        break;
    case LIVE_SETTING_BITRATE:
        show_live_setting_bitrate();
        break;
    case LIVE_SETTING_FRAMERATE:
        show_live_setting_framerate();
        break;
    case LIVE_SETTING_PROTOCOL:
        show_live_setting_protocol();
        break;
    case SYS_ENTRY:
        show_system();
        break;
    case SYS_SETTING_1:
        show_sys_setting1();
        break;
    case SYS_SETTING_WIFI:
        show_sys_setting_wifi();
        break;
    case SYS_SETTING_LED:
        show_sys_setting_led();
        break;

    case CAMERA_SHUTDOWN:
        OLED_SHUTDOWN();
        break;
    
    default:
        break;
    }
}

void judge_interface_select()
{
    guint value;
    g_object_get(GOBJECT(entry), "interface", &value, NULL);
    g_object_set(GOBJECT(entry), "interface", value, NULL);
}

gint OLED_START()
{
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *image;

    gtk_init(NULL, NULL);
    g_type_init();
    entry = g_object_new(AW_TYPE_GTK, "interface", 0, "interface_select", 1, NULL);

    window = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_window_set_title(GTK_WINDOW(window), "AnyWhere");
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_window_set_default_size(GTK_WINDOW(window), 128, 64);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    g_signal_connect (entry, "notify::interface", G_CALLBACK(judge_interface), NULL);     //采用信号机制，收到信号调用show_jpeg_mode
    g_signal_connect (entry, "notify::interface_select", G_CALLBACK(judge_interface_select), NULL);
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);     //创建新盒子，第二个参数为单元格之间的间隙，即控件之间的间隔大小，一般为0
    gtk_container_add(GTK_CONTAINER(window),vbox);
     
    image = gtk_image_new_from_file("./pictures/start.png");   //加载图片的路径
    gtk_box_pack_start(GTK_BOX(vbox), image, FALSE, FALSE, 0);

    gtk_widget_show_all(window);
    gtk_main();

    return FALSE;
}

gint OLED_SHUTDOWN()
{
    GtkWidget *window;
    GtkWidget *image;
    window = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_window_set_title(GTK_WINDOW(window), "AnyWhere");
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_window_set_default_size(GTK_WINDOW(window), 128, 64);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    image = gtk_image_new_from_file("关机图片加载路径");   //加载图片的路径
    gtk_container_add(GTK_CONTAINER(window), image);
    gtk_widget_show_all(window);
    g_object_unref(entry);

    return FALSE;
}