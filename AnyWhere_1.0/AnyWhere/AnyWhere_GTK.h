#ifndef _ANYWHERE_GTK_H_
#define _ANYWHERE_GTK_H_

#include "AnyWhere_mode.h"
#include <glib-object.h>
#include <cairo/cairo.h>
#include <fcntl.h>
#include <linux/types.h>
#include <linux/ioctl.h>
#include <linux/fb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

#define AW_TYPE_GTK (aw_gtk_get_type())
//强制转换，GObject基类转换为本子类
#define AW_GTK(object) G_TYPE_CHECK_INSTANCE_CAST((object), AW_TYPE_GTK, AwGtk)

#define WIDTH (128)
#define HIGH (64)

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

/* Destroy a cairo surface */
void cairo_linuxfb_surface_destroy(void *device)
{
	cairo_linuxfb_device_t *dev = (cairo_linuxfb_device_t *)device;

	if (dev == NULL)
		return;

	munmap(dev->fb_data, dev->fb_screensize);
	close(dev->fb_fd);
	free(dev);
}

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

gint OLED_SHUTDOWN();

cairo_surface_t *surface;
AwGtk *entry;
gchar *image_route;

#endif