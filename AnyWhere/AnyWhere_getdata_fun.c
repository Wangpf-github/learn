#include "AnyWhere_GTK.h"

gchar *get_network_image_url()
{
    guint net_value;
    static gchar net_buf[100] = {0};
    g_object_get(G_OBJECT (entry), "network_mode", &net_value, NULL);
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

gchar *get_battery_image_url()
{
    guint battery_value;
    static gchar battery_buf[100] = {0};
    g_object_get(G_OBJECT (entry), "battery", &battery_value, NULL);
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