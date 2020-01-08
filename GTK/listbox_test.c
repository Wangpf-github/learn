//编译：gcc listbox_test.c -o listbox_test `pkg-config --libs --cflags gtk+-3.0`

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <stdlib.h>
#include <string.h>
#include <gdk/gdkkeysyms.h>

int main(int argc, char* argv[])
{
    gchar buf[100] = {0};
    GtkWidget *window;
    GtkWidget *listbox;
    GtkWidget *label0;
    GtkWidget *label1;
    GtkWidget *label2;
    GtkWidget *label3;
    GtkWidget *listboxrow;
    GtkWidget *listboxrow1;
    GtkWidget *listboxrow2;
    GtkWidget *listboxrow3;
    GtkWidget *listboxrow4;
    GtkWidget *listboxrow5;
    GtkWidget *hbox;
    GtkWidget *image1;
    GtkWidget *image2;
    GtkWidget *event;
    gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_window_set_title(GTK_WINDOW(window), "AnyWhere");
    g_signal_connect(G_OBJECT(window), "destroy", 
        G_CALLBACK(gtk_main_quit), NULL);
    gtk_window_set_default_size(GTK_WINDOW(window), 128, 64);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    event = gtk_event_box_new();
    label0 = gtk_label_new(".....");
    label1 = gtk_label_new("Mode:Stanard");
    gtk_widget_set_size_request(label1, 85, 12);
    sprintf(buf, "<span foreground='black' font_desc='10'>Mode:Stanard</span>");  //修改label的字体大小，颜色等，可加下划线等
    gtk_label_set_markup(GTK_LABEL(label1), buf);
    label2 = gtk_label_new("Raw:On");
    gtk_widget_set_size_request(label2, 85, 12);
    sprintf(buf, "<span foreground='black' font_desc='10'>Raw:On</span>");  //修改label的字体大小，颜色等，可加下划线等
    gtk_label_set_markup(GTK_LABEL(label2), buf);
    label3 = gtk_label_new("Expo:Auto");
    gtk_widget_set_size_request(label3, 85, 12);
    sprintf(buf, "<span foreground='black' font_desc='10'>Expo:Auto</span>");  //修改label的字体大小，颜色等，可加下划线等
    gtk_label_set_markup(GTK_LABEL(label3), buf);  
    image1 = gtk_image_new_from_file("./pictures/ethnet.png");
    image2 = gtk_image_new_from_file("./pictures/battery.png");
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(hbox), image1, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), image2, FALSE, FALSE, 0);
//    gtk_box_set_spacing(GTK_BOX(hbox), 20);

    listbox = gtk_list_box_new();
    gtk_container_add(GTK_CONTAINER(window), listbox);

    listboxrow = gtk_list_box_row_new();     //创建一个空的row， 作用是不选择界面
    listboxrow1 = gtk_list_box_row_new();  
    gtk_container_add(GTK_CONTAINER(listboxrow1), hbox);
    gtk_widget_set_size_request(listboxrow1, 128, 12);
    listboxrow2 = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(listboxrow2), label1);
    listboxrow3 = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(listboxrow3), label2);
    listboxrow4 = gtk_list_box_row_new();     //创建新的row
    gtk_container_add(GTK_CONTAINER(listboxrow4), label3);    //将控件放到row中
    listboxrow5 = gtk_list_box_row_new();     //创建新的row

    gtk_list_box_insert(GTK_LIST_BOX(listbox), listboxrow1, 1);   //把row放入listbox中
    gtk_list_box_insert(GTK_LIST_BOX(listbox), listboxrow2, 2);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), listboxrow3, 3);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), listboxrow4, 4);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), listboxrow5, 5);

    gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(listboxrow5));    //GTK_LIST_BOX_ROW(listboxrow4)
//    gtk_list_box_unselect_all(GTK_LIST_BOX(listbox));

    gtk_widget_show_all(window);
    
    gtk_main();

    return FALSE;
}