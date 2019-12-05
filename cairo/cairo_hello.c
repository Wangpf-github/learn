
#include <stdio.h>
#include "/usr/include/cairo/cairo.h"

int main()
{
    cairo_surface_t *surface = 
        cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 128, 64);     //基础画布大小，即分辨率,默认白色底
    cairo_t *cr1 = 
        cairo_create(surface);
    cairo_t *cr2 = 
        cairo_create(surface);

    cairo_select_font_face(cr1, "serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_select_font_face(cr2, "serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr1, 20.0);
    cairo_set_font_size(cr2, 10.0);
    cairo_set_source_rgb(cr1, 0.0, 0.0, 0.0); 
    cairo_set_source_rgb(cr2, 0.0, 0.0, 0.0);      //颜色设置，rgb为0，0，0时颜色为黑色
    cairo_move_to(cr1, 10.0, 34.0);     //座标，以左下角为基准
    cairo_move_to(cr2, 2.0, 15.0);
    cairo_show_text(cr1, "anywhere");
    cairo_show_text(cr2, "10.0.0.61");

    cairo_t *line = cairo_create(surface);
    cairo_move_to(line, 1, 63);
    cairo_line_to(line, 128, 63);
    cairo_set_source_rgb(line, 0.0, 1.0, 0.0);
    cairo_set_line_width(line, 1);
    cairo_stroke(line);

    cairo_destroy(cr1);
    cairo_destroy(cr2);
    cairo_surface_write_to_png(surface, "hello.png");
    cairo_surface_destroy(surface);

    return 0;
}