//# include <cairo.h>
#include "/usr/include/cairo/cairo.h"

int draw_network_mode()
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

int draw_IP_addr()
{
    return 0;
}

int draw_battery()
{
    return 0;
}

// 测试程序
int main()
{

}