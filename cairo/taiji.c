#include <cairo.h>
#include <stdio.h>

#define SIZE	(640)		/* 太极图大小 */
#define CX	(SIZE / 2)	/* 太极圆中心横坐标 */
#define CY	(SIZE / 2)	/* 太极圆中心纵坐标 */
#define R	(SIZE / 2)	/* 太极圆半径 */

/* 把角度转换为所对应的弧度 */
#define ANGLE(ang)	(ang * 3.1415926 / 180.0)

int main(int argc, char *argv[])
{
	/* 创建32位RGBA颜色格式的Cairo绘图环境，直接在Memory中渲染 */
	cairo_surface_t* surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, SIZE + 1, SIZE + 1);
	cairo_t* cr = cairo_create(surface);

	/* 绘制太极边框 */
	cairo_set_line_width(cr, 2.0);
	cairo_set_source_rgba(cr, 0, 0, 0, 1);
	cairo_arc(cr, CX, CY, R, ANGLE(0), ANGLE(360));
	cairo_stroke(cr);
 
	/* 绘制阴阳圆 */
	cairo_set_source_rgba(cr, 0, 0, 0, 1);
	cairo_arc(cr, CX, CY, R, ANGLE(90), ANGLE(270));
	cairo_fill(cr);
	cairo_set_source_rgba(cr, 1, 1, 1, 1);
	cairo_arc(cr, CX, CY, R, ANGLE(-90), ANGLE(90));
	cairo_fill(cr);
 
	/* 绘制阴阳线 */
	cairo_set_source_rgba(cr, 0, 0, 0, 1);
	cairo_arc(cr, CX, CY - R / 2, R / 2, ANGLE(-90), ANGLE(90));
	cairo_fill(cr);
	cairo_set_source_rgba(cr, 1, 1, 1, 1);
	cairo_arc(cr, CX, CY + R / 2, R / 2, ANGLE(90), ANGLE(270));
	cairo_fill(cr);
 
	/* 绘制太极眼 */
	cairo_set_source_rgba(cr, 1, 1, 1, 1);
	cairo_arc(cr, CX, CY - R / 2, R / 10, ANGLE(0), ANGLE(360));
	cairo_fill(cr);
	cairo_set_source_rgba(cr, 0, 0, 0, 1);
	cairo_arc(cr, CX, CY + R / 2, R / 10, ANGLE(0), ANGLE(360));
	cairo_fill(cr);
 
//	printf("test %d\n",*cr);
	/* 将Memory的渲染效果存储到图片中 */
	cairo_surface_write_to_png(surface, "taichi.png");
	/* 销毁并退出Cairo绘图环境 */
	cairo_destroy(cr);
	cairo_surface_destroy (surface);
 
	return 0;
}
