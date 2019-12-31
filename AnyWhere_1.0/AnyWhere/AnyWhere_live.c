#include "AnyWhere_GTK.h"

gint show_live_state()
{
    gchar *ip_addr, *worktime, *resolution, *bitrate, *framerate;
    g_object_get(G_OBJECT (entry), "IPaddr", &ip_addr, "live_work_time_9", &worktime, "live_resolution_9", &resolution,
                                                       "live_bitrate_9", &bitrate, "live_framerate_9", &framerate, NULL);
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
	cairo_surface_t *image3 = cairo_image_surface_create_from_png (get_work_mode_image_small_url(WORKMODE_LIVE, SELECT_NO));
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
    cairo_move_to(cr5, 35.0, 40.0);     //剩余空间位置
    cairo_show_text(cr5, worktime);
    cairo_select_font_face(cr6, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr6, 14.0);
    cairo_set_source_rgb(cr6, 1.0, 1.0, 1.0); 
    cairo_move_to(cr6, 4.0, 60.0);     //分辨率位置
    cairo_show_text(cr6, resolution);
    cairo_select_font_face(cr7, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr7, 14.0);
    cairo_set_source_rgb(cr7, 1.0, 1.0, 1.0); 
    cairo_move_to(cr7, 33.0, 60.0);     //帧率位置
    cairo_show_text(cr7, framerate);
    cairo_select_font_face(cr8, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr8, 14.0);
    cairo_set_source_rgb(cr8, 1.0, 1.0, 1.0); 
    cairo_move_to(cr8, 90.0, 60.0);     //码率位置
    cairo_show_text(cr8, bitrate);

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

gint show_live_setting()
{
    
}