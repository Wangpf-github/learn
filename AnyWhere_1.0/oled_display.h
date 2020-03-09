#ifndef _ANYWHERE_OLED_H_
#define _ANYWHERE_OLED_H_

#include <glib-object.h>
#include <glib.h>

#define TYPE_OLED_SCREEN (oled_screen_get_type())
#define OLED_SCREEN(object) G_TYPE_CHECK_INSTANCE_CAST((object), TYPE_OLED_SCREEN, OledScreen)

/* 结构体 */
typedef struct _OledScreen
{
    GObject parent;
}OledScreen;

typedef struct _OledScreenClass
{
    GObjectClass parent_class;
}OledScreenClass;

GType oled_screen_get_type(void);

/* 枚举类型 */
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
    SELECT_EIGHTH,
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

#endif