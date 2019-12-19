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
    JPEG_ENTRY,     //进入拍照模式主界面
    JPEG_STATE,     //进入状态显示界面
    JPEG_SETTING_1,    //拍照设置页面1
//    JPEG_SETTING_2,    //拍照设置页面2
    JPEG_SETTING_SHOOT,
//    JPEG_SETTING_RAW,    //一期不做
    MP4_ENTRY,
    MP4_STATE,
    MP4_SETTING_1,
    MP4_SETTING_2,
//    MP4_SETTING_RESOLUTION,  //一期只有8K
//    MP4_SETTING_BITRATE,     //只有100Mb
//    MP4_SETTING_FRAMERATE,   //只有30FP
    MP4_SETTING_SEPARATE_1,
    MP4_SETTING_SEPARATE_2,
    LIVE_ENTRY,
    LIVE_STATE,
    LIVE_SETTING_1,
    LIVE_SETTING_2,
    LIVE_SETTING_RESOLUTION,
    LIVE_SETTING_BITRATE,
    LIVE_SETTING_FRAMERATE,
    LIVE_SETTING_PROTOCOL,
#if 0
    EXPO_SETTING_1,       //曝光设置页面1
    EXPO_SETTING_2,
    EXPO_SETTING_AUTO,    //曝光-自动模式
    EXPO_SETTING_MANUAL,
    EXPO_SETTING_SHUTTER,
    EXPO_SETTING_ISO,
    EXPO_ISO_SETTING_1,    //iso设置页面1
    EXPO_ISO_SETTING_2,
    EXPO_SHUTTER_SETTING_1,  //快门设置页面1
    EXPO_SHUTTER_SETTING_2,
    EXPO_SHUTTER_SETTING_3,
#endif     //关于曝光调节，一期不做
    SYS_ENTRY,
    SYS_SETTING_1,        //系统设置主页1
    SYS_SETTING_WIFI,
    SYS_SETTING_LED,
};

enum INTERFACE_SELECT
{
    SELECT_NONE = 0,         // 无选项页面
    SELECT_HEAD,             // 选择导航栏
    SELECT_FIRST,            // 选择第一行
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
    guint jpeg_continue_interval[8];   //前端输入连拍间隔
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
StateData state_data = {0};

/* 获取显示信息函数 */
gchar *get_network_image_url();
gchar *get_battery_image_url();
gchar *get_work_mode_image_big_url(gint work_mode);
gchar *get_work_mode_image_sel_url(gint work_mode, gint select_mode);     //获取设置模式时导航栏的图片路径
gchar *get_state_mode_image_url(gint work_mode);
//gchar *get_expo_iso_string(gint expo_mode);
//gchar *get_expo_shutter_string(gint expo_mode);
gchar *get_wifi_mode_string();
gcahr *get_jpeg_continue_interval_string();   //连拍模式的拍照时间间隔
gchar *get_mp4_separate_string();
gint get_live_resolution_val();
gint get_live_bitrate_val();
gchar *get_jpeg_space_string();
gchar *get_work_time_string(gint work_mode);
gchar *get_jpeg_shoot_string();    //获取拍照模式
gchar *get_setting_wifi_mode();
gchar *get_setting_led_mode();
gchar *get_setting_lan_mode();

/* 显示页面函数 */
gint show_jpeg();
gint show_jpeg_state();
gint show_jpeg_setting1();
//gint show_jpeg_setting2();
//gint show_jpeg_raw_setting();
gint show_jpeg_shoot_setting();
//gint show_expo_setting1();

gint show_mp4();
gint show_mp4_state();
gint show_mp4_setting1();
gint show_mp4_setting2();
gint show_mp4_setting_separate1();
gint show_mp4_setting_separate2();

gint show_live();
gint show_live_state();
gint show_live_setting1();
gint show_live_setting2();
gint show_live_setting_resolution();
gint show_live_setting_bitrate();
gint show_live_setting_framerate();
gint show_live_setting_protocol();

gint show_system();
gint show_sys_setting1();
gint show_sys_setting_wifi();
gint show_sys_setting_led();

gint OLED_SHUTDOWN();
#endif