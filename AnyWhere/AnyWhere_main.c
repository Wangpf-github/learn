#include "AnyWhere_GTK.h"

/* 进入系统为开机界面然后跳转到拍照模式 */
G_DEFINE_TYPE(AwGtk, aw_gtk, G_TYPE_OBJECT);

#define AW_GTK_GET_PRIVATE(object) G_TYPE_INSTANCE_GET_PRIVATE((object), AW_TYPE_GTK, AwGtkPrivate)

typedef struct _AwGtkPrivate
{
    guint camera_mode;   //开机时显示开机画面，完成后进入显示拍照页面，关机显示关机页面
    guint work_mode;  //对应工作模式，对应拍照、录像、直播和工作模式设置
    
    guint jpeg_mode;  //对应拍照状态显示和设置两个状态
    guint jpeg_setting_mode; //对应拍照设置的各个选项
    gboolean jpeg_shoot_entry;  //进入拍照设置模式
    guint jpeg_shoot_mode;  //拍照模式， 对应标准和连拍
    gboolean jpeg_raw_entry;  //进入raw设置命令
    guint jpeg_raw_mode;  //对应RAW的开关模式
    gboolean jpeg_expo_entry;

    guint mp4_mode;  //对应录像模式显示和设置两个状态
    gboolean mp4_resolution_entry;
    guint mp4_resolution_mode;
    gboolean mp4_bitrate_entry;
    guint mp4_bitrate_mode;
    gboolean mp4_fps_entry;
    guint mp4_fps_mode;
    gboolean mp4_separate_entey;
    guint mp4_separate_mode;

    /* 以下为设置曝光属性和状态的成员 */
    guint expo_mode;  //对应拍照模式的曝光模式
    guint expo_auto;  //自动模式两种状态
}AwGtkPrivate;

enum
{
    PROPERTY_0,
    ...,
    N_PROPERTY
};

static void aw_gtk_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    AwGtk *self = AW_GTK(object);
    AwGtkPrivate *priv = AW_GTK_GET_PRIVATE(self);
}

static void aw_gtk_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    AwGtk *self = AW_GTK(object);
    AwGtkPrivate *priv = AW_GTK_GET_PRIVATE(self);
}

static void aw_gtk_class_init(AwGtkClass *kclass)
{
    g_type_class_add_private(kclass, sizeof(AwGtkPrivate));
    GObjectClass *base_class = G_OBJECT_CLASS(kclass);
    base_class->get_property = aw_gtk_get_property;
    base_class->set_property = aw_gtk_set_property;

    GParamSpec *properties[N_PROPERTY] = {NULL, };
    properties[?] = g_param_spec_uint("");
    //...

    g_object_class_install_properties(base_class, N_PROPERTY, properties);
}

static void aw_gtk_init(AwGtk *self)
{

}

int OLED_START(int argc,char *argv[])
{
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *image;
    gtk_init(&argc, &argv);
    g_type_init();
    entry = g_object_new(AW_TYPE_GTK, ..., NULL);
    window = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_window_set_title(GTK_WINDOW(window), "AnyWhere");
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_window_set_default_size(GTK_WINDOW(window), 128, 64);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    g_signal_connect (G_OBJECT(window), "开机几秒后收信号", G_CALLBACK(show_jpeg), NULL);     //采用信号机制，收到信号调用show_jpeg_mode

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);     //创建新盒子，第二个参数为单元格之间的间隙，即控件之间的间隔大小，一般为0
    gtk_container_add(GTK_CONTAINER(window),vbox);
     
    image = gtk_image_new_from_file("./pictures/start.png");   //加载图片的路径
    gtk_box_pack_start(GTK_BOX(vbox), image, FALSE, FALSE, 0);

    gtk_widget_show_all(window);

    //可能情况：开机2秒，自己发信号跳转到jpeg模式

    gtk_main();

    return FALSE;
}