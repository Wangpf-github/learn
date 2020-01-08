//编译：aarch64-buildroot-linux-gnu-gcc GTK_3559.c -o GTK_3559 `pkg-config --libs --cflags gtk+-3.0`

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <stdlib.h>
#include <string.h>
#include <gdk/gdkkeysyms.h>

int main(int argc, char* argv[])
{
    GtkWidget *window;
    GtkWidget *image;
    gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_window_set_title(GTK_WINDOW(window), "AnyWhere");
    g_signal_connect(G_OBJECT(window), "destroy", 
        G_CALLBACK(gtk_main_quit), NULL);
    gtk_window_set_default_size(GTK_WINDOW(window), 128, 64);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    image = gtk_image_new_from_file("./pictures/start.png");   //加载图片的路径
    gtk_container_add(GTK_CONTAINER(window),image);

    gtk_widget_show_all(window);
    gtk_main();

    return FALSE;
}