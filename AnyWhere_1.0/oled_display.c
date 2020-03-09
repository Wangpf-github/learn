//export PATH=/home/wang/hi3559av_buitroot/buildroot-env/output/host/bin:$PATH
//export PKG_CONFIG_PATH=/home/linux/git/buildroot-env/output/host/lib/pkgconfig/:$PATH
//aarch64-buildroot-linux-gnu-gcc oled_display.c -shared -fPIC -o liboled_display.so `pkg-config --libs --cflags cairo glib-2.0 gobject-2.0` -I.

#include <oled_display.h>
#include <cairo/cairo.h>
#include <fcntl.h>
#include <linux/types.h>
#include <linux/ioctl.h>
#include <linux/fb.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>

#define OLED_SCREEN_GET_PRIVATE(object) G_TYPE_INSTANCE_GET_PRIVATE((object), TYPE_OLED_SCREEN, OledScreenPrivate)
#define WIDTH  (128)
#define HIGH   (64)

typedef struct _OledScreenPrivate
{
    guint interface;
    GList *JpegList;  
    GList *VideoList;    
    GList *LiveList;      
    GList *SystemList;  
    gint InterfaceSelect;            //界面控制

    guint NetworkMode;
    gchar *IPaddr;
    guint battery;

    gchar *JpegDiskSpace;        //拍照模式，还能拍照的数量
    gchar *JpegResolution;        //拍照模式分辨率，按照设计，只在状态显示页面使用
    gchar *JpegShootMode;        //拍照模式： 标准/连拍
    gchar *JpegFormat;            //照片存储格式，一期只有JPEG，后面加入DNG

    gchar *VideoWorkTime;          //录像模式工作时长
    gchar *VideoResolution;         //录像模式分辨率：8K
    gchar *VideoBitrate;            //录像模式码率：100Mb
    gchar *VideoFramerate;          //录像模式帧率：30PFS

    gchar *LiveWorkTime;         //直播模式工作时间
    gchar *LiveResolution;        //直播模式分辨率： 4K/6K/8K
    gchar *LiveBitrate;           //直播模式码率： 5M/10M/15M/30M/40M/50M/60M
    gchar *LiveFramerate;         //直播模式帧率： 15FPS/20FPS/25FPS/30PFS/60FPS

    gchar *ImagePath;                //图片加载路径
}OledScreenPrivate;

G_DEFINE_TYPE_WITH_CODE (OledScreen, oled_screen, G_TYPE_OBJECT, G_ADD_PRIVATE (OledScreen))

/* 全局变量 */
cairo_surface_t *surface;
gchar *image_route;
OledScreenPrivate *priv;
int fbfd = 0;
char *fbp;

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

enum IS_SELECTED
{
    SELECT_NO,
    SELECT_YES
};

/* 函数声明 */
/* 获取显示信息函数 */
gchar *get_network_image_url(gpointer data);                                            //网络模式路径
gchar *get_battery_image_url(gpointer data);                                            //电池图片路径
gchar *get_work_mode_image_big_url(gint work_mode);                        //状态显示页面图片路径
gchar *get_work_mode_image_small_url(gint work_mode, gint select_mode);    //获取设置模式时导航栏的图片路径
gchar *get_angle_url(gint select_mode);
/* 显示页面函数 */
gint show_main(gpointer data, guint mode);
gint show_jpeg_state(gpointer data);
gint show_jpeg_setting(gpointer data);
gint show_mp4_state(gpointer data);
gint show_mp4_setting(gpointer data);
gint show_live_state(gpointer data);
gint show_live_setting(gpointer data);
gint show_sys_setting(gpointer data);
gint oled_start(gpointer data);
gint oled_shutdown();
/* 回调函数 */
static void judge_interface(gpointer data);
static void judge_interface_select(gpointer data);
static void judge_navigat(gpointer data);
static void judge_jpeg_state(gpointer data);
static void judge_mp4_state(gpointer data);
static void judge_live_state(gpointer data);

static void oled_screen_dispose (GObject *gobject)
{
        OledScreen *self = OLED_SCREEN (gobject);
        priv = OLED_SCREEN_GET_PRIVATE (self);

        G_OBJECT_CLASS (oled_screen_parent_class)->dispose (gobject);
}
 
static void oled_screen_finalize (GObject *gobject)
{       
        OledScreen *self        = OLED_SCREEN (gobject);
        priv = OLED_SCREEN_GET_PRIVATE (self);
        cairo_surface_destroy(surface);
        munmap(fbp, (WIDTH * HIGH / 8));
        close(fbfd);

        G_OBJECT_CLASS (oled_screen_parent_class)->finalize (gobject);
}

static void oled_screen_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    OledScreen *self = OLED_SCREEN(object);
    priv = OLED_SCREEN_GET_PRIVATE(self);

    switch (property_id)
    {
    case PROPERTY_INTERFACE:
        priv->interface = g_value_get_uint(value);
        break;
    //4个list
    case PROPERTY_JPEG_LIST:
        priv->JpegList = g_list_copy(g_value_get_pointer(value));
        break;
    case PROPERTY_MP4_LIST:
        priv->VideoList = g_list_copy(g_value_get_pointer(value));
        break;
    case PROPERTY_LIVE_LIST:
        priv->LiveList = g_list_copy(g_value_get_pointer(value));
        break;
    case PROPERTY_SYSTEM_LIST:
        priv->SystemList = g_list_copy(g_value_get_pointer(value));
        break;
    case PROPERTY_INTERFACE_SELECT:
        priv->InterfaceSelect = g_value_get_int(value);
        break;
    case PROPERTY_NETWORK_MODE:
        priv->NetworkMode = g_value_get_uint(value);
        break;
    case PROPERTY_IPADDR:
        priv->IPaddr = g_value_get_pointer(value);
        break;
    case PROPERTY_BATTERY:
        priv->battery = g_value_get_uint(value);
        break;
    case PROPERTY_JPEG_DISK_SPACE:
        priv->JpegDiskSpace = g_value_get_pointer(value);
        break;
    case PROPERTY_JPEG_RESOLUTION:
        priv->JpegResolution = g_value_get_pointer(value);
        break;
    case PROPERTY_JPEG_SHOOT:
        priv->JpegShootMode = g_value_get_pointer(value);
        break;
    case PROPERTY_JPEG_FOMAT:
        priv->JpegFormat = g_value_get_pointer(value);
        break;
    case PROPERTY_MP4_WORKTIME:
        priv->VideoWorkTime = g_value_get_pointer(value);
        break;
    case PROPERTY_MP4_RESOLUTION:
        priv->VideoResolution = g_value_get_pointer(value);
        break;
    case PROPERTY_MP4_BITRATE:
        priv->VideoBitrate = g_value_get_pointer(value);
        break;
    case PROPERTY_MP4_FRAMETATE:
        priv->VideoFramerate = g_value_get_pointer(value);
        break;
    case PROPERTY_LIVE_WORKTIME:
        priv->LiveWorkTime = g_value_get_pointer(value);
        break;
    case PROPERTY_LIVE_RESOLUTION:
        priv->LiveResolution = g_value_get_pointer(value);
        break;
    case PROPERTY_LIVE_BITRATE:
        priv->LiveBitrate = g_value_get_pointer(value);
        break;
    case PROPERTY_LIVE_FRAMETATE:
        priv->LiveFramerate = g_value_get_pointer(value);
        break;
    case PROPERTY_IMAGE_URL:
        priv->ImagePath = g_value_get_pointer(value);
        break;

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void oled_screen_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    OledScreen *self = OLED_SCREEN(object);
    priv = OLED_SCREEN_GET_PRIVATE(self);

    switch (property_id)
    {
    case PROPERTY_INTERFACE:
        g_value_set_uint(value, priv->interface);
        break;
    //4个glist
    case PROPERTY_JPEG_LIST:
        g_value_set_pointer(value, priv->JpegList);
        break;
    case PROPERTY_MP4_LIST:
        g_value_set_pointer(value, priv->VideoList);
        break;
    case PROPERTY_LIVE_LIST:
        g_value_set_pointer(value, priv->LiveList);
        break;
    case PROPERTY_SYSTEM_LIST:
        g_value_set_pointer(value, priv->SystemList);
        break;
    case PROPERTY_INTERFACE_SELECT:
        g_value_set_int(value, priv->InterfaceSelect);
        break;
    case PROPERTY_NETWORK_MODE:
        g_value_set_uint(value, priv->NetworkMode);
        break;
    case PROPERTY_IPADDR:
        g_value_set_pointer(value, priv->IPaddr);
        break;
    case PROPERTY_BATTERY:
        g_value_set_uint(value, priv->battery);
        break;
    case PROPERTY_JPEG_DISK_SPACE:
        g_value_set_pointer(value, priv->JpegDiskSpace);
        break;
    case PROPERTY_JPEG_RESOLUTION:
        g_value_set_pointer(value, priv->JpegResolution);
        break;
    case PROPERTY_JPEG_SHOOT:
        g_value_set_pointer(value, priv->JpegShootMode);
        break;
    case PROPERTY_JPEG_FOMAT:
        g_value_set_pointer(value, priv->JpegFormat);
        break;
    case PROPERTY_MP4_WORKTIME:
        g_value_set_pointer(value, priv->VideoWorkTime);
        break;
    case PROPERTY_MP4_RESOLUTION:
        g_value_set_pointer(value, priv->VideoResolution);
        break;
    case PROPERTY_MP4_BITRATE:
        g_value_set_pointer(value, priv->VideoBitrate);
        break;
    case PROPERTY_MP4_FRAMETATE:
        g_value_set_pointer(value, priv->VideoFramerate);
        break;
    case PROPERTY_LIVE_WORKTIME:
        g_value_set_pointer(value, priv->LiveWorkTime);
        break;
    case PROPERTY_LIVE_RESOLUTION:
        g_value_set_pointer(value, priv->LiveResolution);
        break;
    case PROPERTY_LIVE_BITRATE:
        g_value_set_pointer(value, priv->LiveBitrate);
        break;
    case PROPERTY_LIVE_FRAMETATE:
        g_value_set_pointer(value, priv->LiveFramerate);
        break;
    case PROPERTY_IMAGE_URL:
        g_value_set_pointer(value, priv->ImagePath);
        break;

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void oled_screen_class_init(OledScreenClass *kclass)
{
    GObjectClass *base_class = G_OBJECT_CLASS(kclass);
    base_class->get_property = oled_screen_get_property;
    base_class->set_property = oled_screen_set_property;
    base_class->dispose      = oled_screen_dispose;
    base_class->finalize     = oled_screen_finalize;

    GParamSpec *properties[N_PROPERTY] = {NULL, };
    properties[PROPERTY_INTERFACE] = g_param_spec_uint("interface", "Interface", "Which interface to show", 0, G_MAXUINT32, 0, G_PARAM_READWRITE);
    //4个list
    properties[PROPERTY_JPEG_LIST] = g_param_spec_pointer("JpegList",NULL, NULL, G_PARAM_READWRITE);
    properties[PROPERTY_MP4_LIST] = g_param_spec_pointer("VideoList", NULL, NULL, G_PARAM_READWRITE);
    properties[PROPERTY_LIVE_LIST] = g_param_spec_pointer("LiveList", NULL, NULL, G_PARAM_READWRITE);
    properties[PROPERTY_SYSTEM_LIST] = g_param_spec_pointer("SystemList", NULL, NULL, G_PARAM_READWRITE);
    properties[PROPERTY_INTERFACE_SELECT] = g_param_spec_int("InterfaceSelect", NULL, NULL, -2, G_MAXINT32, 0, G_PARAM_READWRITE);
    properties[PROPERTY_NETWORK_MODE] = g_param_spec_uint("NetworkMode", NULL, NULL, 0, G_MAXUINT32, 0, G_PARAM_READWRITE);
    properties[PROPERTY_IPADDR] = g_param_spec_pointer("IPaddr", NULL, NULL,G_PARAM_READWRITE);
    properties[PROPERTY_BATTERY] = g_param_spec_uint("battery", NULL, NULL, 0, G_MAXUINT32, BATTERY_100, G_PARAM_READWRITE);
    properties[PROPERTY_JPEG_DISK_SPACE] = g_param_spec_pointer("JpegDiskSpace", NULL, NULL, G_PARAM_READWRITE);
    properties[PROPERTY_JPEG_RESOLUTION] = g_param_spec_pointer("JpegResolution", NULL, NULL, G_PARAM_READWRITE);
    properties[PROPERTY_JPEG_SHOOT] = g_param_spec_pointer("JpegShootMode", NULL, NULL, G_PARAM_READWRITE);
    properties[PROPERTY_JPEG_FOMAT] = g_param_spec_pointer("JpegFormat", NULL, NULL, G_PARAM_READWRITE);
    properties[PROPERTY_MP4_WORKTIME] = g_param_spec_pointer("VideoWorkTime", NULL, NULL, G_PARAM_READWRITE);
    properties[PROPERTY_MP4_RESOLUTION] = g_param_spec_pointer("VideoResolution", NULL, NULL, G_PARAM_READWRITE);
    properties[PROPERTY_MP4_BITRATE] = g_param_spec_pointer("VideoBitrate", NULL, NULL, G_PARAM_READWRITE);
    properties[PROPERTY_MP4_FRAMETATE] = g_param_spec_pointer("VideoFramerate", NULL, NULL, G_PARAM_READWRITE);
    properties[PROPERTY_LIVE_WORKTIME] = g_param_spec_pointer("LiveWorkTime", NULL, NULL, G_PARAM_READWRITE);
    properties[PROPERTY_LIVE_RESOLUTION] = g_param_spec_pointer("LiveResolution", NULL, NULL, G_PARAM_READWRITE);
    properties[PROPERTY_LIVE_BITRATE] = g_param_spec_pointer("LiveBitrate", NULL, NULL, G_PARAM_READWRITE);
    properties[PROPERTY_LIVE_FRAMETATE] = g_param_spec_pointer("LiveFramerate", NULL, NULL, G_PARAM_READWRITE);
    properties[PROPERTY_IMAGE_URL] = g_param_spec_pointer("ImagePath", NULL, NULL, G_PARAM_READWRITE);

    g_object_class_install_properties(base_class, N_PROPERTY, properties);
}

static void oled_screen_init(OledScreen *self)
{
    oled_start(self);
    g_signal_connect (self, "notify::interface", G_CALLBACK(judge_interface), NULL);     //采用信号机制，收到信号调用show_jpeg_mode
    g_signal_connect (self, "notify::InterfaceSelect", G_CALLBACK(judge_interface_select), NULL);
    g_signal_connect (self, "notify::JpegList", G_CALLBACK(judge_interface_select), NULL);
    g_signal_connect (self, "notify::VideoList", G_CALLBACK(judge_interface_select), NULL);
    g_signal_connect (self, "notify::LiveList", G_CALLBACK(judge_interface_select), NULL);
    g_signal_connect (self, "notify::SystemList", G_CALLBACK(judge_interface_select), NULL);
    g_signal_connect (self, "notify::NetworkMode", G_CALLBACK(judge_navigat), NULL);
    g_signal_connect (self, "notify::IPaddr", G_CALLBACK(judge_navigat), NULL);
    g_signal_connect (self, "notify::battery", G_CALLBACK(judge_navigat), NULL);
    g_signal_connect (self, "notify::JpegDiskSpace", G_CALLBACK(judge_jpeg_state), NULL);
    g_signal_connect (self, "notify::JpegResolution", G_CALLBACK(judge_jpeg_state), NULL);
    g_signal_connect (self, "notify::JpegShootMode", G_CALLBACK(judge_jpeg_state), NULL);
    g_signal_connect (self, "notify::JpegFormat", G_CALLBACK(judge_jpeg_state), NULL);
    g_signal_connect (self, "notify::VideoWorkTime", G_CALLBACK(judge_mp4_state), NULL);
    g_signal_connect (self, "notify::VideoResolution", G_CALLBACK(judge_mp4_state), NULL);
    g_signal_connect (self, "notify::VideoBitrate", G_CALLBACK(judge_mp4_state), NULL);
    g_signal_connect (self, "notify::VideoFramerate", G_CALLBACK(judge_mp4_state), NULL);
    g_signal_connect (self, "notify::LiveWorkTime", G_CALLBACK(judge_live_state), NULL);
    g_signal_connect (self, "notify::LiveResolution", G_CALLBACK(judge_live_state), NULL);
    g_signal_connect (self, "notify::LiveBitrate", G_CALLBACK(judge_live_state), NULL);
    g_signal_connect (self, "notify::LiveFramerate", G_CALLBACK(judge_live_state), NULL);
}

static void judge_interface(gpointer data)
{
    guint value;
    g_object_get(G_OBJECT(data), "ImagePath", &image_route, NULL);
    g_object_get(G_OBJECT(data), "interface", &value, NULL);

    switch (value)
    {
    case CAMERA_SHUTDOWN:
        oled_shutdown();
        break;
    case JPEG_MAIN:
        show_main(data, WORKMODE_JPEG);
        break;
    case JPEG_STATE:
        show_jpeg_state(data);
        break;
    case JPEG_SETTING:
        show_jpeg_setting(data);
        break;
    case MP4_MAIN:
        show_main(data, WORKMODE_MP4);
        break;
    case MP4_STATE:
        show_mp4_state(data);
        break;
    case MP4_SETTING:
        show_mp4_setting(data);
        break;
    case LIVE_MAIN:
        show_main(data, WORKMODE_LIVE);
        break;
    case LIVE_STATE:
        show_live_state(data);
        break;
    case LIVE_SETTING:
        show_live_setting(data);
        break;
    case SYS_MAIN:
        show_main(data, WORKMODE_SETTING);
        break;
    case SYS_SETTING:
        show_sys_setting(data);
        break;

    default:
        break;
    }
    return;
}

static void judge_interface_select(gpointer data)
{
    gint value;
    g_object_get(G_OBJECT(data), "interface", &value, NULL);
    g_object_set(G_OBJECT(data), "interface", value, NULL);
    return;
}

static void judge_navigat(gpointer data)
{
    gint value;
    g_object_get(G_OBJECT(data), "interface", &value, NULL);
    switch (value)
    {
    case JPEG_MAIN:
    case JPEG_STATE:
    case MP4_MAIN:
    case MP4_STATE:
    case LIVE_MAIN:
    case LIVE_STATE:
    case SYS_MAIN:
    case SYS_SETTING:
        g_object_set(G_OBJECT(data), "interface", value, NULL);
        break;

    default:
        break;
    }
    return;
}

static void judge_jpeg_state(gpointer data)
{
    gint value;
    g_object_get(G_OBJECT(data), "interface", &value, NULL);
    if (JPEG_STATE == value)
        g_object_set(G_OBJECT(data), "interface", value, NULL);
    return;
}

static void judge_mp4_state(gpointer data)
{
    gint value;
    g_object_get(G_OBJECT(data), "interface", &value, NULL);
    if (MP4_STATE == value)
        g_object_set(G_OBJECT(data), "interface", value, NULL);
    return;
}

static void judge_live_state(gpointer data)
{
    gint value;
    g_object_get(G_OBJECT(data), "interface", &value, NULL);
    if (LIVE_STATE == value)
        g_object_set(G_OBJECT(data), "interface", value, NULL);
    return;
}

gint oled_start(gpointer data)
{
	int screensize = 0;
	int bits_per_pixel = 1;

	//打开显示设备
	fbfd = open("/dev/fb0", O_RDWR);
	if (fbfd < 0)
	{
		printf("Error: cannot open framebuffer device.\n");
		return -1;
	}
	//计算屏幕的总大小（字节）
	screensize = WIDTH * HIGH * bits_per_pixel/8;
	//对象映射
	fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
	if (*fbp == -1)
	{
		printf("Error: failed to map framebuffer device to memory.\n");
		return -1;
	}

    surface = cairo_image_surface_create_for_data((unsigned char*)fbp, CAIRO_FORMAT_A1, WIDTH, HIGH, 
												   cairo_format_stride_for_width(CAIRO_FORMAT_A1,WIDTH));

    return 0;
}

gint oled_shutdown()
{
    cairo_t *cr1 = cairo_create(surface);
    cairo_t *cr2 = cairo_create(surface);
	cairo_set_operator(cr2, CAIRO_OPERATOR_CLEAR);
	cairo_rectangle (cr2, 0, 0, 128, 128);
	cairo_fill(cr2);
	cairo_stroke(cr2);
    cairo_select_font_face(cr1, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr1, 26.0);
    cairo_set_source_rgb(cr1, 1.0, 1.0, 1.0); 
    cairo_move_to(cr1, 14.0, 42.0);     //剩余空间位置
    cairo_show_text(cr1, "Shutdown");

    cairo_destroy(cr1);
    cairo_destroy(cr2);

    return TRUE;
}

gint show_main(gpointer data, guint mode)
{
    gchar *ip_addr;
    g_object_get(G_OBJECT (data), "IPaddr", &ip_addr, NULL);
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
    cairo_surface_t *image1 = cairo_image_surface_create_from_png (get_network_image_url(data));
	cairo_surface_t *image2 = cairo_image_surface_create_from_png (get_battery_image_url(data));
	cairo_surface_t *image3 = cairo_image_surface_create_from_png (get_work_mode_image_big_url(mode));
    cairo_set_source_surface (cr1, image1, 1, 4);
    cairo_set_source_surface (cr2, image2, 108, 5);
    cairo_set_source_surface (cr3, image3, 48, 25);

    cairo_paint(cr1);
    cairo_select_font_face(cr4, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr4, 12.0);
    cairo_set_source_rgb(cr4, 0.0, 0.0, 0.0); 
    cairo_move_to(cr4, 16.0, 14.0);     //座标，以左下角为基准
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

gint show_jpeg_state(gpointer data)
{
    gint disk_len;
    gchar *ip_addr, *disk_space, *shoot;
    g_object_get(G_OBJECT (data), "IPaddr", &ip_addr, "JpegDiskSpace", &disk_space, "JpegShootMode", &shoot, NULL);
    cairo_t *cr1 = cairo_create(surface);     //网络工作模式，图片
    cairo_t *cr2 = cairo_create(surface);     //电量，图片
    cairo_t *cr3 = cairo_create(surface);     //工作模式，图片
    cairo_t *cr4 = cairo_create(surface);     //IP时地址，字符串
    cairo_t *cr5 = cairo_create(surface);     //磁盘剩余空间，字符串
    cairo_t *cr6 = cairo_create(surface);     //分辨率，字符串
    cairo_t *cr7 = cairo_create(surface);     //拍照模式，字符串
    cairo_t *cr8 = cairo_create(surface);     //照片存储格式，字符串
    cairo_t *cr9 = cairo_create(surface);
	cairo_set_operator(cr9, CAIRO_OPERATOR_CLEAR);
	cairo_rectangle (cr9, 0, 0, 128, 128);
	cairo_fill(cr9);
	cairo_stroke(cr9);
    cairo_surface_t *image1 = cairo_image_surface_create_from_png (get_network_image_url(data));
	cairo_surface_t *image2 = cairo_image_surface_create_from_png (get_battery_image_url(data));
	cairo_surface_t *image3 = cairo_image_surface_create_from_png (get_work_mode_image_small_url(WORKMODE_JPEG, SELECT_NO));
    cairo_set_source_surface (cr1, image1, 1, 4);
    cairo_set_source_surface (cr2, image2, 108, 5);
    cairo_set_source_surface (cr3, image3, 4, 25);

    cairo_paint(cr1);
    cairo_select_font_face(cr4, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr4, 12.0);
    cairo_set_source_rgb(cr4, 0.0, 0.0, 0.0); 
    cairo_move_to(cr4, 16.0, 14.0);     //IP位置
    cairo_show_text(cr4, ip_addr);
    cairo_paint(cr2);
    cairo_paint(cr3);
    cairo_select_font_face(cr5, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr5, 22.0);
    cairo_set_source_rgb(cr5, 1.0, 1.0, 1.0); 
    disk_len = strlen(disk_space);
    switch (disk_len)
    {
    case 4:
        cairo_move_to(cr5, 55.0, 39.0);     //剩余空间位置
        break;
    case 5:
        cairo_move_to(cr5, 51.0, 39.0);
        break;
    case 6:
        cairo_move_to(cr5, 45.0, 39.0);
        break;
    case 7:
        cairo_move_to(cr5, 38.0, 39.0);
        break;
    case 8:
        cairo_move_to(cr5, 33.0, 39.0);
        break;
    case 9:
        cairo_move_to(cr5, 28.0, 39.0);
        break;
    
    default:
        break;
    }
    cairo_show_text(cr5, disk_space);
    cairo_select_font_face(cr6, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr6, 14.0);
    cairo_set_source_rgb(cr6, 1.0, 1.0, 1.0); 
    cairo_move_to(cr6, 1.0, 60.0);     //分辨率位置
    cairo_show_text(cr6, "8K");
    cairo_select_font_face(cr7, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr7, 14.0);
    cairo_set_source_rgb(cr7, 1.0, 1.0, 1.0); 
    cairo_move_to(cr7, 25.0, 60.0);     //拍照模式位置
    cairo_show_text(cr7, shoot);
    cairo_select_font_face(cr8, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr8, 14.0);
    cairo_set_source_rgb(cr8, 1.0, 1.0, 1.0); 
    cairo_move_to(cr8, 96.0, 60.0);     //存储格式位置
    cairo_show_text(cr8, "JPEG");

    cairo_destroy(cr1);
	cairo_destroy(cr2);
	cairo_destroy(cr3);
	cairo_destroy(cr4);
    cairo_destroy(cr5);
	cairo_destroy(cr6);
	cairo_destroy(cr7);
	cairo_destroy(cr8);
    cairo_destroy(cr9);
    cairo_surface_destroy(image1);
    cairo_surface_destroy(image2);
    cairo_surface_destroy(image3);

    return TRUE;
}

gint show_jpeg_setting(gpointer data)  //包含拍照模式，连拍间隔，采用Glist
{
    gint select;
    GList *list = NULL;
    gchar *item1, *item2, *item3;
    g_object_get(G_OBJECT (data), "JpegList", &list, "InterfaceSelect", &select, NULL);
    item1 = (gchar *)g_list_nth_data(list, 0);
    item2 = (gchar *)g_list_nth_data(list, 1);
    item3 = (gchar *)g_list_nth_data(list, 2);
    cairo_t *cr1 = cairo_create(surface);     //工作模式，图片
    cairo_t *cr2 = cairo_create(surface);     //三角箭头，图片
    cairo_t *cr3 = cairo_create(surface);     //第一行:拍照模式，字符串
    cairo_t *cr4 = cairo_create(surface);     //第二行:连拍间隔，字符串
    cairo_t *cr5 = cairo_create(surface);     //第三行:done，字符串
    cairo_t *cr6 = cairo_create(surface);     //占位，矩形
    cairo_t *cr7 = cairo_create(surface);     //占位，矩形
    cairo_t *cr8 = cairo_create(surface);
    cairo_set_operator(cr8, CAIRO_OPERATOR_CLEAR);
    cairo_rectangle (cr8, 0, 0, 128, 128);
    cairo_fill(cr8);
    cairo_stroke(cr8);
    cairo_surface_t *image1;
	cairo_surface_t *image2;

    cairo_select_font_face(cr3, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr3, 12.0);
    cairo_set_source_rgb(cr3, 1.0, 1.0, 1.0); 
    cairo_move_to(cr3, 8.0, 36.0); 

    cairo_select_font_face(cr4, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr4, 12.0);
    cairo_set_source_rgb(cr4, 1.0, 1.0, 1.0); 
    cairo_move_to(cr4, 8.0, 48.0);

    cairo_select_font_face(cr5, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr5, 12.0);
    cairo_set_source_rgb(cr5, 1.0, 1.0, 1.0); 
    cairo_move_to(cr5, 8.0, 60.0);

    switch (select)
    {
    case -1:
        image1 = cairo_image_surface_create_from_png (get_work_mode_image_small_url(WORKMODE_JPEG, SELECT_YES));
        image2 = cairo_image_surface_create_from_png (get_angle_url(SELECT_YES));
        cairo_set_source_surface (cr1, image1, 47, 0);
        cairo_set_source_surface (cr2, image2, 73, 0);
        cairo_rectangle (cr6, 0, 0, 47, 28);
        cairo_fill(cr6);
        cairo_stroke(cr6);
        cairo_rectangle (cr6, 67, 0, 6, 28);
        cairo_fill(cr6);
        cairo_stroke(cr6);
        cairo_rectangle (cr6, 81, 0, 55, 28);
        cairo_fill(cr6);
        cairo_stroke(cr6);
        break;
    case SELECT_FIRST:
        image1 = cairo_image_surface_create_from_png (get_work_mode_image_small_url(WORKMODE_JPEG, SELECT_NO));
        image2 = cairo_image_surface_create_from_png (get_angle_url(SELECT_NO));
        cairo_set_source_surface (cr1, image1, 47, 6);
        cairo_set_source_surface (cr2, image2, 73, 7);
        cairo_rectangle (cr7, 0, 26, 128, 12);
        cairo_fill(cr7);
        cairo_stroke(cr7);
    	cairo_set_operator(cr3, CAIRO_OPERATOR_CLEAR);
        break;
    case SELECT_SECOND:
        image1 = cairo_image_surface_create_from_png (get_work_mode_image_small_url(WORKMODE_JPEG, SELECT_NO));
        image2 = cairo_image_surface_create_from_png (get_angle_url(SELECT_NO));
        cairo_set_source_surface (cr1, image1, 47, 6);
        cairo_set_source_surface (cr2, image2, 73, 7);
        cairo_rectangle (cr7, 0, 38, 128, 12);
        cairo_fill(cr7);
        cairo_stroke(cr7);
        cairo_set_operator(cr4, CAIRO_OPERATOR_CLEAR);
        break;
    case SELECT_THIRD:
        image1 = cairo_image_surface_create_from_png (get_work_mode_image_small_url(WORKMODE_JPEG, SELECT_NO));
        image2 = cairo_image_surface_create_from_png (get_angle_url(SELECT_NO));
        cairo_set_source_surface (cr1, image1, 47, 6);
        cairo_set_source_surface (cr2, image2, 73, 7);
        cairo_rectangle (cr7, 0, 50, 128, 12);
        cairo_fill(cr7);
        cairo_stroke(cr7);
    	cairo_set_operator(cr5, CAIRO_OPERATOR_CLEAR);
        break;

    default:
        break;
    }

    cairo_paint(cr1);
    cairo_paint(cr2);
    cairo_set_operator(cr6, CAIRO_OPERATOR_CLEAR);
    cairo_rectangle (cr6, 0, 0, 128, 4);
    cairo_fill(cr6);
    cairo_stroke(cr6);
    if (-1 == select)
    {
        cairo_rectangle (cr6, 0, 24, 128, 4);
        cairo_fill(cr6);
        cairo_stroke(cr6);
    }
    cairo_show_text(cr3, item1);
    cairo_show_text(cr4, item2);
    cairo_show_text(cr5, item3);

    cairo_destroy(cr1);
	cairo_destroy(cr2);
	cairo_destroy(cr3);
	cairo_destroy(cr4);
    cairo_destroy(cr5);
    cairo_destroy(cr6);
    cairo_destroy(cr7);
    cairo_destroy(cr8);
    cairo_surface_destroy(image1);
    cairo_surface_destroy(image2);

    return TRUE;
}
gint show_mp4_state(gpointer data)
{
    gchar *ip_addr, *worktime;
    g_object_get(G_OBJECT (data), "IPaddr", &ip_addr, "VideoWorkTime", &worktime, NULL);
    cairo_t *cr1 = cairo_create(surface);     //网络工作模式，图片
    cairo_t *cr2 = cairo_create(surface);     //电量，图片
    cairo_t *cr3 = cairo_create(surface);     //工作模式，图片
    cairo_t *cr4 = cairo_create(surface);     //IP时地址，字符串
    cairo_t *cr5 = cairo_create(surface);     //磁盘剩余空间，字符串
    cairo_t *cr6 = cairo_create(surface);     //分辨率，字符串
    cairo_t *cr7 = cairo_create(surface);     //拍照模式，字符串
    cairo_t *cr8 = cairo_create(surface);     //照片存储格式，字符串
    cairo_t *cr9 = cairo_create(surface);
	cairo_set_operator(cr9, CAIRO_OPERATOR_CLEAR);
	cairo_rectangle (cr9, 0, 0, 128, 128);
	cairo_fill(cr9);
	cairo_stroke(cr9);
    cairo_surface_t *image1 = cairo_image_surface_create_from_png (get_network_image_url(data));
	cairo_surface_t *image2 = cairo_image_surface_create_from_png (get_battery_image_url(data));
	cairo_surface_t *image3 = cairo_image_surface_create_from_png (get_work_mode_image_small_url(WORKMODE_MP4, SELECT_NO));
    cairo_set_source_surface (cr1, image1, 1, 4);
    cairo_set_source_surface (cr2, image2, 108, 5);
    cairo_set_source_surface (cr3, image3, 4, 25);

    cairo_paint(cr1);
    cairo_select_font_face(cr4, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr4, 12.0);
    cairo_set_source_rgb(cr4, 0.0, 0.0, 0.0); 
    cairo_move_to(cr4, 16.0, 14.0);     //IP位置
    cairo_show_text(cr4, ip_addr);
    cairo_paint(cr2);
    cairo_paint(cr3);
    cairo_select_font_face(cr5, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr5, 22.0);
    cairo_set_source_rgb(cr5, 1.0, 1.0, 1.0); 
    cairo_move_to(cr5, 35.0, 40.0);     //工作时间
    cairo_show_text(cr5, worktime);
    cairo_select_font_face(cr6, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr6, 14.0);
    cairo_set_source_rgb(cr6, 1.0, 1.0, 1.0); 
    cairo_move_to(cr6, 4.0, 60.0);     //分辨率位置
    cairo_show_text(cr6, "8K");
    cairo_select_font_face(cr7, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr7, 14.0);
    cairo_set_source_rgb(cr7, 1.0, 1.0, 1.0); 
    cairo_move_to(cr7, 33.0, 60.0);     //帧率位置
    cairo_show_text(cr7, "30FPS");
    cairo_select_font_face(cr8, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr8, 14.0);
    cairo_set_source_rgb(cr8, 1.0, 1.0, 1.0); 
    cairo_move_to(cr8, 90.0, 60.0);     //码率位置
    cairo_show_text(cr8, "100MB");

    cairo_destroy(cr1);
	cairo_destroy(cr2);
	cairo_destroy(cr3);
	cairo_destroy(cr4);
    cairo_destroy(cr5);
	cairo_destroy(cr6);
	cairo_destroy(cr7);
	cairo_destroy(cr8);
    cairo_destroy(cr9);
    cairo_surface_destroy(image1);
    cairo_surface_destroy(image2);
    cairo_surface_destroy(image3);

    return TRUE;
}

gint show_mp4_setting(gpointer data)
{
    gint select;
    GList *list = NULL;
    gchar *item1 = NULL;
    gchar *item2 = NULL;
    gchar *item3 = NULL;
    g_object_get(G_OBJECT (data), "VideoList", &list, "InterfaceSelect", &select, NULL);
    cairo_t *cr1 = cairo_create(surface);     //工作模式，图片
    cairo_t *cr2 = cairo_create(surface);     //三角箭头，图片
    cairo_t *cr3 = cairo_create(surface);     //第一行:拍照模式，字符串
    cairo_t *cr4 = cairo_create(surface);     //第二行:连拍间隔，字符串
    cairo_t *cr5 = cairo_create(surface);     //第三行:done，字符串
    cairo_t *cr6 = cairo_create(surface);     //占位，矩形
    cairo_t *cr7 = cairo_create(surface);     //占位，矩形
    cairo_t *cr8 = cairo_create(surface);
    cairo_set_operator(cr8, CAIRO_OPERATOR_CLEAR);
    cairo_rectangle (cr8, 0, 0, 128, 128);
    cairo_fill(cr8);
    cairo_stroke(cr8);
    cairo_surface_t *image1;
	cairo_surface_t *image2;

    cairo_select_font_face(cr3, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr3, 12.0);
    cairo_set_source_rgb(cr3, 1.0, 1.0, 1.0); 
    cairo_move_to(cr3, 8.0, 36.0); 

    cairo_select_font_face(cr4, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr4, 12.0);
    cairo_set_source_rgb(cr4, 1.0, 1.0, 1.0); 
    cairo_move_to(cr4, 8.0, 48.0);

    cairo_select_font_face(cr5, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr5, 12.0);
    cairo_set_source_rgb(cr5, 1.0, 1.0, 1.0); 
    cairo_move_to(cr5, 8.0, 60.0);
    if (select <= 2)
    {
        item1 = (gchar *)g_list_nth_data(list, 0);
        item2 = (gchar *)g_list_nth_data(list, 1);
        item3 = (gchar *)g_list_nth_data(list, 2);
        switch (select)
        {
        case -1:
            image1 = cairo_image_surface_create_from_png (get_work_mode_image_small_url(WORKMODE_MP4, SELECT_YES));
            image2 = cairo_image_surface_create_from_png (get_angle_url(SELECT_YES));
            cairo_set_source_surface (cr1, image1, 47, 0);
            cairo_set_source_surface (cr2, image2, 73, 0);
            cairo_rectangle (cr6, 0, 0, 47, 28);
            cairo_fill(cr6);
            cairo_stroke(cr6);
            cairo_rectangle (cr6, 67, 0, 6, 28);
            cairo_fill(cr6);
            cairo_stroke(cr6);
            cairo_rectangle (cr6, 81, 0, 55, 28);
            cairo_fill(cr6);
            cairo_stroke(cr6);
            break;
        case SELECT_FIRST:
            image1 = cairo_image_surface_create_from_png (get_work_mode_image_small_url(WORKMODE_MP4, SELECT_NO));
            image2 = cairo_image_surface_create_from_png (get_angle_url(SELECT_NO));
            cairo_set_source_surface (cr1, image1, 47, 6);
            cairo_set_source_surface (cr2, image2, 73, 7);
            cairo_rectangle (cr7, 0, 26, 128, 12);
            cairo_fill(cr7);
            cairo_stroke(cr7);
            cairo_set_operator(cr3, CAIRO_OPERATOR_CLEAR);
            break;
        case SELECT_SECOND:
            image1 = cairo_image_surface_create_from_png (get_work_mode_image_small_url(WORKMODE_MP4, SELECT_NO));
            image2 = cairo_image_surface_create_from_png (get_angle_url(SELECT_NO));
            cairo_set_source_surface (cr1, image1, 47, 6);
            cairo_set_source_surface (cr2, image2, 73, 7);
            cairo_rectangle (cr7, 0, 38, 128, 12);
            cairo_fill(cr7);
            cairo_stroke(cr7);
            cairo_set_operator(cr4, CAIRO_OPERATOR_CLEAR);
            break;
        case SELECT_THIRD:
            image1 = cairo_image_surface_create_from_png (get_work_mode_image_small_url(WORKMODE_MP4, SELECT_NO));
            image2 = cairo_image_surface_create_from_png (get_angle_url(SELECT_NO));
            cairo_set_source_surface (cr1, image1, 47, 6);
            cairo_set_source_surface (cr2, image2, 73, 7);
            cairo_rectangle (cr7, 0, 50, 128, 12);
            cairo_fill(cr7);
            cairo_stroke(cr7);
            cairo_set_operator(cr5, CAIRO_OPERATOR_CLEAR);
            break;

        default:
            break;
        }
    }
    else
    {
        item1 = (gchar *)g_list_nth_data(list, 3);
        item2 = (gchar *)g_list_nth_data(list, 4);
        switch (select)
        {
        case SELECT_FOURTH:
            image1 = cairo_image_surface_create_from_png (get_work_mode_image_small_url(WORKMODE_MP4, SELECT_NO));
            image2 = cairo_image_surface_create_from_png (get_angle_url(SELECT_NO));
            cairo_set_source_surface (cr1, image1, 47, 6);
            cairo_set_source_surface (cr2, image2, 73, 7);
            cairo_rectangle (cr7, 0, 26, 128, 12);
            cairo_fill(cr7);
            cairo_stroke(cr7);
            cairo_set_operator(cr3, CAIRO_OPERATOR_CLEAR);
            break;
        case SELECT_FIFTH:
            image1 = cairo_image_surface_create_from_png (get_work_mode_image_small_url(WORKMODE_MP4, SELECT_NO));
            image2 = cairo_image_surface_create_from_png (get_angle_url(SELECT_NO));
            cairo_set_source_surface (cr1, image1, 47, 6);
            cairo_set_source_surface (cr2, image2, 73, 7);
            cairo_rectangle (cr7, 0, 38, 128, 12);
            cairo_fill(cr7);
            cairo_stroke(cr7);
            cairo_set_operator(cr4, CAIRO_OPERATOR_CLEAR);
            break;

        default:
            break;
        }
    }

    cairo_paint(cr1);
    cairo_paint(cr2);
    cairo_set_operator(cr6, CAIRO_OPERATOR_CLEAR);
    cairo_rectangle (cr6, 0, 0, 128, 4);
    cairo_fill(cr6);
    cairo_stroke(cr6);
    if (-1 == select)
    {
        cairo_rectangle (cr6, 0, 24, 128, 4);   //删除模式切换选项下4像素
        cairo_fill(cr6);
        cairo_stroke(cr6);
    }
    cairo_show_text(cr3, item1);
    cairo_show_text(cr4, item2);
    cairo_show_text(cr5, item3);

    cairo_destroy(cr1);
	cairo_destroy(cr2);
	cairo_destroy(cr3);
	cairo_destroy(cr4);
    cairo_destroy(cr5);
    cairo_destroy(cr6);
    cairo_destroy(cr7);
    cairo_destroy(cr8);
    cairo_surface_destroy(image1);
    cairo_surface_destroy(image2);

    return TRUE;
}


gint show_live_state(gpointer data)
{
    gint live_state_length;
    gchar *ip_addr, *worktime, *resolution, *bitrate, *framerate;
    g_object_get(G_OBJECT (data), "IPaddr", &ip_addr, "LiveWorkTime", &worktime, "LiveResolution", &resolution,
                                                       "LiveBitrate", &bitrate, "LiveFramerate", &framerate, NULL);
    cairo_t *cr1 = cairo_create(surface);     //网络工作模式，图片
    cairo_t *cr2 = cairo_create(surface);     //电量，图片
    cairo_t *cr3 = cairo_create(surface);     //工作模式，图片
    cairo_t *cr4 = cairo_create(surface);     //IP时地址，字符串
    cairo_t *cr5 = cairo_create(surface);     //磁盘剩余空间，字符串
    cairo_t *cr6 = cairo_create(surface);     //分辨率，字符串
    cairo_t *cr7 = cairo_create(surface);     //拍照模式，字符串
    cairo_t *cr8 = cairo_create(surface);     //照片存储格式，字符串
    cairo_t *cr9 = cairo_create(surface);
	cairo_set_operator(cr9, CAIRO_OPERATOR_CLEAR);
	cairo_rectangle (cr9, 0, 0, 128, 128);
	cairo_fill(cr9);
	cairo_stroke(cr9);
    cairo_surface_t *image1 = cairo_image_surface_create_from_png (get_network_image_url(data));
	cairo_surface_t *image2 = cairo_image_surface_create_from_png (get_battery_image_url(data));
	cairo_surface_t *image3 = cairo_image_surface_create_from_png (get_work_mode_image_small_url(WORKMODE_LIVE, SELECT_NO));
    cairo_set_source_surface (cr1, image1, 1, 4);
    cairo_set_source_surface (cr2, image2, 108, 5);
    cairo_set_source_surface (cr3, image3, 4, 25);

    cairo_paint(cr1);
    cairo_select_font_face(cr4, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr4, 12.0);
    cairo_set_source_rgb(cr4, 0.0, 0.0, 0.0); 
    cairo_move_to(cr4, 16.0, 14.0);     //IP位置
    cairo_show_text(cr4, ip_addr);
    cairo_paint(cr2);
    cairo_paint(cr3);
    cairo_select_font_face(cr5, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr5, 22.0);
    cairo_set_source_rgb(cr5, 1.0, 1.0, 1.0); 
    live_state_length = strlen(worktime);
    switch (live_state_length)
    {
    case 4:
        cairo_move_to(cr5, 55.0, 39.0);     //工作时间
        break;
    case 5:
        cairo_move_to(cr5, 51.0, 39.0);
        break;
    case 6:
        cairo_move_to(cr5, 45.0, 39.0);
        break;
    case 7:
        cairo_move_to(cr5, 38.0, 39.0);
        break;
    case 8:
        cairo_move_to(cr5, 33.0, 39.0);
        break;
    case 9:
        cairo_move_to(cr5, 28.0, 39.0);
        break;
    
    default:
        break;
    }  
    cairo_show_text(cr5, worktime);
    cairo_select_font_face(cr6, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr6, 14.0);
    cairo_set_source_rgb(cr6, 1.0, 1.0, 1.0); 
    cairo_move_to(cr6, 4.0, 60.0);     //分辨率位置
    cairo_show_text(cr6, resolution);
    cairo_select_font_face(cr7, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr7, 14.0);
    cairo_set_source_rgb(cr7, 1.0, 1.0, 1.0); 
    cairo_move_to(cr7, 39.0, 60.0);     //帧率位置
    cairo_show_text(cr7, framerate);
    cairo_select_font_face(cr8, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr8, 14.0);
    cairo_set_source_rgb(cr8, 1.0, 1.0, 1.0); 
    cairo_move_to(cr8, 98.0, 60.0);     //码率位置
    cairo_show_text(cr8, bitrate);

    cairo_destroy(cr1);
	cairo_destroy(cr2);
	cairo_destroy(cr3);
	cairo_destroy(cr4);
    cairo_destroy(cr5);
	cairo_destroy(cr6);
	cairo_destroy(cr7);
	cairo_destroy(cr8);
    cairo_destroy(cr9);
    cairo_surface_destroy(image1);
    cairo_surface_destroy(image2);
    cairo_surface_destroy(image3);

    return TRUE;
}

gint show_live_setting(gpointer data)
{
    gint select;
    GList *list = NULL;
    gchar *item1 = NULL;
    gchar *item2 = NULL;
    gchar *item3 = NULL;
    g_object_get(G_OBJECT (data), "LiveList", &list, "InterfaceSelect", &select, NULL);
    cairo_t *cr1 = cairo_create(surface);     //工作模式，图片
    cairo_t *cr2 = cairo_create(surface);     //三角箭头，图片
    cairo_t *cr3 = cairo_create(surface);     //第一行:拍照模式，字符串
    cairo_t *cr4 = cairo_create(surface);     //第二行:连拍间隔，字符串
    cairo_t *cr5 = cairo_create(surface);     //第三行:done，字符串
    cairo_t *cr6 = cairo_create(surface);     //占位，矩形
    cairo_t *cr7 = cairo_create(surface);     //占位，矩形
    cairo_t *cr8 = cairo_create(surface);
    cairo_set_operator(cr8, CAIRO_OPERATOR_CLEAR);
    cairo_rectangle (cr8, 0, 0, 128, 128);
    cairo_fill(cr8);
    cairo_stroke(cr8);
    cairo_surface_t *image1;
	cairo_surface_t *image2;

    cairo_select_font_face(cr3, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr3, 12.0);
    cairo_set_source_rgb(cr3, 1.0, 1.0, 1.0); 
    cairo_move_to(cr3, 8.0, 36.0); 

    cairo_select_font_face(cr4, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr4, 12.0);
    cairo_set_source_rgb(cr4, 1.0, 1.0, 1.0); 
    cairo_move_to(cr4, 8.0, 48.0);

    cairo_select_font_face(cr5, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr5, 12.0);
    cairo_set_source_rgb(cr5, 1.0, 1.0, 1.0); 
    cairo_move_to(cr5, 8.0, 60.0);
    if (select <= 2)
    {
        item1 = (gchar *)g_list_nth_data(list, 0);
        item2 = (gchar *)g_list_nth_data(list, 1);
        item3 = (gchar *)g_list_nth_data(list, 2);
        switch (select)
        {
        case -1:
            image1 = cairo_image_surface_create_from_png (get_work_mode_image_small_url(WORKMODE_LIVE, SELECT_YES));
            image2 = cairo_image_surface_create_from_png (get_angle_url(SELECT_YES));
            cairo_set_source_surface (cr1, image1, 47, 0);
            cairo_set_source_surface (cr2, image2, 73, 0);
            cairo_rectangle (cr6, 0, 0, 47, 28);
            cairo_fill(cr6);
            cairo_stroke(cr6);
            cairo_rectangle (cr6, 67, 0, 6, 28);
            cairo_fill(cr6);
            cairo_stroke(cr6);
            cairo_rectangle (cr6, 81, 0, 55, 28);
            cairo_fill(cr6);
            cairo_stroke(cr6);
            break;
        case SELECT_FIRST:
            image1 = cairo_image_surface_create_from_png (get_work_mode_image_small_url(WORKMODE_LIVE, SELECT_NO));
            image2 = cairo_image_surface_create_from_png (get_angle_url(SELECT_NO));
            cairo_set_source_surface (cr1, image1, 47, 6);
            cairo_set_source_surface (cr2, image2, 73, 7);
            cairo_rectangle (cr7, 0, 26, 128, 12);
            cairo_fill(cr7);
            cairo_stroke(cr7);
            cairo_set_operator(cr3, CAIRO_OPERATOR_CLEAR);
            break;
        case SELECT_SECOND:
            image1 = cairo_image_surface_create_from_png (get_work_mode_image_small_url(WORKMODE_LIVE, SELECT_NO));
            image2 = cairo_image_surface_create_from_png (get_angle_url(SELECT_NO));
            cairo_set_source_surface (cr1, image1, 47, 6);
            cairo_set_source_surface (cr2, image2, 73, 7);
            cairo_rectangle (cr7, 0, 38, 128, 12);
            cairo_fill(cr7);
            cairo_stroke(cr7);
            cairo_set_operator(cr4, CAIRO_OPERATOR_CLEAR);
            break;
        case SELECT_THIRD:
            image1 = cairo_image_surface_create_from_png (get_work_mode_image_small_url(WORKMODE_LIVE, SELECT_NO));
            image2 = cairo_image_surface_create_from_png (get_angle_url(SELECT_NO));
            cairo_set_source_surface (cr1, image1, 47, 6);
            cairo_set_source_surface (cr2, image2, 73, 7);
            cairo_rectangle (cr7, 0, 50, 128, 12);
            cairo_fill(cr7);
            cairo_stroke(cr7);
            cairo_set_operator(cr5, CAIRO_OPERATOR_CLEAR);
            break;

        default:
            break;
        }
    }
    else
    {
        item1 = (gchar *)g_list_nth_data(list, 3);
        item2 = (gchar *)g_list_nth_data(list, 4);
        switch (select)
        {
        case SELECT_FOURTH:
            image1 = cairo_image_surface_create_from_png (get_work_mode_image_small_url(WORKMODE_LIVE, SELECT_NO));
            image2 = cairo_image_surface_create_from_png (get_angle_url(SELECT_NO));
            cairo_set_source_surface (cr1, image1, 47, 6);
            cairo_set_source_surface (cr2, image2, 73, 7);
            cairo_rectangle (cr7, 0, 26, 128, 12);
            cairo_fill(cr7);
            cairo_stroke(cr7);
            cairo_set_operator(cr3, CAIRO_OPERATOR_CLEAR);
            break;
        case SELECT_FIFTH:
            image1 = cairo_image_surface_create_from_png (get_work_mode_image_small_url(WORKMODE_LIVE, SELECT_NO));
            image2 = cairo_image_surface_create_from_png (get_angle_url(SELECT_NO));
            cairo_set_source_surface (cr1, image1, 47, 6);
            cairo_set_source_surface (cr2, image2, 73, 7);
            cairo_rectangle (cr7, 0, 38, 128, 12);
            cairo_fill(cr7);
            cairo_stroke(cr7);
            cairo_set_operator(cr4, CAIRO_OPERATOR_CLEAR);
            break;

        default:
            break;
        }
    }

    cairo_paint(cr1);
    cairo_paint(cr2);
    cairo_set_operator(cr6, CAIRO_OPERATOR_CLEAR);    //删除顶部4像素
    cairo_rectangle (cr6, 0, 0, 128, 4);
    cairo_fill(cr6);
    cairo_stroke(cr6);
    if (-1 == select)
    {
        cairo_rectangle (cr6, 0, 24, 128, 4);
        cairo_fill(cr6);
        cairo_stroke(cr6);
    }
    cairo_show_text(cr3, item1);
    cairo_show_text(cr4, item2);
    cairo_show_text(cr5, item3);

    cairo_destroy(cr1);
	cairo_destroy(cr2);
	cairo_destroy(cr3);
	cairo_destroy(cr4);
    cairo_destroy(cr5);
    cairo_destroy(cr6);
    cairo_destroy(cr7);
    cairo_destroy(cr8);
    cairo_surface_destroy(image1);
    cairo_surface_destroy(image2);

    return TRUE;
}

gint show_sys_setting(gpointer data)
{
    gint select;
    GList *list = NULL;
    gchar *item1 = NULL;
    gchar *item2 = NULL;
    gchar *item3 = NULL;
    g_object_get(G_OBJECT (data), "SystemList", &list, "InterfaceSelect", &select, NULL);
    cairo_t *cr1 = cairo_create(surface);     //工作模式，图片
    cairo_t *cr2 = cairo_create(surface);     //三角箭头，图片
    cairo_t *cr3 = cairo_create(surface);     //第一行:拍照模式，字符串
    cairo_t *cr4 = cairo_create(surface);     //第二行:连拍间隔，字符串
    cairo_t *cr5 = cairo_create(surface);     //第三行:done，字符串
    cairo_t *cr6 = cairo_create(surface);     //占位，矩形
    cairo_t *cr7 = cairo_create(surface);     //占位，矩形
    cairo_t *cr8 = cairo_create(surface);
    cairo_set_operator(cr8, CAIRO_OPERATOR_CLEAR);
    cairo_rectangle (cr8, 0, 0, 128, 128);
    cairo_fill(cr8);
    cairo_stroke(cr8);
    cairo_surface_t *image1;
	cairo_surface_t *image2;


    cairo_select_font_face(cr3, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr3, 12.0);
    cairo_set_source_rgb(cr3, 1.0, 1.0, 1.0); 
    cairo_move_to(cr3, 8.0, 36.0); 

    cairo_select_font_face(cr4, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr4, 12.0);
    cairo_set_source_rgb(cr4, 1.0, 1.0, 1.0); 
    cairo_move_to(cr4, 8.0, 48.0);

    cairo_select_font_face(cr5, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr5, 12.0);
    cairo_set_source_rgb(cr5, 1.0, 1.0, 1.0); 
    cairo_move_to(cr5, 8.0, 60.0);
    if (select <= 2)
    {
        item1 = (gchar *)g_list_nth_data(list, 0);
        item2 = (gchar *)g_list_nth_data(list, 1);
        item3 = (gchar *)g_list_nth_data(list, 2);
        switch (select)
        {
        case -1:
            image1 = cairo_image_surface_create_from_png (get_work_mode_image_small_url(WORKMODE_SETTING, SELECT_YES));
            image2 = cairo_image_surface_create_from_png (get_angle_url(SELECT_YES));
            cairo_set_source_surface (cr1, image1, 47, 0);
            cairo_set_source_surface (cr2, image2, 73, 0);
            cairo_rectangle (cr6, 0, 0, 47, 28);
            cairo_fill(cr6);
            cairo_stroke(cr6);
            cairo_rectangle (cr6, 67, 0, 6, 28);
            cairo_fill(cr6);
            cairo_stroke(cr6);
            cairo_rectangle (cr6, 81, 0, 55, 28);
            cairo_fill(cr6);
            cairo_stroke(cr6);
            break;
        case SELECT_FIRST:
            image1 = cairo_image_surface_create_from_png (get_work_mode_image_small_url(WORKMODE_SETTING, SELECT_NO));
            image2 = cairo_image_surface_create_from_png (get_angle_url(SELECT_NO));
            cairo_set_source_surface (cr1, image1, 47, 6);
            cairo_set_source_surface (cr2, image2, 73, 7);
            cairo_rectangle (cr7, 0, 26, 128, 12);
            cairo_fill(cr7);
            cairo_stroke(cr7);
            cairo_set_operator(cr3, CAIRO_OPERATOR_CLEAR);
            break;
        case SELECT_SECOND:
            image1 = cairo_image_surface_create_from_png (get_work_mode_image_small_url(WORKMODE_SETTING, SELECT_NO));
            image2 = cairo_image_surface_create_from_png (get_angle_url(SELECT_NO));
            cairo_set_source_surface (cr1, image1, 47, 6);
            cairo_set_source_surface (cr2, image2, 73, 7);
            cairo_rectangle (cr7, 0, 38, 128, 12);
            cairo_fill(cr7);
            cairo_stroke(cr7);
            cairo_set_operator(cr4, CAIRO_OPERATOR_CLEAR);
            break;
        case SELECT_THIRD:
            image1 = cairo_image_surface_create_from_png (get_work_mode_image_small_url(WORKMODE_SETTING, SELECT_NO));
            image2 = cairo_image_surface_create_from_png (get_angle_url(SELECT_NO));
            cairo_set_source_surface (cr1, image1, 47, 6);
            cairo_set_source_surface (cr2, image2, 73, 7);
            cairo_rectangle (cr7, 0, 50, 128, 12);
            cairo_fill(cr7);
            cairo_stroke(cr7);
            cairo_set_operator(cr5, CAIRO_OPERATOR_CLEAR);
            break;

        default:
            break;
        }
    }
    else
    {
        item1 = (gchar *)g_list_nth_data(list, 3);
        item2 = (gchar *)g_list_nth_data(list, 4);
        switch (select)
        {
        case SELECT_FOURTH:
            image1 = cairo_image_surface_create_from_png (get_work_mode_image_small_url(WORKMODE_SETTING, SELECT_NO));
            image2 = cairo_image_surface_create_from_png (get_angle_url(SELECT_NO));
            cairo_set_source_surface (cr1, image1, 47, 6);
            cairo_set_source_surface (cr2, image2, 73, 7);
            cairo_rectangle (cr7, 0, 26, 128, 12);
            cairo_fill(cr7);
            cairo_stroke(cr7);
            cairo_set_operator(cr3, CAIRO_OPERATOR_CLEAR);
            break;
        case SELECT_FIFTH:
            image1 = cairo_image_surface_create_from_png (get_work_mode_image_small_url(WORKMODE_SETTING, SELECT_NO));
            image2 = cairo_image_surface_create_from_png (get_angle_url(SELECT_NO));
            cairo_set_source_surface (cr1, image1, 47, 6);
            cairo_set_source_surface (cr2, image2, 73, 7);
            cairo_rectangle (cr7, 0, 38, 128, 12);
            cairo_fill(cr7);
            cairo_stroke(cr7);
            cairo_set_operator(cr4, CAIRO_OPERATOR_CLEAR);
            break;

        default:
            break;
        }
    }

    cairo_paint(cr1);
    cairo_paint(cr2);
    cairo_set_operator(cr6, CAIRO_OPERATOR_CLEAR);
    cairo_rectangle (cr6, 0, 0, 128, 4);
    cairo_fill(cr6);
    cairo_stroke(cr6);
    if (-1 == select)
    {
        cairo_rectangle (cr6, 0, 24, 128, 4);
        cairo_fill(cr6);
        cairo_stroke(cr6);
    }
    cairo_show_text(cr3, item1);
    cairo_show_text(cr4, item2);
    cairo_show_text(cr5, item3);

    cairo_destroy(cr1);
	cairo_destroy(cr2);
	cairo_destroy(cr3);
	cairo_destroy(cr4);
    cairo_destroy(cr5);
    cairo_destroy(cr6);
    cairo_destroy(cr7);
    cairo_destroy(cr8);
    cairo_surface_destroy(image1);
    cairo_surface_destroy(image2);

    return TRUE;
}

gchar *get_network_image_url(gpointer data)
{
    guint net_value;
    static gchar net_buf[100] = {0};
    g_object_get(G_OBJECT (data), "NetworkMode", &net_value, NULL);
    if (NETWORK_WIRE == net_value)
    {
        sprintf(net_buf, "%s/anywhere_picture/fittings/net_wire.png", image_route);
    }
    else if(NETWORK_WIFI == net_value)
    {
        sprintf(net_buf, "%s/anywhere_picture/fittings/net_wifi.png", image_route);
    }
    return net_buf;
}

gchar *get_battery_image_url(gpointer data)
{
    guint battery_value;
    static gchar battery_buf[100] = {0};
    g_object_get(G_OBJECT (data), "battery", &battery_value, NULL);
    switch (battery_value)
    {
    case BATTERY_LESS_THAN_5:
        sprintf(battery_buf, "%s/anywhere_picture/battery/less5.png", image_route);
        break;
    case BATTERY_10:
        sprintf(battery_buf, "%s/anywhere_picture/battery/10.png", image_route);
        break;
    case BATTERY_20:
        sprintf(battery_buf, "%s/anywhere_picture/battery/20.png", image_route);
        break;
    case BATTERY_40:
        sprintf(battery_buf, "%s/anywhere_picture/battery/40.png", image_route);
        break;
    case BATTERY_60:
        sprintf(battery_buf, "%s/anywhere_picture/battery/60.png", image_route);
        break;
    case BATTERY_80:
        sprintf(battery_buf, "%s/anywhere_picture/battery/80.png", image_route);
        break;
    case BATTERY_100:
        sprintf(battery_buf, "%s/anywhere_picture/battery/100.png", image_route);
        break;
    case BATTERY_P_0:
        sprintf(battery_buf, "%s/anywhere_picture/battery/P_0.png", image_route);
        break;
    case BATTERY_P_10:
        sprintf(battery_buf, "%s/anywhere_picture/battery/P_10.png", image_route);
        break;
    case BATTERY_P_20:
        sprintf(battery_buf, "%s/anywhere_picture/battery/P_20.png", image_route);
        break;
    case BATTERY_P_40:
        sprintf(battery_buf, "%s/anywhere_picture/battery/P_40.png", image_route);
        break;
    case BATTERY_P_60:
        sprintf(battery_buf, "%s/anywhere_picture/battery/P_60.png", image_route);
        break;
    case BATTERY_P_80:
        sprintf(battery_buf, "%s/anywhere_picture/battery/P_80.png", image_route);
        break;
    case BATTERY_P_100:
        sprintf(battery_buf, "%s/anywhere_picture/battery/P_100.png", image_route);
        break;

    default:
        break;
    }
    return battery_buf;
}

gchar *get_work_mode_image_big_url(gint work_mode)
{
    static gchar mode_buf[100] = {0};
    switch (work_mode)
    {
    case WORKMODE_JPEG:
        sprintf(mode_buf, "%s/anywhere_picture/fittings/jpeg_l.png", image_route);
        break;
    case WORKMODE_MP4:
        sprintf(mode_buf, "%s/anywhere_picture/fittings/mp4_l.png", image_route);
        break;
    case WORKMODE_LIVE:
        sprintf(mode_buf, "%s/anywhere_picture/fittings/live_l.png", image_route);
        break;
    case WORKMODE_SETTING:
        sprintf(mode_buf, "%s/anywhere_picture/fittings/set_l.png", image_route);
        break;

    default:
        break;
    }
    return mode_buf;
}

gchar *get_work_mode_image_small_url(gint work_mode, gint select_mode)
{
    static gchar mode_buf[100] = {0};
    switch (work_mode)
    {
    case WORKMODE_JPEG:
        if (SELECT_NO == select_mode)
        {
            sprintf(mode_buf, "%s/anywhere_picture/fittings/jpeg_s.png", image_route);
        }
        else if (SELECT_YES == select_mode)
        {
            sprintf(mode_buf, "%s/anywhere_picture/fittings/jpeg_s_r.png", image_route);
        }
        break;
    case WORKMODE_MP4:
        if (SELECT_NO == select_mode)
        {
            sprintf(mode_buf, "%s/anywhere_picture/fittings/mp4_s.png", image_route);
        }
        else if (SELECT_YES == select_mode)
        {
            sprintf(mode_buf, "%s/anywhere_picture/fittings/mp4_s_r.png", image_route);
        }
        break;
    case WORKMODE_LIVE:
        if (SELECT_NO == select_mode)
        {
            sprintf(mode_buf, "%s/anywhere_picture/fittings/live_s.png", image_route);
        }
        else if (SELECT_YES == select_mode)
        {
            sprintf(mode_buf, "%s/anywhere_picture/fittings/live_s_r.png", image_route);
        }
        break;
    case WORKMODE_SETTING:
        if (SELECT_NO == select_mode)
        {
            sprintf(mode_buf, "%s/anywhere_picture/fittings/set_s.png", image_route);
        }
        else if (SELECT_YES == select_mode)
        {
            sprintf(mode_buf, "%s/anywhere_picture/fittings/set_s_r.png", image_route);
        }
        break;

    default:
        break;
    }
    return mode_buf;
}

gchar *get_angle_url(gint select_mode)
{
    static gchar angle_buf[100] = {0};
    if (SELECT_NO == select_mode)
    {
        sprintf(angle_buf, "%s/anywhere_picture/fittings/angle.png", image_route);
    }
    else if (SELECT_YES == select_mode)
    {
        sprintf(angle_buf, "%s/anywhere_picture/fittings/angle_r.png", image_route);
    }
    return angle_buf;
}
