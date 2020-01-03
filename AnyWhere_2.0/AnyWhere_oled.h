#ifndef _ANYWHERE_OLED_H_
#define _ANYWHERE_OLED_H_

#include <glib-object.h>
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
#include <glib.h>

#define AW_TYPE_GTK (aw_gtk_get_type())
//强制转换，GObject基类转换为本子类
#define AW_GTK(object) G_TYPE_CHECK_INSTANCE_CAST((object), AW_TYPE_GTK, AwGtk)
#define WIDTH  (128)
#define HIGH   (60)

typedef struct _AwGtk
{
    GObject parent;
}AwGtk;

typedef struct _AwGtkClass
{
    GObjectClass parent_class;
}AwGtkClass;

GType aw_gtk_get_type(void);

typedef struct _cairo_linuxfb_device {
	int fb_fd;
	unsigned char *fb_data;
	long fb_screensize;
	struct fb_var_screeninfo fb_vinfo;
	struct fb_fix_screeninfo fb_finfo;
} cairo_linuxfb_device_t;

/* 函数声明 */
/* 获取显示信息函数 */
gchar *get_network_image_url();                                            //网络模式路径
gchar *get_battery_image_url();                                            //电池图片路径
gchar *get_work_mode_image_big_url(gint work_mode);                        //状态显示页面图片路径
gchar *get_work_mode_image_small_url(gint work_mode, gint select_mode);    //获取设置模式时导航栏的图片路径
gchar *get_angle_url(gint select_mode);
/* 显示页面函数 */
gint show_main(guint mode);
gint show_jpeg_state();
gint show_jpeg_setting();
gint show_mp4_state();
gint show_mp4_setting();
gint show_live_state();
gint show_live_setting();
gint show_sys_setting();
gint oled_start();
gint oled_shutdown();
void cairo_linuxfb_surface_destroy(void *device);
gpointer thread_func(gpointer data);

cairo_surface_t *surface;
AwGtk *entry;
gchar *image_route;
GMainLoop *loop;

enum INTERFACE
{
    CAMERA_START = 0,
    CAMERA_SHUTDOWN,
    JPEG_MAIN,                 //进入拍照模式主界面
    JPEG_STATE,                //进入状态显示界面
    JPEG_SETTING,              //拍照设置页面1
    MP4_MAIN,
    MP4_STATE,
    MP4_SETTING,
    LIVE_MAIN,
    LIVE_STATE,
    LIVE_SETTING,
    SYS_MAIN,
    SYS_SETTING            //系统设置主页1
};

enum INTERFACE_SELECT
{
    SELECT_FIRST = 0,            
    SELECT_SECOND,
    SELECT_THIRD,
    SELECT_FOURTH,
    SELECT_FIFTH,
    SELECT_SIXTH,
    SELECT_SEVENTH,
    SELECT_NINETH,
    SELECT_TENTH
};

enum WORKMODE
{
    WORKMODE_JPEG,
    WORKMODE_MP4,
    WORKMODE_LIVE,
    WORKMODE_SETTING
};

enum NETWORK_STATE
{
    NETWORK_WIRE,
    NETWORK_WIFI
};

enum BATTERY_STATE
{
    BATTERY_LESS_THAN_5 = 0,
    BATTERY_10,
    BATTERY_20,
    BATTERY_40,
    BATTERY_60,
    BATTERY_80,
    BATTERY_100,
    BATTERY_P_0,
    BATTERY_P_10,
    BATTERY_P_20,
    BATTERY_P_40,
    BATTERY_P_60,
    BATTERY_P_80,
    BATTERY_P_100
};

enum IS_SELECTED
{
    SELECT_NO,
    SELECT_YES
};

#endif