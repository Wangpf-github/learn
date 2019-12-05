#include <stdio.h>
#include "/usr/include/cairo/cairo.h"

int drawing_start()
{
    cairo_surface_t *surface = 
        cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 128, 64);     //基础画布大小，即分辨率,默认白色底
    cairo_t *cr1 = 
        cairo_create(surface);
    cairo_t *cr2 = 
        cairo_create(surface);

    cairo_select_font_face(cr1, "cursive", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);  //设置字体样式，倾斜，加粗
    cairo_select_font_face(cr2, "serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr1, 20.0);
    cairo_set_font_size(cr2, 13.0);
    cairo_set_source_rgb(cr1, 0.0, 0.0, 0.0); 
    cairo_set_source_rgb(cr2, 0.0, 0.0, 0.0);      //颜色设置，rgb为0，0，0时颜色为黑色
    cairo_move_to(cr1, 24.0, 28.0);     //座标，以左中为基准
    cairo_move_to(cr2, 10.0, 50.0);
    cairo_show_text(cr1, "TECHE");
    cairo_show_text(cr2, "360 AnyWhere");

    cairo_destroy(cr1);
    cairo_destroy(cr2);
    cairo_surface_write_to_png(surface, "start.png");
    cairo_surface_destroy(surface);

    return 0;
}

int draeing_shutdown()
{
    cairo_surface_t *surface = 
        cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 128, 64);     //基础画布大小，即分辨率,默认白色底
    cairo_t *cr1 = 
        cairo_create(surface);

    cairo_select_font_face(cr1, "serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr1, 20.0);
    cairo_set_source_rgb(cr1, 0.0, 0.0, 0.0);      //颜色设置，rgb为0，0，0时颜色为黑色
    cairo_move_to(cr1, 10.0, 40.0);     //座标，以左中为基准
    cairo_show_text(cr1, "Shotdown");

    cairo_destroy(cr1);
    cairo_surface_write_to_png(surface, "shutdown.png");
    cairo_surface_destroy(surface);

    return 0;
}

int main()
{
    int ret = 0;

    ret = drawing_start();
    if(ret < 0)
    {
        printf("drawing start picture failed!\n");
    }

    ret = draeing_shutdown();
    if(ret < 0)
    {
        printf("drawing shutdown picture failed!\n");
    }

    return 0;
}