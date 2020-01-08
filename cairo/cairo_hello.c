
#include <cairo/cairo.h>
#include <fcntl.h>
#include <linux/types.h>
#include <linux/ioctl.h>
#include <linux/fb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

int main()
{
    cairo_surface_t *surface = 
        cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 128, 64);     //基础画布大小，即分辨率,默认白色底
    cairo_t *cr1 = 
        cairo_create(surface);
    cairo_t *cr2 = 
        cairo_create(surface);

    cairo_rectangle (cr1, 0, 15, 120, 14);
    cairo_fill(cr1);
    cairo_stroke(cr1);
	cairo_select_font_face(cr2, "serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr2, 12.0);
    cairo_set_source_rgba(cr2, 0.0, 0.0, 0.0, 1); 
    cairo_move_to(cr2, 16.0, 13.0);     //座标，以左下角为基准
    cairo_show_text(cr2, "192.168.100.100");

	cairo_set_operator(cr2, CAIRO_OPERATOR_CLEAR);
	cairo_set_source_rgba(cr2, 1.0, 1.0, 1.0, 1); 
	cairo_move_to(cr2, 16.0, 30.0);     //座标，以左下角为基准
    cairo_show_text(cr2, "192.168.100.100");

    cairo_destroy(cr1);
    cairo_destroy(cr2);
    cairo_surface_write_to_png(surface, "hello.png");
    cairo_surface_destroy(surface);

    return 0;
}