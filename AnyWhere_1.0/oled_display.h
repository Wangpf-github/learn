#ifndef _ANYWHERE_OLED_H_
#define _ANYWHERE_OLED_H_

#include <glib-object.h>
#include <glib.h>

#define TYPE_OLED_DISPLAY (oled_display_get_type())
#define OLED_DISPLAY(object) G_TYPE_CHECK_INSTANCE_CAST((object), TYPE_OLED_DISPLAY, OledDisplay)

/* 结构体 */
typedef struct _OledDisplay
{
    GObject parent;
}OledDisplay;

typedef struct _OledDisplayClass
{
    GObjectClass parent_class;
}OledDisplayClass;

GType oled_display_get_type(void);

/* 枚举类型 */
enum INTERFACE
{
    OLED_CAMERA_START,
    OLED_CAMERA_SHUTDOWN,
    OLED_JPEG_MAIN,                 //进入拍照模式主界面
    OLED_JPEG_STATE,                //进入状态显示界面
    OLED_JPEG_SETTING,              //拍照设置页面1
    OLED_MP4_MAIN,
    OLED_MP4_STATE,
    OLED_MP4_SETTING,
    OLED_LIVE_MAIN,
    OLED_LIVE_STATE,
    OLED_LIVE_SETTING,
    OLED_SYS_MAIN,
    OLED_SYS_SETTING            //系统设置主页1
};

enum INTERFACE_SELECT
{
    OLED_SELECT_FIRST,
    OLED_SELECT_SECOND,
    OLED_SELECT_THIRD,
    OLED_SELECT_FOURTH,
    OLED_SELECT_FIFTH,
    OLED_SELECT_SIXTH,
    OLED_SELECT_SEVENTH,
    OLED_SELECT_EIGHTH,
    OLED_SELECT_NINETH,
    OLED_SELECT_TENTH
};

enum WORKMODE
{
    OLED_WORKMODE_JPEG,
    OLED_WORKMODE_MP4,
    OLED_WORKMODE_LIVE,
    OLED_WORKMODE_SETTING
};

enum NETWORK_STATE
{
    OLED_NETWORK_WIRE,
    OLED_NETWORK_WIFI
};

enum BATTERY_STATE
{
    OLED_BATTERY_LESS_THAN_5,
    OLED_BATTERY_10,
    OLED_BATTERY_20,
    OLED_BATTERY_40,
    OLED_BATTERY_60,
    OLED_BATTERY_80,
    OLED_BATTERY_100,
    OLED_BATTERY_P_0,
    OLED_BATTERY_P_10,
    OLED_BATTERY_P_20,
    OLED_BATTERY_P_40,
    OLED_BATTERY_P_60,
    OLED_BATTERY_P_80,
    OLED_BATTERY_P_100
};

#endif