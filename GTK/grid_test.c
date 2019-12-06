//编译：gcc grid_test.c -o grid_test `pkg-config --libs --cflags gtk+-3.0`

#include <gtk/gtk.h>
 
 
static void print_hello(GtkWidget *button , gpointer data);
static void activate(GtkApplication *app , gpointer data);
 
int main(int argc , char **argv)
{
    GtkApplication *app;
    int app_status;
 
    app = gtk_application_new("org.rain.gtk" , G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app , "activate" , G_CALLBACK(activate) , NULL);
    app_status = g_application_run(G_APPLICATION(app) , argc , argv);
 
    g_object_unref(app);
 
    return app_status;
}
 
static void print_hello(GtkWidget *button , gpointer data)
{
    printf("Hello,World!\n");
}
 
static void activate(GtkApplication *app , gpointer data)
{
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *button;
 
    //创建窗口
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window) , "grid test");
//    gtk_window_set_default_size(GTK_WINDOW(window) , 200 , 200);
 
    //创建网格
    grid = gtk_grid_new();
    
    //添加网格布局到window上
    gtk_container_set_border_width(GTK_CONTAINER(window) , 20);
    gtk_container_add(GTK_CONTAINER(window) , grid);
 
    //添加按钮一到位置(0,0）处，并设置高，宽均为一个单元
    button = gtk_button_new_with_label("Button1");
    g_signal_connect(button , "clicked" , G_CALLBACK(print_hello) , NULL);
    gtk_grid_attach(GTK_GRID(grid) , button , 0 , 0 , 1 , 1); 
 
    //添加按钮一到位置(1,0）处，并设置高，宽均为一个单元
    button = gtk_button_new_with_label("Button2");
    g_signal_connect(button , "clicked" , G_CALLBACK(print_hello) , NULL);
    gtk_grid_attach(GTK_GRID(grid) , button , 1 , 0 , 1 , 1);
 
    //添加按钮一到位置(0,1）处，并设置高为一个单元，宽为两个单元
    button = gtk_button_new_with_label("Exit");
    g_signal_connect_swapped(button , "clicked" , G_CALLBACK(gtk_widget_destroy) , window);
    gtk_grid_attach(GTK_GRID(grid) , button , 2 , 0 , 1 , 1);
    
    //添加按钮一到位置(1,1）处，并设置高为一个单元，宽为两个单元
    button = gtk_button_new_with_label("Exit");
    g_signal_connect_swapped(button , "clicked" , G_CALLBACK(gtk_widget_destroy) , window);
    gtk_grid_attach(GTK_GRID(grid) , button , 1 , 1 , 2 , 1);
    
    gtk_widget_show_all(window);
 
}