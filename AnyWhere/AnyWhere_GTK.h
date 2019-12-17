#ifndef _ANYWHERE_GTK_H_
#define _ANYWHERE_GTK_H_

#include "AnyWhere_mode.h"
#include <gtk/gtk.h>
#include <glib-gobject.h>
#include <gdk/gdkkeysyms.h>

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

enum INTERFACE
{
    CAMERA_START = 0,
    CAMERA_SHUTDOWN,
    JPEG_ENTRY,
    JPEG_STATE,
    JPEG_SETTING_1,
    JPEG_SETTING_2,
    JPEG_SETTING_SHOOT,
    JPEG_SETTING_RAW,
    JPEG_SETTING_INTERVAL_1,
    JPEG_SETTING_INTERVAL_2,
    MP4_ENTRY,
    MP4_STATE,
    MP4_SETTING_1,
    MP4_SETTING_2,
    MP4_SETTING_RESOLUTION,
    MP4_SETTING_BITRATE,
    MP4_SETTING_FRAMERATE,
    MP4_SETTING_SEPARATE,
    LIVE_ENTRY,
    LIVE_STATE,
    LIVE_SETTING_1,
    LIVE_SETTING_2,
    LIVE_SETTING_RESOLUTION,
    LIVE_SETTING_BITRATE,
    LIVE_SETTING_FRAMERATE,
    LIVE_SETTING_PROTOCOL,
    EXPO_SETTING_1,
    EXPO_SETTING_2,
    EXPO_SETTING_AUTO,
    EXPO_SETTING_MANUAL,
    EXPO_SETTING_SHUTTER,
    EXPO_SETTING_ISO,
    EXPO_ISO_SETTING_1,
    EXPO_ISO_SETTING_2,
    EXPO_SHUTTER_SETTING_1,
    EXPO_SHUTTER_SETTING_2,
    EXPO_SHUTTER_SETTING_3,
    SYS_SETTING,
    SYS_SETTING_WIFI,
    SYS_SETTING_LED,
};

enum INTERFACE_SELECT
{
    SELECT_NONE = 0,
    SELECT_HEAD,
    SELECT_FIRST,
    SELECT_SECOND,
    SELECT_THIRD
};

typedef struct _WorkTime
{
    guint work_time_hour;
    guint work_time_minu;
    guint work_time_sec;
}WorkTime;

typedef struct _JPEGData
{
    guint jpeg_disk_space;
    guint jpeg_shoot_mode;
    guint jpeg_raw_mode;
    guint jpeg_continue_interval;
}JPEGData;

typedef struct _MP4Data
{
    WorkTime mp4_worktime;
    guint mp4_resolution;
    guint mp4_bitrate;
    guint mp4_framerate;
    guint mp4_separate;
}MP4Data;

typedef struct _LiveData
{
    WorkTime live_worktime;
    guint live_resolution;
    guint live_bitrate;
    guint live_framerate;
    guint live_protocol;
}LiveData;

typedef struct _SysSetting
{
    guint Wifi;
    guint LED;
    guint LAN;
}SysSetting;

typedef struct _StateData
{
    guint workmode;
    guint network;
    gchar IPaddr[16];
    guint battery;
    JPEGData jpeg_data;
    MP4Data mp4_data;
    LiveData live_data;
    guint expo_mode;
    guint expo_iso_mode;
    guint expo_shutter_mode;
    SysSetting sys_setting;
}StateData;
StateData state_data;

gint show_jpeg(GtkWidget *widget, gpointer data);
gchar *get_network_image_url();
gchar *get_battery_image_url();
gchar *get_work_mode_image_url();
gchar *get_expo_iso_string(gint expo_mode);
gchar *get_expo_shutter_string(gint expo_mode);
gchar *get_wifi_mode_string();
gcahr *get_jpeg_continue_interval_string();
gchar *get_mp4_separate_string();
gint get_live_resolution_val();
gint get_live_bitrate_val();

#endif