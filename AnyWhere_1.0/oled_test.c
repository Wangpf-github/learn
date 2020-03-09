//aarch64-buildroot-linux-gnu-gcc oled_test.c -o oled_test `pkg-config --libs --cflags glib-2.0 gobject-2.0` -I. -L. -loled_display

#include <oled_display.h>
#include <cairo/cairo.h>
#include <fcntl.h>
#include <linux/types.h>
#include <linux/ioctl.h>
#include <linux/fb.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>

gboolean oled_test(gpointer data)
{
    gint num = 0;

    g_print("PLease input num\n");
    scanf("%d", &num);
    if(num == 0)
    {
        g_object_unref(data);
        return FALSE;
    }

    g_object_set(G_OBJECT(data), "interface", num, NULL);

    return TRUE;
}

/* 测试程序 */
int main()
{
    GMainLoop *loop;
    gint num;
    OledScreen *oled;
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

    oled = g_object_new(TYPE_OLED_SCREEN, "interface",     0, 
                                        "JpegList",        list1,
                                        "VideoList",       list2,
                                        "LiveList",        list3,
                                        "SystemList",      list4,
                                        "InterfaceSelect", 0,
                                        "NetworkMode",     NETWORK_WIRE,
                                        "IPaddr",          "255.255.255.255",
                                        "battery",          BATTERY_P_100,
                                        "JpegDiskSpace",   "123456789",
                                        "JpegResolution",  "8K",
                                        "JpegShootMode",   "Standard",
                                        "JpegFormat",      "JPEG",
                                        "VideoWorkTime",   "01:30:45",
                                        "VideoResolution", "8K",
                                        "VideoBitrate",    "30FPS",
                                        "VideoFramerate",  "100MB",
                                        "LiveWorkTime",    "00:00:00",
                                        "LiveResolution",  "6K",
                                        "LiveBitrate",     "60M",
                                        "LiveFramerate",   "20FPS",
                                        "ImagePath",       "/test", NULL);

//    g_print("PLease input num\n");
//    scanf("%d", &num);
//    g_object_set(G_OBJECT(entry), "interface", num, NULL);
    gchar time[20] = {0};
    list1 = g_list_insert(list1, "Interval:1/5s", 1);
    list2 = g_list_insert(list2, "BitRate:15Mbps", 1);
    list3 = g_list_insert(list3, "FrameRate:26FPS", 2);
    
    g_timeout_add(100, oled_test, oled);

    loop =  g_main_loop_new(NULL, TRUE);
    g_main_loop_run(loop);

}
