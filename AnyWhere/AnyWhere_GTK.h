#ifndef _ANYWHERE_GTK_H_
#define _ANYWHERE_GTK_H_

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <stdlib.h>
#include <string.h>
#include <gdk/gdkkeysyms.h>
#include <glib-gobject.h>

#define AW_TYPE_GTK (aw_gtk_get_type())
//强制转换，GObject基类转换为本子类
#define AW_GTK(object) G_TYPE_CHECK_INSTANCE_CAST((object), AW_TYPE_GTK, AwGtk)

typedef struct _AwGtk
{
    GObject parent;
}AwGtk;

typedef struct _AwGtkClass
{
    GObjectClass parent_class;
}AwGtkClass;

GType aw_gtk_get_type(void);
AwGtk *entry;

enum CAMERA_MODE
{
    CAMERA_START = 0,
    CAMERA_WORK,
    CAMERA_SHUTDOWN
};

enum WORK_MODE
{
    WORK_JPEG = 0,
    WORK_MP4,
    WORK_LIVE,
    WORK_SETTINGS
};

enum JPEG_MODE
{
    JPEG_STATUS = 0,
    JPEG_SETTINGS
};

enum JPEG_SHOOT_MODE
{
    JPEG_SHOOT_STANDARD = 0,
    JPEG_SHOOT_CONTINUOUS
};

enum JPEG_RAW_MODE
{
    JPEG_RAW_ON = 0,
    JPEG_RAW_OFF
};

enum EXPO_MODE
{
    EXPO_AUTO = 0,
    EXPO_MANUAL,
    EXPO_SHUTTER,
    EXPO_ISO
};

int show_jpeg(GtkWidget *widget, gpointer data);
int show_mp4(GtkWidget *widget, gpointer data);
int show_live(GtkWidget *widget, gpointer data);
int show_setting(GtkWidget *widget, gpointer data);

gchar expo_iso_fix[6][6] = {"Auto", "100", "200", "400", "800", "1600"};
gchar expo_iso_range[6][6] = {"Auto", "<100", "<200", "<400", "<800", "<1600"};
gchar expo_shutter_fix[8][8] = {"Auto", "1/3200", "1/1000", "1/500", "1/100", "1/50", "1/30", "1/15"};
gchar expo_shutter_range[8][8] = {"Auto", ">1/3200", ">1/1000", ">1/500", ">1/100", ">1/50", ">1/30", ">1/15"};

#endif