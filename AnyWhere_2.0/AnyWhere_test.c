#include "AnyWhere_oled.h"


/* 测试程序 */
gpointer thread_func(gpointer data)
{
    gint num;
    GList *list1 = NULL;
    GList *list2 = NULL;
    GList *list3 = NULL;
    GList *list4 = NULL;
    list1 = g_list_prepend(list1, "Mode:Standard");
    list1 = g_list_insert(list1, "Interval:1/3s", 1);
    list1 = g_list_insert(list1, "Done", 2);

    list2 = g_list_prepend(list2, "Resolution:8K");
    list2 = g_list_insert(list2, "BitRate:10Mbps", 1);
    list2 = g_list_insert(list2, "FrameRate:20FPS", 2);
    list2 = g_list_insert(list2, "Separate:30min", 3);
    list2 = g_list_insert(list2, "Done", 4);

    list3 = g_list_prepend(list3, "Resolution:8K");
    list3 = g_list_insert(list3, "BitRate:60Mbps", 1);
    list3 = g_list_insert(list3, "FrameRate:30FPS", 2);
    list3 = g_list_insert(list3, "Protocol:RTSP", 3);
    list3 = g_list_insert(list3, "Done", 4);

    list4 = g_list_prepend(list4, "WIFI:AP");
    list4 = g_list_insert(list4, "LED:ON", 1);
    list4 = g_list_insert(list4, "LAN:DHCP", 2);
    list4 = g_list_insert(list4, "Done", 3);
    
    g_object_set(G_OBJECT(entry), "interface",       0, 
                                  "jpeglist",        list1,
                                  "videolist",         list2,
                                  "livelist",        list3,
                                  "systemlist",      list4,
                                  "select",          -1,
                                  "network",     NETWORK_WIRE,
                                  "IPaddr",           "192.168.10.100",
                                  "battery",          BATTERY_P_100,
                                  "JpegDisk", "12300",
                                  "JpegResolution", "8K",
                                  "JpegShoot", "Standard",
                                  "JpegFormat",    "JPEG",
                                  "VideoTime",  "01:30:45",
                                  "VideoResolution", "8K",
                                  "VideoBitrate",    "30FPS",
                                  "VideoFramerate",  "100MB",
                                  "LiveTime", "00:00:00",
                                  "LiveResolution", "6K",
                                  "LiveBitrate",   "60M",
                                  "LiveFramerate", "20FPS",
                                  "ImageUrl",       "/nfsroot", NULL);
    
    g_object_get(G_OBJECT(entry), "ImageUrl", &image_route, NULL);
    g_print("PLease input num\n");
    scanf("%d", &num);
    g_object_set(G_OBJECT(entry), "interface", num, NULL);
    gchar time[20] = {0};
    list1 = g_list_insert(list1, "Interval:1/5s", 1);
    list2 = g_list_insert(list2, "BitRate:15Mbps", 1);
    list3 = g_list_insert(list3, "FrameRate:26FPS", 2);
    num = 0;
#if 1
    while (1)
    {
        
        g_object_set(G_OBJECT(entry), "livelist", list3, NULL);
    }
#endif
}


int main()
{
    oled_start();
}