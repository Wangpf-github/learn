#include "AnyWhere_GTK.h"

gint show_jpeg_shoot_setting()    //拍照模式，单拍或者连拍
{
    gchar buf[100] = {0};
    GtkWidget *window;
    GtkWidget *image;
    GtkWidget *label_std;
    GtkWidget *label_con;
    GtkWidget *listbox;
    GtkWidget *row_image_head;
    GtkWidget *row_label_std;
    GtkWidget *row_label_con;
    //创建底图
    window = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_window_set_title(GTK_WINDOW(window), "AnyWhere");
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_window_set_default_size(GTK_WINDOW(window), 128, 64);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    //创建一个listbox
    listbox = gtk_list_box_new();
    gtk_container_add(GTK_CONTAINER(window), listbox);
    //插入表头图片
    image = gtk_image_new_from_file("jpeg模式图片");
    //创建单拍和连拍模式的标签
    label_std = gtk_label_new("Mode:Standard");
    gtk_widget_set_size_request(label_std, 85, 15);
    sprintf(buf, "<span foreground='black' font_desc='10'>Mode:Stanard</span>");
    gtk_label_set_markup(GTK_LABEL(label_std), buf);
    label_con = gtk_label_new("Mode:Continuous");
    gtk_widget_set_size_request(label_con, 85, 15);
    sprintf(buf, "<span foreground='black' font_desc='10'>Mode:Continuous</span>");
    gtk_label_set_markup(GTK_LABEL(label_con), buf);
    //创建3行row，将标签和图片插入row中
    row_image_head = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_image_head), image);
    row_label_std = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_label_std), label_std);
    row_label_con = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_label_con), label_con);

    //将3行row插入到listbox中
    gtk_list_box_prepend(GTK_LIST_BOX(listbox), row_image_head);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_label_std, 2);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_label_con, 3);

    gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_image_head));

    //收到页面切换信号，进入页面选择页面
    g_signal_connect(entry, "notify::interface", G_CALLBACK(judge_show_jpeg_shoot_setting), NULL);
    //收到选择项变更信号
    g_signal_connect(entry, "notify::interface_select", G_CALLBACK(show_jpeg_shoot_setting), NULL);
    
    gtk_widget_show_all(window);
    return FALSE;
}

gint show_jpeg_raw_setting()    //拍照模式，单拍或者连拍
{
    gchar buf[100] = {0};
    GtkWidget *window;
    GtkWidget *image;
    GtkWidget *label_on;
    GtkWidget *label_off;
    GtkWidget *listbox;
    GtkWidget *row_image_head;
    GtkWidget *row_label_on;
    GtkWidget *row_label_off;
    //创建底图
    window = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_window_set_title(GTK_WINDOW(window), "AnyWhere");
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_window_set_default_size(GTK_WINDOW(window), 128, 64);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    //创建一个listbox
    listbox = gtk_list_box_new();
    gtk_container_add(GTK_CONTAINER(window), listbox);
    //插入表头图片
    image = gtk_image_new_from_file("jpeg模式图片");
    //创建单拍和连拍模式的标签
    label_on = gtk_label_new("Raw:On");
    gtk_widget_set_size_request(label_on, 85, 15);
    sprintf(buf, "<span foreground='black' font_desc='10'>Raw:On</span>");
    gtk_label_set_markup(GTK_LABEL(label_on), buf);
    label_off = gtk_label_new("Raw:Off");
    gtk_widget_set_size_request(label_off, 85, 15);
    sprintf(buf, "<span foreground='black' font_desc='10'>Raw:Off</span>");
    gtk_label_set_markup(GTK_LABEL(label_off), buf);
    //创建3行row，将标签和图片插入row中
    row_image_head = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_image_head), image);
    row_label_on = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_label_on), label_on);
    row_label_off = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_label_off), label_off);

    //将3行row插入到listbox中
    gtk_list_box_prepend(GTK_LIST_BOX(listbox), row_image_head);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_label_std, 2);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_label_con, 3);

    gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_image_head));

    //收到页面切换信号，进入页面选择页面
    g_signal_connect(entry, "notify::interface", G_CALLBACK(judge_show_jpeg_raw_setting), NULL);
    //收到选择项变更信号
    g_signal_connect(entry, "notify::interface_select", G_CALLBACK(show_jpeg_raw_setting), NULL);
    
    gtk_widget_show_all(window);
    return FALSE;
}

gint show_jpeg_setting1()  //包含拍照模式，raw，曝光，采用listbox
{
    gchr buf[100] = {0};
    GtkWidget *window;
    GtkWidget *image;
    GtkWidget *label_mode;
    GtkWidget *label_raw;
    GtkWidget *label_expo;
    GtkWidget *listbox;
    GtkWidget *row_image_head;
    GtkWidget *row_label_mode;
    GtkWidget *row_label_raw;
    GtkWidget *row_label_expo;
    //创建底图
    window = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_window_set_title(GTK_WINDOW(window), "AnyWhere");
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_window_set_default_size(GTK_WINDOW(window), 128, 64);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    //创建一个listbox
    listbox = gtk_list_box_new();
    gtk_container_add(GTK_CONTAINER(window), listbox);
    //插入表头图片
    image = gtk_image_new_from_file("jpeg模式图片");
    //创建三个标签
    label_mode = gtk_label_new("mode setting");
    gtk_widget_set_size_request(label_mode, 85, 15);
    sprintf(buf, "<span foreground='black' font_desc='10'>Mode:Stanard</span>");  //修改label的字体大小，颜色等，可加下划线等
    gtk_label_set_markup(GTK_LABEL(label_mode), buf);
    label_raw = gtk_label_new("Raw:On");
    gtk_widget_set_size_request(label_raw, 85, 15);
    sprintf(buf, "<span foreground='black' font_desc='10'>Raw:On</span>");  //修改label的字体大小，颜色等，可加下划线等
    gtk_label_set_markup(GTK_LABEL(label_raw), buf);
    label_expo = gtk_label_new("Expo:Auto");
    gtk_widget_set_size_request(label_expo, 85, 15);
    sprintf(buf, "<span foreground='black' font_desc='10'>Expo:Auto</span>");  //修改label的字体大小，颜色等，可加下划线等
    gtk_label_set_markup(GTK_LABEL(label_expo), buf);  

    //创建4行row，将标签和图片插入row中
    row_image_head = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_image_head), image);
    row_label_mode = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_label_mode), label_mode);
    row_label_raw = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_label_raw), label_raw);
    row_label_expo = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_label_expo), label_expo);
    
    //将4行row插入到listbox中
    gtk_list_box_prepend(GTK_LIST_BOX(listbox), row_image_head);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_label_mode, 2);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_label_raw, 3);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_label_expo, 4);

    gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_image_head));    //用于选择切换选项

    //信号链接函数,界面变更信号
    g_signal_connect(entry, "notify::interface", G_CALLBACK(judge_show_jpeg_setting1), NULL);
    //选中行变更信号,调用本函数
    g_signal_connect(entry, "notify::interface_select",G_CALLBACK(show_jpeg_setting1), NULL);

    gtk_widget_show_all(window);
    return FALSE;
}

gint show_jpeg_setting2()
{
    //信号链接函数,界面变更信号
    g_signal_connect(entry, "notify::interface", G_CALLBACK(judge_show_jpeg_setting2), NULL);
    //选中行变更信号,调用本函数
    g_signal_connect(entry, "notify::interface_select",G_CALLBACK(show_jpeg_setting2), NULL);
}

gint show_jpeg_state()
{
    GtkWidget *window;
    GtkWidget *hbox1;
    GtkWidget *hbox2;
    GtkWidget *vbox;
    GtkWidget *label_state;
    GtkWidget *label_IP;
    GtkWidget *label_number;
    GtkWidget *image_network;
    GtkWidget *image_battery;
    GtkWidget *image_camera;
    gchar buf_state[100] = {0};
    gchar buf_ipaddr[20] = {0};
    gchar buf_picnum[20] = {0};
    window = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_window_set_title(GTK_WINDOW(window), "AnyWhere");
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_window_set_default_size(GTK_WINDOW(window), 128, 64);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    //第一行显示信息
    hbox1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
    gtk_box_pack_start(GTK_BOX(vbox), hbox1, FALSE, FALSE, 0);
    
    image_network = gtk_image_new_from_file("网络工作方式图片加载路径");
    gtk_box_pack_start(GTK_BOX(hbox1), image_network, FALSE, FALSE, 0);
    label_IP = gtk_label_new("IPaddr");
    gtk_widget_set_size_request(label_IP, 84， 20);
    sprintf(buf_ipaddr, "<span foreground='black' font_desc='8'>192.168.100.100</span>");
    gtk_label_set_markup(GTK_LABEL(label_IP), buf_ipaddr);
    gtk_box_pack_start(GTK_BOX(hbox1), label_IP);
    image_battery = gtk_image_new_from_file("电池状态图片加载路径");
    gtk_box_pack_start(GTK_BOX(hbox1), image_battery, FALSE, FALSE, 0);

    //第二行显示信息
    hbox2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
    gtk_box_pack_start(GTK_BOX(vbox), hbox2, FALSE, FALSE, 0);
    image_camera = gtk_image_new_from_file("相机图片加载路径");
    gtk_box_pack_start(GTK_BOX(hbox2), image_camera);
    label_number = gtk_label_new("picture number");
    gtk_widget_set_size_request(label_number, 70， 30);
    sprintf(buf_picnum, "<span foreground='black' font_desc='8'> No Space </span>");
    gtk_label_set_markup(GTK_LABEL(label_number), buf_picnum);
    gtk_box_pack_start(GTK_BOX(hbox2), label_number);

    //创建状态标签,第三行显示信息
    label_state = gtk_label_new("status");
    gtk_widget_set_size_request(label_state, 120, 30);
    sprintf(buf_state, "<span foreground='black' font_desc='10'>8K Standard JPEG</span>");  //修改label的字体大小，颜色等，可加下划线等
    gtk_label_set_markup(GTK_LABEL(label_state), buf_state);
    gtk_box_pack_start(GTK_BOX(vbox), label_state, FALSE, FALSE, 0);

    //收到模式设置信号后进入设置模式show_jpeg_setting()
    g_signal_connect(entry, "notify::interface", G_CALLBACK(judge_show_jpeg_state), NULL);

    gtk_widget_show_all(window);
    return FALSE;
}

/* 拍照模式中收到跳转信号可切换到录视频模式
 * 1秒之内收到设置信号，进入设置界面
 * 等待1秒自己发信号进入状态显示界面 
 */

gint show_jpeg()
{
    gchar IP_buf[100] = {0};
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
    gtk_window_set_title(GTK_WINDOW(window), "JPEG mode");
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_window_set_default_size(GTK_WINDOW(window), 128, 64);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window),vbox);

    //创建hbox并加入vbox
    header_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_box_pack_start(GTK_BOX(vbox),header_box, FALSE, FALSE, 0);

    //创建网口图片窗口并加入hbox
    image_eth = gtk_image_new_from_file("./pictures/ethnet.png");   //加载图片的路径
    gtk_box_pack_start(GTK_BOX(header_box), image_eth, FALSE, FALSE, 0);

    //创建标签并加入hbox
    label_IPaddr = gtk_label_new("IP addr");
    gtk_widget_set_size_request(label_IPaddr, 70, 10);
    sprintf(IP_buf, "<span foreground='black' font_desc='8'>%s</span>", IP_addr);  //修改label的字体大小，颜色等，可加下划线等
    gtk_label_set_markup(GTK_LABEL(label_IPaddr), IP_buf);  
    gtk_widget_set_size_request(label_IPaddr, 84, 20);
    gtk_container_add(GTK_BOX(header_box), label_IPaddr);

    //创建电池图片窗口并加入hbox
    image_battery = gtk_image_new_from_file("./pictures/battery.png");   //加载图片的路径
    gtk_box_pack_start(GTK_BOX(header_box), image_battery, FALSE, FALSE, 0);
    
    //创建拍照模式图片窗口并加入vbox
    image_camera = gtk_image_new_from_file("./pictures/camera.png");   //加载图片的路径
    gtk_box_pack_start(GTK_BOX(vbox), image_camera, FALSE, FALSE, 0);
    
    //显示1秒后进去状态页面
    g_signal_connect(entry, "notify::interface", G_CALLBACK(judge_show_jpeg), NULL);

    gtk_widget_show_all(window);
    return 0;
}