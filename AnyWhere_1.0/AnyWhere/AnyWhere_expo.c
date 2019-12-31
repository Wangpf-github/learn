#include "AnyWhere_GTK.h"

gint show_iso_setting(gpointer mode)
{
    gchar iso_mode[6][6] = {0};
    gint Expo_mode;
    gchr buf[100] = {0};
    gint i;
    GtkWidget *window;
    GtkWidget *listbox;
    GtkWidget *image;
    GtkWidget *label[6];
    GtkWidget *row_image_head;
    GtkWidget *row_label_1;
    GtkWidget *row_label_2;
    GtkWidget *row_label_3;

    Expo_mode = atoi(mode);
    if(Expo_mode == 1 || Expo_mode == 3)
    {
        shutter_mode = expo_shutter_range;
    }
    else
    {
        shutter_mode = expo_shutter_fix;
    }
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
    //创建4个row

    row_image_head = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_image_head), image);
    row_label_1 = gtk_list_box_row_new();
    row_label_2 = gtk_list_box_row_new();
    row_label_3 = gtk_list_box_row_new();
    gtk_list_box_prepend(GTK_LIST_BOX(listbox), row_label_1);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_label_2, 2);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_label_3, 3);

    for (size_t i = 0; i < 6; i++)
    {
        label[i] = gtk_label_new("shutter");
        gtk_widget_set_size_request(label[i], 85, 15);
        sprintf(buf, "<span foreground='black' font_desc='10'>Shutter:%s</span>", shutter_mode[i]);
        gtk_label_set_markup(GTK_LABEL(label[i]), buf);
    }
    // 根据当前模式选择需要显示的页面， 2选1
    if(   )
    {
        gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_image_head));
        gtk_container_add(GTK_CONTAINER(row_label_1), label[0]);
        gtk_container_add(GTK_CONTAINER(row_label_2), label[1]);
        gtk_container_add(GTK_CONTAINER(row_label_3), label[2]);
    }
    else
    {
        gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_image_head));
        gtk_container_add(GTK_CONTAINER(row_label_1), label[3]);
        gtk_container_add(GTK_CONTAINER(row_label_2), label[4]);
        gtk_container_add(GTK_CONTAINER(row_label_3), label[5]);
    }
    //收到模式切换信号，调用本函数
    g_signal_connect(G_OBJECT(window), "收到模式切换信号", G_CALLBACK(show_iso_setting), NULL);
    //收到模式确认信号，退回到状态显示页面
    g_signal_connect(G_OBJECT(window), "收到模式确认信号", G_CALLBACK(show_jpeg_state), NULL);
    //收到主模式切换信号后，切换其他模式，录像，直播，系统设置
    g_signal_connect(G_OBJECT(window), "收到切换为录像模式信号", G_CALLBACK(show_mp4), NULL);

    gtk_widget_show(window);
    return FALSE;
}

gint show_shutter_setting(gpointer mode)
{
    gchar shutter_mode[8][8] = {0};
    gint Expo_mode;
    gchr buf[100] = {0};
    gint i;
    GtkWidget *window;
    GtkWidget *listbox;
    GtkWidget *image;
    GtkWidget *label[8];
    GtkWidget *row_image_head;
    GtkWidget *row_label_1;
    GtkWidget *row_label_2;
    GtkWidget *row_label_3;

    Expo_mode = atoi(mode);
    if(Expo_mode == 1 || Expo_mode == 4)
    {
        shutter_mode = expo_shutter_range;
    }
    else
    {
        shutter_mode = expo_shutter_fix;
    }
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
    //创建4个row
    row_image_head = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_image_head), image);
    row_label_1 = gtk_list_box_row_new();
    row_label_2 = gtk_list_box_row_new();
    row_label_3 = gtk_list_box_row_new();
    gtk_list_box_prepend(GTK_LIST_BOX(listbox), row_label_1);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_label_2, 2);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_label_3, 3);

    for (size_t i = 0; i < 8; i++)
    {
        label[i] = gtk_label_new("shutter");
        gtk_widget_set_size_request(label[i], 85, 15);
        sprintf(buf, "<span foreground='black' font_desc='10'>Shutter:%s</span>", shutter_mode[i]);
        gtk_label_set_markup(GTK_LABEL(label[i]), buf);
    }
    // 根据当前模式选择需要显示的页面， 3选1
    if(   )
    {
        gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_image_head));
        gtk_container_add(GTK_CONTAINER(row_label_1), label[0]);
        gtk_container_add(GTK_CONTAINER(row_label_2), label[1]);
        gtk_container_add(GTK_CONTAINER(row_label_3), label[2]);
    }
    else if (/* condition */)
    {
        gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_image_head));
        gtk_container_add(GTK_CONTAINER(row_label_1), label[3]);
        gtk_container_add(GTK_CONTAINER(row_label_2), label[4]);
        gtk_container_add(GTK_CONTAINER(row_label_3), label[5]);
    }
    else
    {
        gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_image_head));
        gtk_container_add(GTK_CONTAINER(row_label_1), label[6]);
        gtk_container_add(GTK_CONTAINER(row_label_2), label[7]);
    }
    //收到模式切换信号，调用本函数
    g_signal_connect(G_OBJECT(window), "收到模式切换信号", G_CALLBACK(show_shutter_setting), NULL);
    //收到模式确认信号，退回到状态显示页面
    g_signal_connect(G_OBJECT(window), "收到模式确认信号", G_CALLBACK(show_jpeg_state), NULL);
    //收到主模式切换信号后，切换其他模式，录像，直播，系统设置
    g_signal_connect(G_OBJECT(window), "收到切换为录像模式信号", G_CALLBACK(show_mp4), NULL);

    gtk_widget_show(window);
    return FALSE;
}

gint show_jpeg_expo_setting_auto()
{
    gchr buf[100] = {0};
    GtkWidget *window;
    GtkWidget *listbox;
    GtkWidget *label_iso;
    GtkWidget *label_shutter;
    GtkWidget *image;
    GtkWidget *row_image_head;
    GtkWidget *row_label_iso;
    GtkWidget *row_label_shutter;
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
    //创建2个label
    label_iso = gtk_label_new("Expo:ISO");
    gtk_widget_set_size_request(label_iso, 85, 15);
    sprintf(buf, "<span foreground='black' font_desc='10'>ISO:%s</span>", expo_iso_range[0]);
    gtk_label_set_markup(GTK_LABEL(label_iso), buf);
    label_shutter = gtk_label_new("Expo:Shutter");
    gtk_widget_set_size_request(label_shutter, 85, 15);
    sprintf(buf, "<span foreground='black' font_desc='10'>Shutter:%s</span>", expo_shutter_range[0]);
    gtk_label_set_markup(GTK_LABEL(label_shutter), buf);
    //创建3个row
    row_image_head = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_image_head), image);
    row_label_iso = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_label_iso), label_iso);
    row_label_shutter = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_label_shutter), label_shutter);
    
    gtk_list_box_prepend(GTK_LIST_BOX(listbox), row_image_head);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_label_iso, 2);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_label_shutter, 3);
    gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_image_head));
    //收到iso设置进入iso设置界面
    g_signal_connect(G_OBJECT(window), "收到iso设置信号", G_CALLBACK(show_iso_setting), "1");
    //收到快门设置进入快门设置界面
    g_signal_connect(G_OBJECT(window), "收到快门设置信号", G_CALLBACK(show_shutter_setting), "1");
    //收到主模式切换信号后，切换其他模式，录像，直播，系统设置
    g_signal_connect(G_OBJECT(window), "收到切换为录像模式信号", G_CALLBACK(show_mp4), NULL);
    gtk_widget_show_all(window)

    return FALSE;
}

gint show_jpeg_expo_setting_manual()
{
    gchr buf[100] = {0};
    GtkWidget *window;
    GtkWidget *listbox;
    GtkWidget *label_iso;
    GtkWidget *label_shutter;
    GtkWidget *image;
    GtkWidget *row_image_head;
    GtkWidget *row_label_iso;
    GtkWidget *row_label_shutter;
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
    //创建2个label
    label_iso = gtk_label_new("Expo:ISO");
    gtk_widget_set_size_request(label_iso, 85, 15);
    sprintf(buf, "<span foreground='black' font_desc='10'>ISO:%s</span>", expo_iso_fix[0]);
    gtk_label_set_markup(GTK_LABEL(label_iso), buf);
    label_shutter = gtk_label_new("Expo:Shutter");
    gtk_widget_set_size_request(label_shutter, 85, 15);
    sprintf(buf, "<span foreground='black' font_desc='10'>Shutter:%s</span>", expo_shutter_fix[0]);
    gtk_label_set_markup(GTK_LABEL(label_shutter), buf);
    //创建3个row
    row_image_head = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_image_head), image);
    row_label_iso = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_label_iso), label_iso);
    row_label_shutter = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_label_shutter), label_shutter);
    
    gtk_list_box_prepend(GTK_LIST_BOX(listbox), row_image_head);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_label_iso, 2);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_label_shutter, 3);
    gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_image_head));
    //收到iso设置进入iso设置界面
    g_signal_connect(G_OBJECT(window), "收到iso设置信号", G_CALLBACK(show_iso_setting), "2");
    //收到快门设置进入快门设置界面
    g_signal_connect(G_OBJECT(window), "收到快门设置信号", G_CALLBACK(show_shutter_setting), "2");
    //收到主模式切换信号后，切换其他模式，录像，直播，系统设置
    g_signal_connect(G_OBJECT(window), "收到切换为录像模式信号", G_CALLBACK(show_mp4), NULL);
    gtk_widget_show_all(window)

    return FALSE;
}

gint show_jpeg_expo_setting_shutter()
{
    gchr buf[100] = {0};
    GtkWidget *window;
    GtkWidget *listbox;
    GtkWidget *label_iso;
    GtkWidget *label_shutter;
    GtkWidget *image;
    GtkWidget *row_image_head;
    GtkWidget *row_label_iso;
    GtkWidget *row_label_shutter;
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
    //创建2个label
    label_iso = gtk_label_new("Expo:ISO");
    gtk_widget_set_size_request(label_iso, 85, 15);
    sprintf(buf, "<span foreground='black' font_desc='10'>ISO:%s</span>", expo_iso_range[0]);
    gtk_label_set_markup(GTK_LABEL(label_iso), buf);
    label_shutter = gtk_label_new("Expo:Shutter");
    gtk_widget_set_size_request(label_shutter, 85, 15);
    sprintf(buf, "<span foreground='black' font_desc='10'>Shutter:%s</span>", expo_shutter_fix[0]);
    gtk_label_set_markup(GTK_LABEL(label_shutter), buf);
    //创建3个row
    row_image_head = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_image_head), image);
    row_label_iso = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_label_iso), label_iso);
    row_label_shutter = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_label_shutter), label_shutter);
    
    gtk_list_box_prepend(GTK_LIST_BOX(listbox), row_image_head);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_label_iso, 2);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_label_shutter, 3);
    gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_image_head));
    //收到iso设置进入iso设置界面
    g_signal_connect(G_OBJECT(window), "收到iso设置信号", G_CALLBACK(show_iso_setting), "3");
    //收到快门设置进入快门设置界面
    g_signal_connect(G_OBJECT(window), "收到快门设置信号", G_CALLBACK(show_shutter_setting), "3");
    //收到主模式切换信号后，切换其他模式，录像，直播，系统设置
    g_signal_connect(G_OBJECT(window), "收到切换为录像模式信号", G_CALLBACK(show_mp4), NULL);
    gtk_widget_show_all(window)

    return FALSE;
}

gint show_jpeg_expo_setting_iso()
{
    gchr buf[100] = {0};
    GtkWidget *window;
    GtkWidget *listbox;
    GtkWidget *label_iso;
    GtkWidget *label_shutter;
    GtkWidget *image;
    GtkWidget *row_image_head;
    GtkWidget *row_label_iso;
    GtkWidget *row_label_shutter;
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
    //创建2个label
    label_iso = gtk_label_new("Expo:ISO");
    gtk_widget_set_size_request(label_iso, 85, 15);
    sprintf(buf, "<span foreground='black' font_desc='10'>ISO:%s</span>", expo_iso_fix[0]);
    gtk_label_set_markup(GTK_LABEL(label_iso), buf);
    label_shutter = gtk_label_new("Expo:Shutter");
    gtk_widget_set_size_request(label_shutter, 85, 15);
    sprintf(buf, "<span foreground='black' font_desc='10'>Shutter:%s</span>", expo_shutter_range[0]);
    gtk_label_set_markup(GTK_LABEL(label_shutter), buf);
    //创建3个row
    row_image_head = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_image_head), image);
    row_label_iso = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_label_iso), label_iso);
    row_label_shutter = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_label_shutter), label_shutter);
    
    gtk_list_box_prepend(GTK_LIST_BOX(listbox), row_image_head);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_label_iso, 2);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_label_shutter, 3);
    gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_image_head));
    //收到iso设置进入iso设置界面
    g_signal_connect(G_OBJECT(window), "收到iso设置信号", G_CALLBACK(show_iso_setting), "4");
    //收到快门设置进入快门设置界面
    g_signal_connect(G_OBJECT(window), "收到快门设置信号", G_CALLBACK(show_shutter_setting), "4");
    //收到主模式切换信号后，切换其他模式，录像，直播，系统设置
    g_signal_connect(G_OBJECT(window), "收到切换为录像模式信号", G_CALLBACK(show_mp4), NULL);
    gtk_widget_show_all(window)

    return FALSE;
}

gint show_expo_setting1()    //四个选项中能显示前三个后者后一个
{
    gchr buf[100] = {0};
    GtkWidget *window;
    GtkWidget *listbox;
    GtkWidget *label_auto;
    GtkWidget *label_manual;
    GtkWidget *label_shutter;
    GtkWidget *label_iso;
    GtkWidget *image;
    GtkWidget *row_image_head;
    GtkWidget *row_label_auto;
    GtkWidget *row_label_manual;
    GtkWidget *row_label_shutter;
    GtkWidget *row_label_iso;
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
    //创建4种曝光模式的标签
    label_auto = gtk_label_new("Expo:Auto");
    gtk_widget_set_size_request(label_auto, 85, 15);
    sprintf(buf, "<span foreground='black' font_desc='10'>Expo:Auto</span>");
    gtk_label_set_markup(GTK_LABEL(label_auto), buf);
    label_manual = gtk_label_new("Expo:Manual");
    gtk_widget_set_size_request(label_manual, 85, 15);
    sprintf(buf, "<span foreground='black' font_desc='10'>Expo:Manual</span>");
    gtk_label_set_markup(GTK_LABEL(label_manual), buf);
    label_shutter = gtk_label_new("Expo:Shutter");
    gtk_widget_set_size_request(label_shutter, 85, 15);
    sprintf(buf, "<span foreground='black' font_desc='10'>Expo:Shutter</span>");
    gtk_label_set_markup(GTK_LABEL(label_shutter), buf);
    label_iso = gtk_label_new("Expo:ISO");
    gtk_widget_set_size_request(label_iso, 85, 15);
    sprintf(buf, "<span foreground='black' font_desc='10'>Expo:ISO</span>");
    gtk_label_set_markup(GTK_LABEL(label_iso), buf);
    //创建5个row
    row_image_head = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_image_head), image);
    row_label_auto = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_label_auto), label_auto);
    row_label_manual = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_label_manual), label_manual);
    row_label_shutter = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_label_shutter), label_shutter);
    row_label_iso = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row_label_iso), label_iso);

    gtk_list_box_prepend(GTK_LIST_BOX(listbox), row_image_head);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_label_auto, 2);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_label_manual, 3);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_label_shutter, 4);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row_label_iso, 5);

    gtk_widget_show(window);
    gtk_widget_show(listbox);
    gtk_widget_show(row_image_head);
    gtk_widget_show(image);
    if()     //用于判断显示第一页还是第二页
    {
        gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_image_head));
        gtk_widget_show(row_label_auto);
        gtk_widget_show(label_auto);
        gtk_widget_show(row_label_manual);
        gtk_widget_show(label_manual);
        gtk_widget_show(row_label_shutter);
        gtk_widget_show(label_shutter);
    }
    else
    {
        gtk_list_box_select_row(GTK_LIST_BOX(listbox), GTK_LIST_BOX_ROW(row_image_head));
        gtk_widget_show(row_label_iso);
        gtk_widget_show(label_iso);
    }
    
    //收到模式切换跳转本函数，更改黑白设置show_jpeg_expo_setting() 
    g_signal_connect(G_OBJECT(window), "收到切换选项信号", G_CALLBACK(show_jpeg_expo_setting), NULL);
    //收到模式选择信号时，进入设置界面show_jpeg_expo_setting_auto/manual/shutter()
    g_signal_connect(G_OBJECT(window), "收到切换选项信号", G_CALLBACK(show_jpeg_expo_setting_auto), NULL);
    g_signal_connect(G_OBJECT(window), "收到切换选项信号", G_CALLBACK(show_jpeg_expo_setting_manual), NULL);
    g_signal_connect(G_OBJECT(window), "收到切换选项信号", G_CALLBACK(show_jpeg_expo_setting_shutter), NULL);
    g_signal_connect(G_OBJECT(window), "收到切换选项信号", G_CALLBACK(show_jpeg_expo_setting_iso), NU信号LL);
    //收到主模式切换信号后，切换其他模式，录像，直播，系统设置
    g_signal_connect(G_OBJECT(window), "收到切换为录像模式信号", G_CALLBACK(show_mp4), NULL);

    return FALSE;
}
