//编译：gcc first_gtk.c -o first_gtk `pkg-config --libs --cflags gtk+-3.0`
#include <gtk/gtk.h>

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

    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window),vbox);

    button = gtk_button_new_with_label ("select");
    g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (show_mp4_mode), "1");
    g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (gtk_widget_destroy), "1");
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);
  
    label_IPaddr = gtk_label_new("192.168.100.100");

    gtk_box_pack_start(GTK_BOX(vbox), label_IPaddr, FALSE, FALSE, 0);

    gtk_widget_show_all(window);
    return 0;
}

int show_mp4_mode(GtkWidget *widget, gpointer data)
{
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *header_box;
    GtkWidget *vbox;
    GtkWidget *image_camera;
    GtkWidget *image_eth;
    GtkWidget *image_battery;
    GtkWidget *label_IPaddr;
    window = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_window_set_title(GTK_WINDOW(window), "AnyWhere");
    g_signal_connect(G_OBJECT(window), "destroy", 
        G_CALLBACK(gtk_main_quit), NULL);
    gtk_window_set_default_size(GTK_WINDOW(window), 128, 94);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    header_box = gtk_hbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window),header_box);

    image_eth = gtk_image_new_from_file("./pictures/ethnet.png");   //加载图片的路径
    gtk_box_pack_start(GTK_BOX(header_box), image_eth, FALSE, FALSE, 0);

    label_IPaddr = gtk_label_new("192.168.100.100");
    gtk_widget_set_size_request(label_IPaddr,50,10);
    gtk_box_pack_start(GTK_BOX(header_box), label_IPaddr, FALSE, FALSE, 0);

    image_battery = gtk_image_new_from_file("./pictures/battery.png");   //加载图片的路径
    gtk_box_pack_start(GTK_BOX(header_box), image_battery, FALSE, FALSE, 0);
    
    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window),vbox);

    button = gtk_button_new_with_label ("select");
    g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (show_jpeg_mode), "1");
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(vbox), header_box, FALSE, FALSE, 0);
  
    image_camera = gtk_image_new_from_file("./pictures/camera.png");   //加载图片的路径
    gtk_box_pack_start(GTK_BOX(vbox), image_camera, FALSE, FALSE, 0);

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

    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window),vbox);

    button = gtk_button_new_with_label ("select");
    g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (show_jpeg_mode), "1");
    g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (gtk_widget_hide), "1");
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);
     
    image = gtk_image_new_from_file("./pictures/start.png");   //加载图片的路径
    gtk_box_pack_start(GTK_BOX(vbox), image, FALSE, FALSE, 0);

    gtk_widget_show_all(window);

    gtk_main();

    return FALSE;
}