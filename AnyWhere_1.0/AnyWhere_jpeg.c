#include "AnyWhere_GTK.h"

gint show_jpeg_setting()  //包含拍照模式，连拍间隔，采用listbox
{
    guint select;
    GList *list = NULL;
    gchar *interval, *item1, *item2, *item3;
    gchar text_buf1[20] = {0};
    gchar text_buf2[20] = {0};
    gchar text_buf3[20] = {0};
    g_object_get(G_OBJECT (entry), "jpeg_list", &list, "interface_select", &select, "jpeg_interval", &interval, NULL);
    item1 = (gchar *)g_list_nth_data(list, 0);
    item2 = (gchar *)g_list_nth_data(list, 1);
    item3 = (gchar *)g_list_nth_data(list, 2);
    cairo_t *cr1 = cairo_create(surface);     //工作模式，图片
    cairo_t *cr2 = cairo_create(surface);     //三角箭头，图片
    cairo_t *cr3 = cairo_create(surface);     //第一行，字符串
    cairo_t *cr4 = cairo_create(surface);     //第二行，字符串
    cairo_t *cr5 = cairo_create(surface);     //第三行，字符串
    cairo_t *cr6 = cairo_create(surface);     //占位，矩形
    cairo_t *cr7 = cairo_create(surface);     //占位，矩形
    cairo_surface_t *image1 = cairo_image_surface_create_from_png (get_work_mode_image_small_url(WORKMODE_JPEG, SELECT_NO));
	cairo_surface_t *image2 = cairo_image_surface_create_from_png (get_angle_url(SELECT_NO));
    cairo_set_source_surface (cr1, image1, 47, 6);
    cairo_set_source_surface (cr2, image2, 73, 7);

    cairo_select_font_face(cr3, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr3, 12.0);
    cairo_set_source_rgb(cr3, 1.0, 1.0, 1.0); 
    cairo_move_to(cr3, 8.0, 36.0); 
    sprintf(text_buf1, item1);
    
    cairo_select_font_face(cr4, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr4, 12.0);
    cairo_set_source_rgb(cr4, 1.0, 1.0, 1.0); 
    cairo_move_to(cr4, 8.0, 48.0);
    sprintf(text_buf2, item2);
    
    cairo_select_font_face(cr4, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr4, 12.0);
    cairo_set_source_rgb(cr4, 1.0, 1.0, 1.0); 
    cairo_move_to(cr4, 8.0, 60.0);
    sprintf(text_buf3, item3);

    switch (select)
    {
    case -1:
        cairo_surface_t *image1 = cairo_image_surface_create_from_png (get_work_mode_image_small_url(WORKMODE_JPEG, SELECT_YES));
	    cairo_surface_t *image2 = cairo_image_surface_create_from_png (get_angle_url(SELECT_YES));
        cairo_set_source_surface (cr1, image1, 47, 0);
        cairo_set_source_surface (cr2, image2, 67, 0);
        cairo_rectangle (cr6, 0, 0, 47, 28);
        cairo_fill(cr6);
        cairo_stroke(cr6);
        cairo_rectangle (cr7, 75, 0, 55, 28);
        cairo_fill(cr7);
        cairo_stroke(cr7);
        break;
    case SELECT_FIRST:
        cairo_select_font_face(cr3, "AnyWhere", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr3, 20.0);
        cairo_set_source_rgb(cr3, 1.0, 1.0, 1.0); 
        cairo_move_to(cr3, 0.0, 36.0); 
        sprintf(text_buf1, " %s           ", item1);
        break;
    case SELECT_SECOND:
        cairo_select_font_face(cr3, "AnyWhere", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr3, 20.0);
        cairo_set_source_rgb(cr3, 1.0, 1.0, 1.0); 
        cairo_move_to(cr3, 0.0, 48.0); 
        sprintf(text_buf1, " %s           ", item2);
        break;
    case SELECT_THIRD:
        cairo_select_font_face(cr3, "AnyWhere", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr3, 20.0);
        cairo_set_source_rgb(cr3, 1.0, 1.0, 1.0); 
        cairo_move_to(cr3, 0.0, 60.0); 
        sprintf(text_buf1, " %s           ", item3);

    default:
        break;
    }
    
    cairo_paint(cr1);
    cairo_paint(cr2);
    cairo_show_text(cr3, text_buf1);
    cairo_show_text(cr4, text_buf2);
    cairo_show_text(cr4, text_buf3);

    cairo_destroy(cr1);
	cairo_destroy(cr2);
	cairo_destroy(cr3);
	cairo_destroy(cr4);
    cairo_destroy(cr5);

    return TRUE;
}

gint show_jpeg_state()
{
    gchar *ip_addr, *disk_space, *shoot;
    gchar urlbuf_workmode[50] = {0};
    g_object_get(G_OBJECT (entry), "IPaddr", &ip_addr, "jpeg_disk_space_3", &disk_space, "jpeg_shoot_mode_3", &shoot, NULL);
    cairo_t *cr1 = cairo_create(surface);     //网络工作模式，图片
    cairo_t *cr2 = cairo_create(surface);     //电量，图片
    cairo_t *cr3 = cairo_create(surface);     //工作模式，图片
    cairo_t *cr4 = cairo_create(surface);     //IP时地址，字符串
    cairo_t *cr5 = cairo_create(surface);     //磁盘剩余空间，字符串
    cairo_t *cr6 = cairo_create(surface);     //分辨率，字符串
    cairo_t *cr7 = cairo_create(surface);     //拍照模式，字符串
    cairo_t *cr8 = cairo_create(surface);     //照片存储格式，字符串
    cairo_surface_t *image1 = cairo_image_surface_create_from_png (get_network_image_url());
	cairo_surface_t *image2 = cairo_image_surface_create_from_png (get_battery_image_url());
    sprintf(urlbuf_workmode, "%s/anywhere_picture/fittings/jpeg_s.png", image_route);
	cairo_surface_t *image3 = cairo_image_surface_create_from_png (urlbuf_workmode);
    cairo_set_source_surface (cr1, image1, 1, 4);
    cairo_set_source_surface (cr2, image2, 108, 4);
    cairo_set_source_surface (cr3, image3, 48, 23);

    cairo_paint(cr1);
    cairo_select_font_face(cr4, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr4, 12.0);
    cairo_set_source_rgb(cr4, 0.0, 0.0, 0.0); 
    cairo_move_to(cr4, 16.0, 13.0);     //IP位置
    cairo_show_text(cr4, ip_addr);
    cairo_paint(cr2);
    cairo_paint(cr3);
    cairo_select_font_face(cr5, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr5, 24.0);
    cairo_set_source_rgb(cr5, 1.0, 1.0, 1.0); 
    cairo_move_to(cr5, 42.0, 40.0);     //剩余空间位置
    cairo_show_text(cr5, disk_space);
    cairo_select_font_face(cr6, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr6, 14.0);
    cairo_set_source_rgb(cr6, 1.0, 1.0, 1.0); 
    cairo_move_to(cr6, 1.0, 60.0);     //分辨率位置
    cairo_show_text(cr6, "8K");
    cairo_select_font_face(cr7, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr7, 14.0);
    cairo_set_source_rgb(cr7, 1.0, 1.0, 1.0); 
    cairo_move_to(cr7, 25.0, 60.0);     //拍照模式位置
    cairo_show_text(cr7, shoot);
    cairo_select_font_face(cr8, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr8, 14.0);
    cairo_set_source_rgb(cr8, 1.0, 1.0, 1.0); 
    cairo_move_to(cr8, 96.0, 60.0);     //存储格式位置
    cairo_show_text(cr8, "JPEG");

    cairo_destroy(cr1);
	cairo_destroy(cr2);
	cairo_destroy(cr3);
	cairo_destroy(cr4);
    cairo_destroy(cr5);
	cairo_destroy(cr6);
	cairo_destroy(cr7);
	cairo_destroy(cr8);

    return TRUE;
}

gint show_jpeg()
{
    gchar *ip_addr;
    g_object_get(G_OBJECT (entry), "IPaddr", &ip_addr, NULL);
    gchar urlbuf_workmode[50] = {0};
    cairo_t *cr1 = cairo_create(surface);     //网络工作模式，图片
    cairo_t *cr2 = cairo_create(surface);     //电量，图片
    cairo_t *cr3 = cairo_create(surface);     //工作模式，图片
    cairo_t *cr4 = cairo_create(surface);     //IP时地址，字符串
    cairo_surface_t *image1 = cairo_image_surface_create_from_png (get_network_image_url());
	cairo_surface_t *image2 = cairo_image_surface_create_from_png (get_battery_image_url());
    sprintf(urlbuf_workmode, "%s/anywhere_picture/fittings/jpeg_l.png", image_route);
	cairo_surface_t *image3 = cairo_image_surface_create_from_png (urlbuf_workmode);
    cairo_set_source_surface (cr1, image1, 1, 4);
    cairo_set_source_surface (cr2, image2, 108, 4);
    cairo_set_source_surface (cr3, image3, 4, 24);

    cairo_paint(cr1);
    cairo_select_font_face(cr4, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr4, 12.0);
    cairo_set_source_rgb(cr4, 0.0, 0.0, 0.0); 
    cairo_move_to(cr4, 16.0, 13.0);     //座标，以左下角为基准
    cairo_show_text(cr4, ip_addr);
    cairo_paint(cr2);
    cairo_paint(cr3);

    cairo_destroy(cr1);
	cairo_destroy(cr2);
	cairo_destroy(cr3);
	cairo_destroy(cr4);

    return TRUE;
}