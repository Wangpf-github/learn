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

gint show_jpeg_expo_setting()    //四个选项中能显示前三个后者后一个
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

gint show_jpeg_mode_setting()    //拍照模式，单拍或者连拍
{
    gchar buf[100] = {0};
    GtkWidget *window;
    GtkWidget *image;
    GtkWidget *label_std;
    GtkWidget *label_con;
    GtkWidget *listbox;
    GtkWIdget *row_image_head;
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

    //收到模式切换信号，调用本函数
    g_signal_connect(G_OBJECT(window), "收到切换选项信号", G_CALLBACK(show_jpeg_mode_setting), NULL);
    //收到模式确认信号，退回到状态显示页面
    g_signal_connect(G_OBJECT(window), "收到模式确认信号", G_CALLBACK(show_jpeg_state), NULL);
    //收到主模式切换信号后，切换其他模式，录像，直播，系统设置
    g_signal_connect(G_OBJECT(window), "收到切换为录像模式信号", G_CALLBACK(show_mp4), NULL);

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
    GtkWIdget *row_image_head;
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

    //收到模式切换信号，调用本函数
    g_signal_connect(G_OBJECT(window), "收到切换选项信号", G_CALLBACK(show_jpeg_raw_setting), NULL);
    //收到模式确认信号，退回到状态显示页面
    g_signal_connect(G_OBJECT(window), "收到模式确认信号", G_CALLBACK(show_jpeg_state), NULL);
    //收到主模式切换信号后，切换其他模式，录像，直播，系统设置
    g_signal_connect(G_OBJECT(window), "收到切换为录像模式信号", G_CALLBACK(show_mp4), NULL);
    
    gtk_widget_show_all(window);
    return FALSE;
    //设置完成跳转到show_jpeg_state（）界面
}

gint show_jpeg_setting()  //包含拍照模式，raw，曝光，采用listbox
{
    gchr buf[100] = {0};
    GtkWidget *window;
    GtkWidget *image;
    GtkWidget *label_mode;
    GtkWidget *label_raw;
    GtkWidget *label_expo;
    GtkWidget *listbox;
    GtkWIdget *row_image_head;
    GtkWIdget *row_label_mode;
    GtkWIdget *row_label_raw;
    GtkWIdget *row_label_expo;
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

    //收到切换信号时跳转本函数，更改黑白设置show_jpeg_setting()
    g_signal_connect(G_OBJECT(window), "收到切换选项信号", G_CALLBACK(show_jpeg_setting), NULL);
    //收到拍照模式选择信号，跳转到模式设置show_jpeg_mode_setting()
    g_signal_connect(G_OBJECT(window), "收到拍照模式设置信号", G_CALLBACK(show_jpeg_mode_setting), NULL);
    //收到raw设置，跳转到show_jpeg_raw_setting()
    g_signal_connect(G_OBJECT(window), "收到raw模式设置信号", G_CALLBACK(show_jpeg_raw_setting), NULL);
    //收到曝光设置跳转到曝光设置1，show_jpeg_expo_setting1
    g_signal_connect(G_OBJECT(window), "收到曝光模式设置信号", G_CALLBACK(show_jpeg_expo_setting), NULL);
    //收到主模式切换信号后，切换其他模式，录像，直播，系统设置
    g_signal_connect(G_OBJECT(window), "收到切换为录像模式信号", G_CALLBACK(show_mp4), NULL);

    gtk_widget_show_all(window);
    return FALSE;
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
    g_signal_connect(G_OBJECT(window), "notify::window", G_CALLBACK(show_jpeg_setting), NULL);
    g_signal_connect(G_OBJECT(window), "本页面信息变更信号", G_CALLBACK(show_jpeg_state), NULL);
    gtk_widget_show_all(window);

    gtk_widget_show_all(window);
    return FALSE;
}

/* 拍照模式中收到跳转信号可切换到录视频模式
 * 1秒之内收到设置信号，进入设置界面
 * 等待1秒自己发信号进入状态显示界面 
 */

gint show_jpeg(GtkWidget *widget, gpointer data)
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
    g_signal_connect(G_OBJECT(window), "进入状态显示页面", G_CALLBACK(show_jpeg_state), NULL);
    //收到模式切换信号，切换到mp4模式
    g_signal_connect(G_OBJECT(window), "切换模式信号", G_CALLBACK(show_mp4)), NULL);

    gtk_widget_show_all(window);
    return 0;
}