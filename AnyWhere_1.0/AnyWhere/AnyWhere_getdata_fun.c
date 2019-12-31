#include "AnyWhere_GTK.h"

gchar expo_iso_fix[6][6] = {"Auto", "100", "200", "400", "800", "1600"};
gchar expo_iso_range[6][6] = {"Auto", "<100", "<200", "<400", "<800", "<1600"};
gchar expo_shutter_fix[8][8] = {"Auto", "1/3200", "1/1000", "1/500", "1/100", "1/50", "1/30", "1/15"};
gchar expo_shutter_range[8][8] = {"Auto", ">1/3200", ">1/1000", ">1/500", ">1/100", ">1/50", ">1/30", ">1/15"};

gchar *get_network_image_url()
{
    switch (state_data.network)
    {
    case NETWORK_WIRE:
        return "网线模式图片路径";
        break;
    case NETWORK_WIRE:
        return "网线模式图片路径";
        break;
    default:
        return NULL;
        break;
    }
}

gchar *get_battery_image_url()
{
    switch (state_data.battery)
    {
    case BATTERY_LESS_THAN_5:
        return "电量少于5%图片";
        break;
    case BATTERY_10:
        return "电量10%图片";
        break;
    case BATTERY_20:
        return "电量20%图片";
        break;
    case BATTERY_40:
        return "电量40%图片";
        break;
    case BATTERY_60:
        return "电量60%图片";
        break;
    case BATTERY_80:
        return "电量80%图片";
        break;
    case BATTERY_100:
        return "电量100%图片";
        break;
    case BATTERY_POWER:
        return "正在充电图片";
        break;
    default:
        return NULL;
        break;
    }
}

gchar *get_work_mode_image_big_url(gint work_mode)
{

}

gchar *get_work_mode_image_sel_url(gint work_mode, gint select_mode)
{

}

gchar *get_state_mode_image_url(gint work_mode)
{

}

gchar *get_expo_iso_string(gint expo_mode)
{

}

gchar *get_expo_shutter_string(gint expo_mode)
{

}

gchar *get_wifi_mode_string()
{

}

gcahr *get_jpeg_continue_interval_string()
{

}

gchar *get_mp4_separate_string()
{

}

gint get_live_resolution_val()
{

}

gint get_live_bitrate_val()
{
    
}

gchar *get_jpeg_space_string()
{

}

gchar *get_work_time_string(gint work_mode)
{
    if(WORKMODE_MP4 == work_mode)
    {

    }
    else if(WORKMODE_LIVE == work_mode)
    {

    }
}

gchar *get_jpeg_shoot_string()
{
    
}

gchar *get_setting_wifi_mode()
{

}

gchar *get_setting_led_mode()
{

}

gchar *get_setting_lan_mode()
{
    
}