//编译：gcc first_gtk.c -o first_gtk `pkg-config --libs --cflags gtk+-3.0`
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <stdlib.h>
#include <string.h>
#include <gdk/gdkkeysyms.h>

int show_jpeg_mode(GtkWidget *widget, gpointer data);
int show_mp4_mode(GtkWidget *widget, gpointer data);
int show_live_mode(GtkWidget *widget, gpointer data);
int show_setting_mode(GtkWidget *widget, gpointer data);

int show_jpeg_mode(GtkWidget *widget, gpointer data)
{
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *vbox;
    GtkWidget *label_IPaddr;
    window = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_window_set_title(GTK_WINDOW(window), "AnyWhere");
    g_signal_connect(G_OBJECT(window), "destroy", 
        G_CALLBACK(gtk_main_quit), NULL);
    gtk_window_set_default_size(GTK_WINDOW(window), 128, 94);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window),vbox);

    button = gtk_button_new_with_label ("select");
    g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (show_mp4_mode), "1");
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);
  
    label_IPaddr = gtk_label_new("192.168.100.100");

    gtk_box_pack_start(GTK_BOX(vbox), label_IPaddr, FALSE, FALSE, 0);

    gtk_widget_show_all(window);
    return 0;
}

int show_mp4_mode(GtkWidget *widget, gpointer data)
{
    gchar buf[100] = {0};
    static guint last_num = 95;
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *header_box;
    GtkWidget *vbox;
    GtkWidget *image_camera;
    GtkWidget *image_eth;
    GtkWidget *image_battery;
    GtkWidget *label_IPaddr;
    GtkWidget *event_box;
    window = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_window_set_title(GTK_WINDOW(window), "AnyWhere");
    g_signal_connect(G_OBJECT(window), "destroy", 
        G_CALLBACK(gtk_main_quit), NULL);
    gtk_window_set_default_size(GTK_WINDOW(window), 128, 90);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window),vbox);

    //创建hbox并加入vbox
    header_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_box_pack_start(GTK_BOX(vbox),header_box, FALSE, FALSE, 0);

    //创建网口图片窗口并加入hbox
    image_eth = gtk_image_new_from_file("./pictures/ethnet.png");   //加载图片的路径
    gtk_box_pack_start(GTK_BOX(header_box), image_eth, FALSE, FALSE, 0);

    //创建事件盒子并加入vbox
    event_box = gtk_event_box_new();
    GdkRGBA event_bg_color;
    event_bg_color.red = 100;
    event_bg_color.green = 100;
    event_bg_color.blue = 100;
//    event_bg_color.alpha = 1;
    gtk_widget_override_background_color(event_box, GTK_STATE_FLAG_NORMAL, &event_bg_color);     //修改事件box背景
    gtk_box_pack_start(GTK_BOX(header_box), event_box, FALSE, FALSE, 0);

    //创建标签并加入事件box
    label_IPaddr = gtk_label_new("192.168.100.100");
    gtk_widget_set_size_request(label_IPaddr,85,20);
    sprintf(buf, "<span foreground='black' font_desc='8'>192.168.100.%d</span>", last_num);  //修改label的字体大小，颜色等，可加下划线等
    last_num++;
    gtk_label_set_markup(GTK_LABEL(label_IPaddr), buf);  
    gtk_container_add(GTK_CONTAINER(event_box), label_IPaddr);

    //创建电池图片窗口并加入hbox
    image_battery = gtk_image_new_from_file("./pictures/battery.png");   //加载图片的路径
    gtk_box_pack_start(GTK_BOX(header_box), image_battery, FALSE, FALSE, 0);
    
    //创建拍照模式图片窗口并加入vbox
    image_camera = gtk_image_new_from_file("./pictures/camera.png");   //加载图片的路径
    gtk_box_pack_start(GTK_BOX(vbox), image_camera, FALSE, FALSE, 0);
    
    //创建测试按钮并加入vbox
    button = gtk_button_new_with_label ("select");
    g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (show_mp4_mode), "1");
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);

    gtk_widget_show_all(window);
    return 0;
}

int show_live_mode(GtkWidget *widget, gpointer data)
{
    return 0;
}

int show_setting_mode(GtkWidget *widget, gpointer data)
{

}

int main(int argc, char* argv[])
{
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *vbox;
    GtkWidget *image;
    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_window_set_title(GTK_WINDOW(window), "AnyWhere");
    g_signal_connect(G_OBJECT(window), "destroy", 
        G_CALLBACK(gtk_main_quit), NULL);
    gtk_window_set_default_size(GTK_WINDOW(window), 128, 64);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);     //创建新盒子，第二个参数为单元格之间的间隙，即控件之间的间隔大小，一般为0
    gtk_container_add(GTK_CONTAINER(window),vbox);

    button = gtk_button_new_with_label ("select");
    g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK(show_mp4_mode), NULL);     //采用信号机制，收到信号调用show_jpeg_mode
//    g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (gtk_widget_hide), "1");
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);
     
    image = gtk_image_new_from_file("./pictures/start.png");   //加载图片的路径
    gtk_box_pack_start(GTK_BOX(vbox), image, FALSE, FALSE, 0);

    gtk_widget_show_all(window);

    gtk_main();

    return FALSE;
}