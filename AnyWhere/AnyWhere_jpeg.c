#include "AnyWhere_GTK.h"

gint show_jpeg_setting()  //包含拍照模式，连拍间隔，采用Glist
{
    guint select;
    GList *list = NULL;
    gchar *item1, *item2, *item3;
    g_object_get(G_OBJECT (entry), "jpeg_list", &list, "interface_select", &select, NULL);
    item1 = (gchar *)g_list_nth_data(list, 0);
    item2 = (gchar *)g_list_nth_data(list, 1);
    item3 = (gchar *)g_list_nth_data(list, 2);
    cairo_t *cr1 = cairo_create(surface);     //工作模式，图片
    cairo_t *cr2 = cairo_create(surface);     //三角箭头，图片
    cairo_t *cr3 = cairo_create(surface);     //第一行:拍照模式，字符串
    cairo_t *cr4 = cairo_create(surface);     //第二行:连拍间隔，字符串
    cairo_t *cr5 = cairo_create(surface);     //第三行:done，字符串
    cairo_t *cr6 = cairo_create(surface);     //占位，矩形
    cairo_t *cr7 = cairo_create(surface);     //占位，矩形
    cairo_t *cr8 = cairo_create(surface);
    cairo_set_operator(cr8, CAIRO_OPERATOR_CLEAR);
    cairo_rectangle (cr8, 0, 0, 128, 128);
    cairo_fill(cr8);
    cairo_stroke(cr8);
    cairo_surface_t *image1;
	cairo_surface_t *image2;
    cairo_set_source_surface (cr1, image1, 47, 6);
    cairo_set_source_surface (cr2, image2, 73, 7);

    cairo_select_font_face(cr3, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr3, 12.0);
    cairo_set_source_rgb(cr3, 1.0, 1.0, 1.0); 
    cairo_move_to(cr3, 8.0, 36.0); 
    
    cairo_select_font_face(cr4, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr4, 12.0);
    cairo_set_source_rgb(cr4, 1.0, 1.0, 1.0); 
    cairo_move_to(cr4, 8.0, 48.0);
    
    cairo_select_font_face(cr4, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr4, 12.0);
    cairo_set_source_rgb(cr4, 1.0, 1.0, 1.0); 
    cairo_move_to(cr4, 8.0, 60.0);

    switch (select)
    {
    case -1:
        image1 = cairo_image_surface_create_from_png (get_work_mode_image_small_url(WORKMODE_JPEG, SELECT_YES));
        image2 = cairo_image_surface_create_from_png (get_angle_url(SELECT_YES));
        cairo_set_source_surface (cr1, image1, 47, 0);
        cairo_set_source_surface (cr2, image2, 73, 0);
        cairo_rectangle (cr6, 0, 0, 47, 28);
        cairo_fill(cr6);
        cairo_stroke(cr6);
        cairo_rectangle (cr6, 67, 0, 6, 28);
        cairo_fill(cr6);
        cairo_stroke(cr6);
        cairo_rectangle (cr6, 81, 0, 55, 28);
        cairo_fill(cr6);
        cairo_stroke(cr6);
        break;
    case SELECT_FIRST:
        image1 = cairo_image_surface_create_from_png (get_work_mode_image_small_url(WORKMODE_JPEG, SELECT_NO));
        image2 = cairo_image_surface_create_from_png (get_angle_url(SELECT_NO));
        cairo_set_source_surface (cr1, image1, 47, 6);
        cairo_set_source_surface (cr2, image2, 73, 7);
        cairo_rectangle (cr7, 0, 26, 128, 12);
        cairo_fill(cr7);
        cairo_stroke(cr7);
    	cairo_set_operator(cr3, CAIRO_OPERATOR_CLEAR);
        break;
    case SELECT_SECOND:
        image1 = cairo_image_surface_create_from_png (get_work_mode_image_small_url(WORKMODE_JPEG, SELECT_NO));
        image2 = cairo_image_surface_create_from_png (get_angle_url(SELECT_NO));
        cairo_set_source_surface (cr1, image1, 47, 6);
        cairo_set_source_surface (cr2, image2, 73, 7);
        cairo_rectangle (cr7, 0, 38, 128, 12);
        cairo_fill(cr7);
        cairo_stroke(cr7);
        cairo_set_operator(cr4, CAIRO_OPERATOR_CLEAR);
        break;
    case SELECT_THIRD:
        image1 = cairo_image_surface_create_from_png (get_work_mode_image_small_url(WORKMODE_JPEG, SELECT_NO));
        image2 = cairo_image_surface_create_from_png (get_angle_url(SELECT_NO));
        cairo_set_source_surface (cr1, image1, 47, 6);
        cairo_set_source_surface (cr2, image2, 73, 7);
        cairo_rectangle (cr7, 0, 50, 128, 12);
        cairo_fill(cr7);
        cairo_stroke(cr7);
    	cairo_set_operator(cr5, CAIRO_OPERATOR_CLEAR);
        break;

    default:
        break;
    }

    cairo_paint(cr1);
    cairo_paint(cr2);
    cairo_set_operator(cr6, CAIRO_OPERATOR_CLEAR);
    cairo_rectangle (cr6, 0, 0, 128, 4);
    cairo_fill(cr6);
    cairo_stroke(cr6);
    if (-1 == select)
    {
        cairo_rectangle (cr6, 0, 24, 128, 4);
        cairo_fill(cr6);
        cairo_stroke(cr6);
    }
    cairo_show_text(cr3, item1);
    cairo_show_text(cr4, item2);
    cairo_show_text(cr5, item3);
    
    cairo_destroy(cr1);
	cairo_destroy(cr2);
	cairo_destroy(cr3);
	cairo_destroy(cr4);
    cairo_destroy(cr5);
    cairo_destroy(cr6);
    cairo_destroy(cr7);
    cairo_destroy(cr8);
    cairo_surface_destroy(image1);
    cairo_surface_destroy(image2);

    return TRUE;
}

gint show_jpeg_state()
{
    gchar *ip_addr, *disk_space, *shoot;
    g_object_get(G_OBJECT (entry), "IPaddr", &ip_addr, "jpeg_disk_space_3", &disk_space, "jpeg_shoot_mode_3", &shoot, NULL);
    cairo_t *cr1 = cairo_create(surface);     //网络工作模式，图片
    cairo_t *cr2 = cairo_create(surface);     //电量，图片
    cairo_t *cr3 = cairo_create(surface);     //工作模式，图片
    cairo_t *cr4 = cairo_create(surface);     //IP时地址，字符串
    cairo_t *cr5 = cairo_create(surface);     //磁盘剩余空间，字符串
    cairo_t *cr6 = cairo_create(surface);     //分辨率，字符串
    cairo_t *cr7 = cairo_create(surface);     //拍照模式，字符串
    cairo_t *cr8 = cairo_create(surface);     //照片存储格式，字符串
    cairo_t *cr9 = cairo_create(surface);
	cairo_set_operator(cr9, CAIRO_OPERATOR_CLEAR);
	cairo_rectangle (cr9, 0, 0, 128, 128);
	cairo_fill(cr9);
	cairo_stroke(cr9);
    cairo_surface_t *image1 = cairo_image_surface_create_from_png (get_network_image_url());
	cairo_surface_t *image2 = cairo_image_surface_create_from_png (get_battery_image_url());
	cairo_surface_t *image3 = cairo_image_surface_create_from_png (get_work_mode_image_small_url(WORKMODE_JPEG, SELECT_NO));
    cairo_set_source_surface (cr1, image1, 1, 4);
    cairo_set_source_surface (cr2, image2, 108, 4);
    cairo_set_source_surface (cr3, image3, 4, 23);

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
    cairo_destroy(cr9);
    cairo_surface_destroy(image1);
    cairo_surface_destroy(image2);
    cairo_surface_destroy(image3);

    return TRUE;
}
