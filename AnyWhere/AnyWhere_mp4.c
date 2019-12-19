#include "AnyWhere_GTK.h"

gint show_mp4()
{
    gchar IP_buf[100] = {0};
    GtkWidget *window;
//    GtkWidget *button;
    GtkWidget *header_box;
    GtkWidget *vbox;
    GtkWidget *image_camera;
    GtkWidget *image_eth;
    GtkWidget *image_battery;
    GtkWidget *label_IPaddr;
    window = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_window_set_title(GTK_WINDOW(window), "mp4 mode");
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
    image_camera = gtk_image_new_from_file(get_work_mode_image_big_url(WORKMODE_MP4));   //加载图片的路径
    gtk_box_pack_start(GTK_BOX(vbox), image_camera, FALSE, FALSE, 0);

    gtk_widget_show_all(window);
    return FALSE;
}

gint show_mp4_state()
{
    GtkWidget *window;
    GtkWidget *hbox1;
    GtkWidget *hbox2;
    GtkWidget *vbox;
    GtkWidget *label_state;
    GtkWidget *label_IP;
    GtkWidget *label_time;
    GtkWidget *image_network;
    GtkWidget *image_battery;
    GtkWidget *image_camera;
    gchar buf_state[100] = {0};
    gchar buf_ipaddr[100] = {0};
    gchar work_time[100] = {0};
    window = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_window_set_default_size(GTK_WINDOW(window), 128, 64);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    //第一行显示信息
    hbox1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
    gtk_box_pack_start(GTK_BOX(vbox), hbox1, FALSE, FALSE, 0);
    
    image_network = gtk_image_new_from_file(get_network_image_url());
    gtk_box_pack_start(GTK_BOX(hbox1), image_network, FALSE, FALSE, 0);
    label_IP = gtk_label_new("IPaddr");
    gtk_widget_set_size_request(label_IP, 84， 11);
    sprintf(buf_ipaddr, "<span foreground='black' font_desc='10'>%s</span>", state_data.IPaddr);
    gtk_label_set_markup(GTK_LABEL(label_IP), buf_ipaddr);
    gtk_box_pack_start(GTK_BOX(hbox1), label_IP);
    image_battery = gtk_image_new_from_file(get_battery_image_url());
    gtk_box_pack_start(GTK_BOX(hbox1), image_battery, FALSE, FALSE, 0);

    //第二行显示信息
    hbox2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
    gtk_box_pack_start(GTK_BOX(vbox), hbox2, FALSE, FALSE, 0);
    image_camera = gtk_image_new_from_file(get_state_mode_image_url(WORKMODE_MP4));
    gtk_box_pack_start(GTK_BOX(hbox2), image_camera);
    label_time = gtk_label_new("time");
    gtk_widget_set_size_request(label_time, 100， 20);
    sprintf(work_time, "<span foreground='black' font_desc='18'> %s </span>", get_work_time_string(WORKMODE_MP4));
    gtk_label_set_markup(GTK_LABEL(label_time), work_time);
    gtk_box_pack_start(GTK_BOX(hbox2), label_time);

    //创建状态标签,第三行显示信息
    label_state = gtk_label_new("status");
    gtk_widget_set_size_request(label_state, 120, 15);
    sprintf(buf_state, "<span foreground='black' font_desc='12'>8K 30PFS 100Mb</span>");  //修改label的字体大小，颜色等，可加下划线等
    gtk_label_set_markup(GTK_LABEL(label_state), buf_state);
    gtk_box_pack_start(GTK_BOX(vbox), label_state, FALSE, FALSE, 0);

    gtk_widget_show_all(window);
    return FALSE;
}

gint show_mp4_setting1()
{
    guint value;
    gchar reso_buf[100] = {0};
    gchar bit_buf[100] = {0};
    gchar frame_buf[100] = {0};
    GtkWidget *window;
    GtkWidget *image;
    GtkWidget *label_resolution;
    GtkWidget *label_bitrate;
    GtkWidget *label_framerate;
    GtkWidget *listbox;
    GtkWidget *row_image_head;
    GtkWidget *row_label_resolution;
    GtkWidget *row_label_bitrate;
    GtkWidget *row_spare;
    GtkWidget *row_label_framerate;

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
    image = gtk_image_new_from_file(get_work_mode_image_sel_url(WORKMODE_MP4, SELECT_NO));
    //创建三个标签
    label_resolution = gtk_label_new("resolution");
    gtk_widget_set_size_request(label_resolution, 85, 15);
    sprintf(reso_buf, "<span foreground='black' font_desc='10'>Resolution:8K</span>");  
    label_bitrate = gtk_label_new("bitrate");
    gtk_widget_set_size_request(label_bitrate, 85, 15);
    sprintf(bit_buf, "<span foreground='black' font_desc='10'>BitRate:100Mbps</span>");  
    label_framerate = gtk_label_new("framerate");
    gtk_widget_set_size_request(label_framerate, 85, 15);
    sprintf(frame_buf, "<span foreground='black' font_desc='10'>FrameRate:30FPS</span>"); 
    
    //创建row，将标签和图片插入row中
    row_image_head = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_image_head), image);
    row_label_resolution = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_label_resolution), label_resolution);
    row_label_bitrate = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_label_bitrate), label_bitrate);
    row_label_framerate = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_label_framerate), label_framerate);
    row_spare = gtk_list_box_row_new();
    
    //将row插入到listbox中
    gtk_list_box_prepend(GTK_LIST_BOX(listbox), row_image_head);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_label_resolution, 2);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_label_bitrate, 3);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_label_framerate, 4);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_spare, 5);

    switch (value)
    {
    case SELECT_NONE:
        gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_spare));
        break;
    case SELECT_HEAD:
        image = gtk_image_new_from_file(get_work_mode_image_sel_url(WORKMODE_MP4, SELECT_YES));
        gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_image_head));
        break;
    case SELECT_FIRST:
        sprintf(reso_buf, "<span foreground='white' font_desc='10'>Resolution:8K</span>");  
        gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_label_resolution));
        break;
    case SELECT_SECOND:
        sprintf(bit_buf, "<span foreground='white' font_desc='10'>BitRate:100Mbps</span>");  
        gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_label_bitrate));
        break;
    case SELECT_THIRD:
        sprintf(frame_buf, "<span foreground='white' font_desc='10'>FrameRate:30FPS</span>"); 
        gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_label_framerate));
        break;
    
    default:
        break;
    }
    gtk_label_set_markup(GTK_LABEL(label_resolution), reso_buf);
    gtk_label_set_markup(GTK_LABEL(label_bitrate), bit_buf);
    gtk_label_set_markup(GTK_LABEL(label_framerate), frame_buf);  

    gtk_widget_show_all(window);
    return FALSE;
}
gint show_mp4_setting2()
{
    guint value;
    gchar sepa_buf[100] = {0};
    GtkWidget *window;
    GtkWidget *image;
    GtkWidget *label_separate;
    GtkWidget *listbox;
    GtkWidget *row_image_head;
    GtkWidget *row_label_separate;
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
    image = gtk_image_new_from_file(get_work_mode_image_sel_url(WORKMODE_MP4, SELECT_NO));
    //创建标签
    label_separate = gtk_label_new("separate");
    gtk_widget_set_size_request(label_separate, 85, 15);
    sprintf(sepa_buf, "<span foreground='black' font_desc='10'>%s</span>", get_mp4_separate_string());  
    
    //创建row，将标签和图片插入row中
    row_image_head = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_image_head), image);
    row_label_separate = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_label_separate), label_separate);
    row_spare = gtk_list_box_row_new();
    
    //将row插入到listbox中
    gtk_list_box_prepend(GTK_LIST_BOX(listbox), row_image_head);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_label_separate, 2);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_spare, 3);

    switch (value)
    {
    case SELECT_NONE:
        gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_spare));
        break;
    case SELECT_HEAD:
        image = gtk_image_new_from_file(get_work_mode_image_sel_url(WORKMODE_MP4, SELECT_YES));
        gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_image_head));
        break;
    case SELECT_FIRST:
        sprintf(sepa_buf, "<span foreground='white' font_desc='10'>%s</span>", get_mp4_separate_string());  
        gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_label_separate));
        break;
    default:
        break;
    }
    gtk_label_set_markup(GTK_LABEL(label_separate), sepa_buf);

    gtk_widget_show_all(window);
    return FALSE;
}

gint show_mp4_setting_separate1()
{
    guint value;
    gchar sepa1_buf[100] = {0};
    gchar sepa2_buf[100] = {0};
    gchar sepa3_buf[100] = {0};
    GtkWidget *window;
    GtkWidget *image;
    GtkWidget *label_separate1;
    GtkWidget *label_separate2;
    GtkWidget *label_separate3;
    GtkWidget *listbox;
    GtkWidget *row_image_head;
    GtkWidget *row_label_separate1;
    GtkWidget *row_label_separate2;
    GtkWidget *row_spare;
    GtkWidget *row_label_separate3;

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
    image = gtk_image_new_from_file(get_work_mode_image_sel_url(WORKMODE_MP4, SELECT_NO));
    //创建标签
    label_separate1 = gtk_label_new("resolution");
    gtk_widget_set_size_request(label_separate1, 85, 15);
    sprintf(sepa1_buf, "<span foreground='black' font_desc='10'>10min</span>");  
    label_separate2 = gtk_label_new("bitrate");
    gtk_widget_set_size_request(label_separate2, 85, 15);
    sprintf(sepa2_buf, "<span foreground='black' font_desc='10'>20min</span>");  
    label_separate3 = gtk_label_new("framerate");
    gtk_widget_set_size_request(label_separate3, 85, 15);
    sprintf(sepa3_buf, "<span foreground='black' font_desc='10'>30min</span>"); 
    
    //创建row，将标签和图片插入row中
    row_image_head = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_image_head), image);
    row_label_separate1 = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_label_separate1), label_separate1);
    row_label_separate2 = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_label_separate2), label_separate2);
    row_label_separate3 = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_label_separate3), label_separate3);
    row_spare = gtk_list_box_row_new();
    
    //将row插入到listbox中
    gtk_list_box_prepend(GTK_LIST_BOX(listbox), row_image_head);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_label_separate1, 2);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_label_separate2, 3);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_label_separate3, 4);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_spare, 5);

    switch (value)
    {
    case SELECT_NONE:
        gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_spare));
        break;
    case SELECT_HEAD:
        image = gtk_image_new_from_file(get_work_mode_image_sel_url(WORKMODE_MP4, SELECT_YES));
        gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_image_head));
        break;
    case SELECT_FIRST:
        sprintf(sepa1_buf, "<span foreground='white' font_desc='10'>10min</span>");  
        gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_label_separate1));
        break;
    case SELECT_SECOND:
        sprintf(sepa2_buf, "<span foreground='white' font_desc='10'>20min</span>");  
        gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_label_separate2));
        break;
    case SELECT_THIRD:
        sprintf(sepa3_buf, "<span foreground='white' font_desc='10'>30min</span>"); 
        gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_label_separate3));
        break;
    default:
        break;
    }
    gtk_label_set_markup(GTK_LABEL(label_separate1), sepa1_buf);
    gtk_label_set_markup(GTK_LABEL(label_separate2), sepa2_buf);
    gtk_label_set_markup(GTK_LABEL(label_separate3), sepa3_buf);

    gtk_widget_show_all(window);
    return FALSE;
}

gint show_mp4_setting_separate2()
{
    guint value;
    gchar sepa1_buf[100] = {0};
    gchar sepa2_buf[100] = {0};
    GtkWidget *window;
    GtkWidget *image;
    GtkWidget *label_separate1;
    GtkWidget *label_separate2;
    GtkWidget *listbox;
    GtkWidget *row_image_head;
    GtkWidget *row_label_separate1;
    GtkWidget *row_label_separate2;
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
    image = gtk_image_new_from_file(get_work_mode_image_sel_url(WORKMODE_MP4, SELECT_NO));
    //创建标签
    label_separate1 = gtk_label_new("resolution");
    gtk_widget_set_size_request(label_separate1, 85, 15);
    sprintf(sepa1_buf, "<span foreground='black' font_desc='10'>60min</span>");  
    label_separate2 = gtk_label_new("bitrate");
    gtk_widget_set_size_request(label_separate2, 85, 15);
    sprintf(sepa2_buf, "<span foreground='black' font_desc='10'>不分段</span>");  
    
    //创建row，将标签和图片插入row中
    row_image_head = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_image_head), image);
    row_label_separate1 = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_label_separate1), label_separate1);
    row_label_separate2 = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_label_separate2), label_separate2);
    
    //将row插入到listbox中
    gtk_list_box_prepend(GTK_LIST_BOX(listbox), row_image_head);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_label_separate1, 2);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_label_separate2, 3);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_spare, 4);

    switch (value)
    {
    case SELECT_NONE:
        gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_spare));
        break;
    case SELECT_HEAD:
        image = gtk_image_new_from_file(get_work_mode_image_sel_url(WORKMODE_MP4, SELECT_YES));
        gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_image_head));
        break;
    case SELECT_FIRST:
        sprintf(sepa1_buf, "<span foreground='white' font_desc='10'>60min</span>");  
        gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_label_separate1));
        break;
    case SELECT_SECOND:
        sprintf(sepa2_buf, "<span foreground='white' font_desc='10'>不分段</span>");  
        gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_label_separate2));
        break;
    default:
        break;
    }
    gtk_label_set_markup(GTK_LABEL(label_separate1), sepa1_buf);
    gtk_label_set_markup(GTK_LABEL(label_separate2), sepa2_buf);

    gtk_widget_show_all(window);
    return FALSE;
}