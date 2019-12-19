#include "AnyWhere_GTK.h"

gint show_system()
{
    gchar IP_buf[100] = {0};
    GtkWidget *window;
    GtkWidget *header_box;
    GtkWidget *vbox;
    GtkWidget *image_setting;
    GtkWidget *image_eth;
    GtkWidget *image_battery;
    GtkWidget *label_IPaddr;
    window = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_window_set_title(GTK_WINDOW(window), "system setting");
    gtk_window_set_default_size(GTK_WINDOW(window), 128, 64);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window),vbox);

    //创建hbox并加入vbox
    header_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_box_pack_start(GTK_BOX(vbox),header_box, FALSE, FALSE, 0);

    //创建网口图片窗口并加入hbox
    image_eth = gtk_image_new_from_file(get_network_image_url());   //加载图片的路径
    gtk_box_pack_start(GTK_BOX(header_box), image_eth, FALSE, FALSE, 0);

    //创建标签并加入hbox
    label_IPaddr = gtk_label_new("IP addr");
    sprintf(IP_buf, "<span foreground='black' font_desc='10'>%s</span>", state_data.IPaddr);  //修改label的字体大小，颜色等，可加下划线等
    gtk_label_set_markup(GTK_LABEL(label_IPaddr), IP_buf);  
    gtk_widget_set_size_request(label_IPaddr, 84, 11);
    gtk_container_add(GTK_BOX(header_box), label_IPaddr);

    //创建电池图片窗口并加入hbox
    image_battery = gtk_image_new_from_file(get_battery_image_url());   
    gtk_box_pack_start(GTK_BOX(header_box), image_battery, FALSE, FALSE, 0);
    
    //创建拍照模式图片窗口并加入vbox
    image_setting = gtk_image_new_from_file(get_work_mode_image_big_url(WORKMODE_SETTING));   //加载图片的路径
    gtk_box_pack_start(GTK_BOX(vbox), image_setting, FALSE, FALSE, 0);

    gtk_widget_show_all(window);
    return FALSE;
}

gint show_sys_setting1()
{
    guint value;
    gchar wifi_buf[100] = {0};
    gchar led_buf[100] = {0};
    gchar lan_buf[100] = {0};
    GtkWidget *window;
    GtkWidget *image;
    GtkWidget *label_wifi;
    GtkWidget *label_led;
    GtkWidget *label_lan;
    GtkWidget *listbox;
    GtkWidget *row_image_head;
    GtkWidget *row_label_wifi;
    GtkWidget *row_label_led;
    GtkWidget *row_label_lan;
    GtkWidget *row_spare;

    guint value;
    g_object_get(GOBJECT(entry), "interface_select", &value, NULL);

    //创建底图
    window = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_window_set_default_size(GTK_WINDOW(window), 128, 64);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    //创建一个listbox
    listbox = gtk_list_box_new();
    gtk_container_add(GTK_CONTAINER(window), listbox);
    //插入表头图片
    image = gtk_image_new_from_file(get_work_mode_image_sel_url(WORKMODE_SETTING, SELECT_NO));
    //创建标签
    label_wifi = gtk_label_new("resolution");
    gtk_widget_set_size_request(label_wifi, 85, 15);
    sprintf(wifi_buf, "<span foreground='black' font_desc='10'>%s</span>", get_setting_wifi_mode());  
    label_led = gtk_label_new("bitrate");
    gtk_widget_set_size_request(label_led, 85, 15);
    sprintf(led_buf, "<span foreground='black' font_desc='10'>%s</span>", get_setting_led_mode());  
    label_lan = gtk_label_new("framerate");
    gtk_widget_set_size_request(label_lan, 85, 15);
    sprintf(lan_buf, "<span foreground='black' font_desc='10'>%s</span>", get_setting_lan_mode()); 
    
    //创建row，将标签和图片插入row中
    row_image_head = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_image_head), image);
    row_label_wifi = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_label_wifi), label_wifi);
    row_label_led = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_label_led), label_led);
    row_label_lan = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_label_lan), label_lan);
    row_spare = gtk_list_box_row_new();
    
    //将row插入到listbox中
    gtk_list_box_prepend(GTK_LIST_BOX(listbox), row_image_head);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_label_wifi, 2);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_label_led, 3);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_label_lan, 4);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_spare, 5);

    switch (value)
    {
    case SELECT_NONE:
        gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_spare));
        break;
    case SELECT_HEAD:
        image = gtk_image_new_from_file(get_work_mode_image_sel_url(WORKMODE_SETTING, SELECT_YES));
        gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_image_head));
        break;
    case SELECT_FIRST:
        sprintf(wifi_buf, "<span foreground='white' font_desc='10'>%s</span>", get_setting_wifi_mode());  
        gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_label_wifi));
        break;
    case SELECT_SECOND:
        sprintf(led_buf, "<span foreground='white' font_desc='10'>%s</span>", get_setting_led_mode());
        gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_label_led));
        break;
    case SELECT_THIRD:
        sprintf(lan_buf, "<span foreground='white' font_desc='10'>%s</span>", get_setting_lan_mode());
        gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_label_lan));
        break;
    default:
        break;
    }
    gtk_label_set_markup(GTK_LABEL(label_wifi), wifi_buf);
    gtk_label_set_markup(GTK_LABEL(label_led), led_buf);
    gtk_label_set_markup(GTK_LABEL(label_lan), lan_buf);

    gtk_widget_show_all(window);
    return FALSE;
}

gint show_sys_setting_wifi()
{
    guint value;
    gchar wifi_buf1[100] = {0};
    gchar wifi_buf2[100] = {0};
    gchar wifi_buf3[100] = {0};
    GtkWidget *window;
    GtkWidget *image;
    GtkWidget *label_wifi1;
    GtkWidget *label_wifi2;
    GtkWidget *label_wifi3;
    GtkWidget *listbox;
    GtkWidget *row_image_head;
    GtkWidget *row_label_wifi1;
    GtkWidget *row_label_wifi2;
    GtkWidget *row_label_wifi3;
    GtkWidget *row_spare;

    guint value;
    g_object_get(GOBJECT(entry), "interface_select", &value, NULL);

    //创建底图
    window = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_window_set_default_size(GTK_WINDOW(window), 128, 64);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    //创建一个listbox
    listbox = gtk_list_box_new();
    gtk_container_add(GTK_CONTAINER(window), listbox);
    //插入表头图片
    image = gtk_image_new_from_file(get_work_mode_image_sel_url(WORKMODE_SETTING, SELECT_NO));
    //创建标签
    label_wifi1 = gtk_label_new("resolution");
    gtk_widget_set_size_request(label_wifi1, 85, 15);
    sprintf(wifi_buf1, "<span foreground='black' font_desc='10'>WIFI:AP</span>");  
    label_wifi2 = gtk_label_new("bitrate");
    gtk_widget_set_size_request(label_wifi2, 85, 15);
    sprintf(wifi_buf2, "<span foreground='black' font_desc='10'>WIFI:Station</span>");  
    label_wifi3 = gtk_label_new("framerate");
    gtk_widget_set_size_request(label_wifi3, 85, 15);
    sprintf(wifi_buf3, "<span foreground='black' font_desc='10'>WIFI:OFF</span>"); 
    
    //创建row，将标签和图片插入row中
    row_image_head = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_image_head), image);
    row_label_wifi1 = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_label_wifi1), label_wifi1);
    row_label_wifi2 = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_label_wifi2), label_wifi2);
    row_label_wifi3 = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_label_wifi3), label_wifi3);
    row_spare = gtk_list_box_row_new();
    
    //将row插入到listbox中
    gtk_list_box_prepend(GTK_LIST_BOX(listbox), row_image_head);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_label_wifi1, 2);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_label_wifi2, 3);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_label_wifi3, 4);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_spare, 5);

    switch (value)
    {
    case SELECT_NONE:
        gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_spare));
        break;
    case SELECT_HEAD:
        image = gtk_image_new_from_file(get_work_mode_image_sel_url(WORKMODE_SETTING, SELECT_YES));
        gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_image_head));
        break;
    case SELECT_FIRST:
        sprintf(wifi_buf1, "<span foreground='white' font_desc='10'>WIFI:AP</span>");  
        gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_label_wifi1));
        break;
    case SELECT_SECOND:
        sprintf(wifi_buf2, "<span foreground='white' font_desc='10'>WIFI:Station</span>");
        gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_label_wifi2));
        break;
    case SELECT_THIRD:
        sprintf(wifi_buf3, "<span foreground='white' font_desc='10'>WIFI:OFF</span>");
        gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_label_wifi3));
        break;
    default:
        break;
    }
    gtk_label_set_markup(GTK_LABEL(label_wifi1), wifi_buf1);
    gtk_label_set_markup(GTK_LABEL(label_wifi2), wifi_buf2);
    gtk_label_set_markup(GTK_LABEL(label_wifi3), wifi_buf3);

    gtk_widget_show_all(window);
    return FALSE;
}

gint show_sys_setting_led()
{
    guint value;
    gchar LED_buf1[100] = {0};
    gchar LED_buf2[100] = {0};
    GtkWidget *window;
    GtkWidget *image;
    GtkWidget *label_LED1;
    GtkWidget *label_LED2;
    GtkWidget *listbox;
    GtkWidget *row_image_head;
    GtkWidget *row_label_LED1;
    GtkWidget *row_label_LED2;
    GtkWidget *row_spare;

    guint value;
    g_object_get(GOBJECT(entry), "interface_select", &value, NULL);

    //创建底图
    window = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_window_set_default_size(GTK_WINDOW(window), 128, 64);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    //创建一个listbox
    listbox = gtk_list_box_new();
    gtk_container_add(GTK_CONTAINER(window), listbox);
    //插入表头图片
    image = gtk_image_new_from_file(get_work_mode_image_sel_url(WORKMODE_SETTING, SELECT_NO));
    //创建标签
    label_LED1 = gtk_label_new("resolution");
    gtk_widget_set_size_request(label_LED1, 85, 15);
    sprintf(LED_buf1, "<span foreground='black' font_desc='10'>LED:ON</span>");  
    label_LED2 = gtk_label_new("bitrate");
    gtk_widget_set_size_request(label_LED2, 85, 15);
    sprintf(LED_buf2, "<span foreground='black' font_desc='10'>LED:OFF</span>");  
    
    //创建row，将标签和图片插入row中
    row_image_head = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_image_head), image);
    row_label_LED1 = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_label_LED1), label_LED1);
    row_label_LED2 = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_label_LED2), label_LED2);
    row_spare = gtk_list_box_row_new();
    
    //将row插入到listbox中
    gtk_list_box_prepend(GTK_LIST_BOX(listbox), row_image_head);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_label_LED1, 2);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_label_LED2, 3);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_spare, 4);

    switch (value)
    {
    case SELECT_NONE:
        gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_spare));
        break;
    case SELECT_HEAD:
        image = gtk_image_new_from_file(get_work_mode_image_sel_url(WORKMODE_SETTING, SELECT_YES));
        gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_image_head));
        break;
    case SELECT_FIRST:
        sprintf(LED_buf1, "<span foreground='white' font_desc='10'>LED:ON</span>");  
        gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_label_LED1));
        break;
    case SELECT_SECOND:
        sprintf(LED_buf2, "<span foreground='white' font_desc='10'>LED:OFF</span>");
        gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_label_LED2));
        break;
    default:
        break;
    }
    gtk_label_set_markup(GTK_LABEL(label_LED1), LED_buf1);
    gtk_label_set_markup(GTK_LABEL(label_LED2), LED_buf2);

    gtk_widget_show_all(window);
    return FALSE;
}
