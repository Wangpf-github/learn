//export PATH=/home/linux/git/buildroot-env/output/host/bin:$PATH
//环境：export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/usr/lib/x86_64-linux-gnu/pkgconfig/
//编译：gcc gtk_hello.c -o gtk_hello `pkg-config --libs --cflags gtk+-3.0`
/* 创建SPLASH窗口 splash.c */
#include <gtk/gtk.h>
//主函数
int main (int argc,char* argv[])
{
   GtkWidget* window ;
   GtkWidget* vbox ;
   GtkWidget* image ;
   GtkWidget* button ;
 
   gtk_init(&argc,&argv);
 
   window = gtk_window_new(GTK_WINDOW_POPUP);             //////////no boundary
   gtk_window_set_title(GTK_WINDOW(window),"Splash");
   g_signal_connect(G_OBJECT(window),"destroy",
      G_CALLBACK(gtk_main_quit),NULL);
   //gtk_container_set_border_width(GTK_CONTAINER(window),20);
   //gtk_window_set_default_size(GTK_WINDOW(window),500,400);
   gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
 
   vbox = gtk_vbox_new(FALSE,0);
   gtk_container_add(GTK_CONTAINER(window),vbox);
   image = gtk_image_new_from_file("../cairo/start.png");
   gtk_box_pack_start(GTK_BOX(vbox),image,FALSE,FALSE,0);
   button = gtk_button_new_with_label("Splash");
   g_signal_connect(G_OBJECT(button),"clicked",
      G_CALLBACK(gtk_main_quit),NULL);
   gtk_box_pack_start(GTK_BOX(vbox),button,FALSE,FALSE,0);
   gtk_widget_show_all(window);
   gtk_main();
   return FALSE;
}