#ifndef _ANYWHERE_MODE_H_
#define _ANYWHERE_MODE_H_

#include <stdlib.h>
#include <string.h>
#include <glib.h>


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
//    MP4_SETTING_RESOLUTION,  //一期只有8K
//    MP4_SETTING_BITRATE,     //只有100Mb
//    MP4_SETTING_FRAMERATE,   //只有30FP
    LIVE_MAIN,
    LIVE_STATE,
    LIVE_SETTING,
#if 0
    EXPO_SETTING_1,            //曝光设置页面1
    EXPO_SETTING_2,
    EXPO_SETTING_AUTO,         //曝光-自动模式
    EXPO_SETTING_MANUAL,
    EXPO_SETTING_SHUTTER,
    EXPO_SETTING_ISO,
    EXPO_ISO_SETTING_1,        //iso设置页面1
    EXPO_ISO_SETTING_2,
    EXPO_SHUTTER_SETTING_1,    //快门设置页面1
    EXPO_SHUTTER_SETTING_2,
    EXPO_SHUTTER_SETTING_3,
#endif     //关于曝光调节，一期不做
    SYS_MAIN,
    SYS_SETTING,            //系统设置主页1
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
}

#endif