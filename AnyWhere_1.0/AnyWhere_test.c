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
    
    g_object_set(G_OBJECT(entry), "interface", 0, 
                                 "jpeg_list", list1,
                                 "MP4_list", list2,
                                 "live_list", list3,
                                 "system_list", list4,
                                 "interface_select", 0,
                                 "network_mode", NETWORK_WIRE,
                                 "IPaddr", "192.168.10.100",
                                 "battery", BATTERY_P_100,
                                 "jpeg_disk_space_3", "12300",
                                 "jpeg_resolution_3", "8K",
                                 "jpeg_shoot_mode_3", "Standard",
                                 "jpeg_format_3", "JPEG",
                                 "mp4_work_time_6", "1:30:45",
                                 "mp4_resolution_6", "8K",
                                 "mp4_bitrate_6", "30FPS",
                                 "mp4_framerate_6", "100MB",
                                 "live_work_time_9", "03:10:19",
                                 "live_resolution_9", "6K",
                                 "live_bitrate_9", "60M",
                                 "live_framerate_9", "20FPS",
                                 "image_url", "/nfsroot", NULL);

    g_object_get(G_OBJECT(entry), "image_url", &image_route, NULL);
    printf("image_route is %s\n", image_route);
    while (1)
    {
        g_print("PLease input num\n");
        scanf("%d", &num);
        
        g_object_set(G_OBJECT(entry), "interface", num, NULL);
            
    }
}

int main()
{
    oled_start();
}