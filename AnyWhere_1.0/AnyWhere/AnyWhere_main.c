#include "AnyWhere_GTK.h"

G_DEFINE_TYPE(AwGtk, aw_gtk, G_TYPE_OBJECT);

#define AW_GTK_GET_PRIVATE(object) G_TYPE_INSTANCE_GET_PRIVATE((object), AW_TYPE_GTK, AwGtkPrivate)

typedef struct _AwGtkPrivate
{
    guint interface;
    GList *jpeg_list;  
    GList *MP4_list;    
    GList *live_list;      
    GList *system_list;  
    gint interface_select;            //界面控制

    guint network_mode;
    gchar *IPaddr;
    guint battery;

    gchar *jpeg_disk_space_3;        //拍照模式，还能拍照的数量
    gchar *jpeg_resolution_3;        //拍照模式分辨率，按照设计，只在状态显示页面使用
    gchar *jpeg_shoot_mode_3;        //拍照模式： 标准/连拍
    gchar *jpeg_format_3;            //照片存储格式，一期只有JPEG，后面加入DNG

    gchar *mp4_work_time_6;          //录像模式工作时长
    gchar *mp4_resolution_6;         //录像模式分辨率：8K
    gchar *mp4_bitrate_6;            //录像模式码率：100Mb
    gchar *mp4_framerate_6;          //录像模式帧率：30PFS

    gchar *live_work_time_9;         //直播模式工作时间
    gchar *live_resolution_9;        //直播模式分辨率： 4K/6K/8K
    gchar *live_bitrate_9;           //直播模式码率： 5M/10M/15M/30M/40M/50M/60M
    gchar *live_framerate_9;         //直播模式帧率： 15FPS/20FPS/25FPS/30PFS/60FPS

    gchar *image_url;                //图片加载路径
}AwGtkPrivate;

enum
{
    PROPERTY_0,
    PROPERTY_INTERFACE,
    PROPERTY_JPEG_LIST,
    PROPERTY_MP4_LIST,
    PROPERTY_LIVE_LIST,
    PROPERTY_SYSTEM_LIST,
    PROPERTY_INTERFACE_SELECT,
    PROPERTY_NETWORK_MODE,
    PROPERTY_IPADDR,
    PROPERTY_BATTERY,
    PROPERTY_JPEG_DISK_SPACE,
    PROPERTY_JPEG_RESOLUTION,
    PROPERTY_JPEG_SHOOT,
    PROPERTY_JPEG_FOMAT,
    PROPERTY_MP4_WORKTIME,
    PROPERTY_MP4_RESOLUTION,
    PROPERTY_MP4_BITRATE,
    PROPERTY_MP4_FRAMETATE,
    PROPERTY_LIVE_WORKTIME,
    PROPERTY_LIVE_RESOLUTION,
    PROPERTY_LIVE_BITRATE,
    PROPERTY_LIVE_FRAMETATE,
    PROPERTY_IMAGE_URL,
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
    //4个list
    case PROPERTY_JPEG_LIST:
        priv->jpeg_list = g_list_copy(g_value_get_pointer(value));
        break;
    case PROPERTY_MP4_LIST:
        priv->MP4_list = g_list_copy(g_value_get_pointer(value));
        break;
    case PROPERTY_LIVE_LIST:
        priv->live_list = g_list_copy(g_value_get_pointer(value));
        break;
    case PROPERTY_SYSTEM_LIST:
        priv->system_list = g_list_copy(g_value_get_pointer(value));
        break;
    case PROPERTY_INTERFACE_SELECT:
        priv->interface_select = g_value_get_int(value);
        break;
    case PROPERTY_NETWORK_MODE:
        priv->network_mode = g_value_get_uint(value);
        break;
    case PROPERTY_IPADDR:
        priv->IPaddr = g_value_get_pointer(value);
        break;
    case PROPERTY_BATTERY:
        priv->battery = g_value_get_uint(value);
        break;
    case PROPERTY_JPEG_DISK_SPACE:
        priv->jpeg_disk_space_3 = g_value_get_pointer(value);
        break;
    case PROPERTY_JPEG_RESOLUTION:
        priv->jpeg_resolution_3 = g_value_get_pointer(value);
        break;
    case PROPERTY_JPEG_SHOOT:
        priv->jpeg_shoot_mode_3 = g_value_get_pointer(value);
        break;
    case PROPERTY_JPEG_FOMAT:
        priv->jpeg_format_3 = g_value_get_pointer(value);
        break;
    case PROPERTY_MP4_WORKTIME:
        priv->mp4_work_time_6 = g_value_get_pointer(value);
        break;
    case PROPERTY_MP4_RESOLUTION:
        priv->mp4_resolution_6 = g_value_get_pointer(value);
        break;
    case PROPERTY_MP4_BITRATE:
        priv->mp4_bitrate_6 = g_value_get_pointer(value);
        break;
    case PROPERTY_MP4_FRAMETATE:
        priv->mp4_framerate_6 = g_value_get_pointer(value);
        break;
    case PROPERTY_LIVE_WORKTIME:
        priv->live_work_time_9 = g_value_get_pointer(value);
        break;
    case PROPERTY_LIVE_RESOLUTION:
        priv->live_resolution_9 = g_value_get_pointer(value);
        break;
    case PROPERTY_LIVE_BITRATE:
        priv->live_bitrate_9 = g_value_get_pointer(value);
        break;
    case PROPERTY_LIVE_FRAMETATE:
        priv->live_framerate_9 = g_value_get_pointer(value);
        break;
    case PROPERTY_IMAGE_URL:
        priv->image_url = g_value_get_pointer(value);
    
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
    //4个glist
    case PROPERTY_JPEG_LIST:
        g_value_set_pointer(value, priv->jpeg_list);
        break;
    case PROPERTY_MP4_LIST:
        g_value_set_pointer(value, priv->MP4_list);
        break;
    case PROPERTY_LIVE_LIST:
        g_value_set_pointer(value, priv->live_list);
        break;
    case PROPERTY_SYSTEM_LIST:
        g_value_set_pointer(value, priv->system_list);
        break;
    case PROPERTY_INTERFACE_SELECT:
        g_value_set_int(value, priv->interface_select);
        break;
    case PROPERTY_NETWORK_MODE:
        g_value_set_uint(value, priv->network_mode);
        break;
    case PROPERTY_IPADDR:
        g_value_set_pointer(value, priv->IPaddr);
        break;
    case PROPERTY_BATTERY:
        g_value_set_uint(value, priv->battery);
        break;
    case PROPERTY_JPEG_DISK_SPACE:
        g_value_set_pointer(value, priv->jpeg_disk_space_3);
        break;
    case PROPERTY_JPEG_RESOLUTION:
        g_value_set_pointer(value, priv->jpeg_resolution_3);
        break;
    case PROPERTY_JPEG_SHOOT:
        g_value_set_pointer(value, priv->jpeg_shoot_mode_3);
        break;
    case PROPERTY_JPEG_FOMAT:
        g_value_set_pointer(value, priv->jpeg_format_3);
        break;
    case PROPERTY_MP4_WORKTIME:
        g_value_set_pointer(value, priv->mp4_work_time_6);
        break;
    case PROPERTY_MP4_RESOLUTION:
        g_value_set_pointer(value, priv->mp4_resolution_6);
        break;
    case PROPERTY_MP4_BITRATE:
        g_value_set_pointer(value, priv->mp4_bitrate_6);
        break;
    case PROPERTY_MP4_FRAMETATE:
        g_value_set_pointer(value, priv->mp4_framerate_6);
        break;
    case PROPERTY_LIVE_WORKTIME:
        g_value_set_pointer(value, priv->live_work_time_9);
        break;
    case PROPERTY_LIVE_RESOLUTION:
        g_value_set_pointer(value, priv->live_resolution_9);
        break;
    case PROPERTY_LIVE_BITRATE:
        g_value_set_pointer(value, priv->live_bitrate_9);
        break;
    case PROPERTY_LIVE_FRAMETATE:
        g_value_set_pointer(value, priv->live_framerate_9);
        break;
    case PROPERTY_IMAGE_URL:
        g_value_set_pointer(value, priv->image_url;
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
    //4个list
    properties[PROPERTY_JPEG_LIST] = g_param_spec_pointer("jpeg_list",NULL, NULL, G_PARAM_READWRITE);
    properties[PROPERTY_MP4_LIST] = g_param_spec_pointer("MP4_list", NULL, NULL, NULL, G_PARAM_READWRITE);
    properties[PROPERTY_LIVE_LIST] = g_param_spec_pointer("live_list", NULL, NULL, NULL, G_PARAM_READWRITE);
    properties[PROPERTY_SYSTEM_LIST] = g_param_spec_pointer("system_list", NULL, NULL, NULL, G_PARAM_READWRITE);
    properties[PROPERTY_INTERFACE_SELECT] = g_param_spec_int("interface_select", NULL, NULL, 0, G_MAXUINT32, 0, G_PARAM_READWRITE);
    properties[PROPERTY_NETWORK_MODE] = g_param_spec_uint("network_mode", NULL, NULL, 0, G_MAXUINT32, 0, G_PARAM_READWRITE);
    properties[PROPERTY_IPADDR] = g_param_spec_pointer("IPaddr", NULL, NULL,G_PARAM_READWRITE);
    properties[PROPERTY_BATTERY] = g_param_spec_uint("battery", NULL, NULL, 0, G_MAXUINT32, BATTERY_100, G_PARAM_READWRITE);
    properties[PROPERTY_JPEG_DISK_SPACE] = g_param_spec_pointer("jpeg_disk_space_3", NULL, NULL, G_PARAM_READWRITE);
    properties[PROPERTY_JPEG_RESOLUTION] = g_param_spec_pointer("jpeg_resolution_3", NULL, NULL, G_PARAM_READWRITE);
    properties[PROPERTY_JPEG_SHOOT] = g_param_spec_pointer("jpeg_shoot_mode_3", NULL, NULL, G_PARAM_READWRITE);
    properties[PROPERTY_JPEG_FOMAT] = g_param_spec_pointer("jpeg_format_3", NULL, NULL, G_PARAM_READWRITE);
    properties[PROPERTY_MP4_WORKTIME] = g_param_spec_pointer("mp4_work_time_6", NULL, NULL, G_PARAM_READWRITE);
    properties[PROPERTY_MP4_RESOLUTION] = g_param_spec_pointer("mp4_resolution_6", NULL, NULL, G_PARAM_READWRITE);
    properties[PROPERTY_MP4_BITRATE] = g_param_spec_pointer("mp4_bitrate_6", NULL, NULL, G_PARAM_READWRITE);
    properties[PROPERTY_MP4_FRAMETATE] = g_param_spec_pointer("mp4_framerate_6", NULL, NULL, G_PARAM_READWRITE);
    properties[PROPERTY_LIVE_WORKTIME] = g_param_spec_pointer("live_work_time_9", NULL, NULL, G_PARAM_READWRITE);
    properties[PROPERTY_LIVE_RESOLUTION] = g_param_spec_pointer("live_resolution_9", NULL, NULL, G_PARAM_READWRITE);
    properties[PROPERTY_LIVE_BITRATE] = g_param_spec_pointer("live_bitrate_9", NULL, NULL, G_PARAM_READWRITE);
    properties[PROPERTY_LIVE_FRAMETATE] = g_param_spec_pointer("live_framerate_9", NULL, NULL, G_PARAM_READWRITE);
    properties[PROPERTY_IMAGE_URL] = g_param_spec_pointer("image_url", NULL, NULL, G_PARAM_READWRITE);

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
    case CAMERA_SHUTDOWN:
        OLED_SHUTDOWN();
        break;
    case JPEG_MAIN:
        show_main(WORKMODE_JPEG);
        break;
    case JPEG_STATE:
        show_jpeg_state();
        break;
    case JPEG_SETTING:
        show_jpeg_setting();
        break;
    case MP4_MAIN:
        show_main(WORKMODE_MP4);
        break;
    case MP4_STATE:
        show_mp4_state();
        break;
    case MP4_SETTING:
        show_mp4_setting();
        break;
    case LIVE_MAIN:
        show_main(WORKMODE_LIVE);
        break;
    case LIVE_STATE:
        show_live_state();
        break;
    case LIVE_SETTING:
        show_live_setting();
        break;
    case SYS_MAIN:
        show_main(WORKMODE_SETTING);
        break;
    case SYS_SETTING:
        show_sys_setting();
        break;

    default:
        break;
    }
}
static void judge_interface_select()
{
    gint value;
    g_object_get(GOBJECT(entry), "interface", &value, NULL);
    g_object_set(GOBJECT(entry), "interface", value, NULL);
}
gint OLED_START()
{
    entry = g_object_new(AW_TYPE_GTK, NULL);
    int fbfd = 0;
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	long int screensize = 0;
	int bits_per_pixel = 1;
	cairo_linuxfb_device_t *device;
	device = malloc(sizeof(*device));
	//打开显示设备
	fbfd = open("/dev/fb0", O_RDWR);
	if (fbfd < 0)
	{
		printf("fbfd is %d\n",fbfd);
		printf("Error: cannot open framebuffer device.\n");
		exit(1);
	}
	//计算屏幕的总大小（字节）
	screensize = 128 * 64 * bits_per_pixel/8;
	//对象映射
	char *fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
	if ((int)fbp == -1)
	{
		printf("Error: failed to map framebuffer device to memory.\n");
		exit(4);
	}
    
    cairo_surface_t *surface = cairo_image_surface_create_for_data((unsigned char*)fbp, CAIRO_FORMAT_A1, WIDTH, HIGH, 
																		cairo_format_stride_for_width(CAIRO_FORMAT_A1,WIDTH));
    cairo_surface_set_user_data(surface, NULL, device, &cairo_linuxfb_surface_destroy);
    
    g_signal_connect (entry, "notify::interface", G_CALLBACK(judge_interface), NULL);     //采用信号机制，收到信号调用show_jpeg_mode
    g_signal_connect (entry, "notify::interface_select", G_CALLBACK(judge_interface_select), NULL);
//    g_signal_connect (entry, "notify::interface_select", G_CALLBACK(judge_interface_select), NULL);
    
    g_object_get(GOBJECT(entry), "image_url", &image_route, NULL);
    cairo_t cr1 = cairo_create(surface);
    cairo_t cr2 = cairo_create(surface);
    cairo_select_font_face(cr1, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr1, 26.0);
    cairo_set_source_rgb(cr1, 1.0, 1.0, 1.0); 
    cairo_move_to(cr1, 35.0, 30.0);     //剩余空间位置
    cairo_show_text(cr1, "TECHE");
    cairo_select_font_face(cr2, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr2, 18.0);
    cairo_set_source_rgb(cr2, 1.0, 1.0, 1.0); 
    cairo_move_to(cr2, 15.0, 50.0);     //剩余空间位置
    cairo_show_text(cr2, "360 AnyWhere");

    cairo_destroy(cr1);
    cairo_destroy(cr2);
    while(1)
    {};
    return TRUE;
}

gint OLED_SHUTDOWN()
{
    cairo_t cr1 = cairo_create(surface);
    cairo_select_font_face(cr1, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr1, 26.0);
    cairo_set_source_rgb(cr1, 1.0, 1.0, 1.0); 
    cairo_move_to(cr1, 30.0, 40.0);     //剩余空间位置
    cairo_show_text(cr1, "Shutdown");
    cairo_destroy(cr1);
    return TRUE;
}

gint show_main(guint mode)
{
    gchar *ip_addr;
    g_object_get(G_OBJECT (entry), "IPaddr", &ip_addr, NULL);
    gchar urlbuf_workmode[50] = {0};
    cairo_t *cr1 = cairo_create(surface);     //网络工作模式，图片
    cairo_t *cr2 = cairo_create(surface);     //电量，图片
    cairo_t *cr3 = cairo_create(surface);     //工作模式，图片
    cairo_t *cr4 = cairo_create(surface);     //IP时地址，字符串
    cairo_t *cr5 = cairo_create(surface);
	cairo_set_operator(cr5, CAIRO_OPERATOR_CLEAR);
	cairo_rectangle (cr5, 0, 0, 128, 128);
	cairo_fill(cr5);
	cairo_stroke(cr5);
    cairo_surface_t *image1 = cairo_image_surface_create_from_png (get_network_image_url());
	cairo_surface_t *image2 = cairo_image_surface_create_from_png (get_battery_image_url());
	cairo_surface_t *image3 = cairo_image_surface_create_from_png (get_work_mode_image_big_url(mode));
    cairo_set_source_surface (cr1, image1, 1, 4);
    cairo_set_source_surface (cr2, image2, 108, 4);
    cairo_set_source_surface (cr3, image3, 48, 25);

    cairo_paint(cr1);
    cairo_select_font_face(cr4, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr4, 12.0);
    cairo_set_source_rgb(cr4, 0.0, 0.0, 0.0); 
    cairo_move_to(cr4, 16.0, 13.0);     //座标，以左下角为基准
    cairo_show_text(cr4, ip_addr);
    cairo_paint(cr2);
    cairo_paint(cr3);

    cairo_destroy(cr1);
	cairo_destroy(cr2);
	cairo_destroy(cr3);
	cairo_destroy(cr4);
    cairo_destroy(cr5);
    cairo_surface_destroy(image1);
    cairo_surface_destroy(image2);
    cairo_surface_destroy(image3);

    return TRUE;
}