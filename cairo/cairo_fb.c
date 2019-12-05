#include <stdio.h>
#include "/usr/include/cairo/cairo.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>

#define WIDTH (128)
#define HIGH (64)

void cairo_draw(cairo_t *cr1)
{
    cairo_select_font_face(cr1, "serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr1, 20.0);
    cairo_set_source_rgb(cr1, 0.0, 0.0, 0.0); 
    cairo_move_to(cr1, 10.0, 34.0); 
    cairo_show_text(cr1, "anywhere");
    
    return ;
}

int main ( int argc, char *argv[] )
{
	int fbfd = 0;
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	long int screensize = 0;
	int bits_per_pixel = 1;
 
	//打开显示设备
	fbfd = open("/dev/fb0", O_RDWR);
	if (fbfd < 0)
	{
		printf("fbfd is %d\n",fbfd);
		printf("Error: cannot open framebuffer device.\n");
		exit(1);
	}
	//计算屏幕的总大小（字节）
	screensize = 128 * 64 * bits_per_pixel/8;
	//对象映射
	char *fbp = (char *)mmap(NULL, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
	if ((int)fbp == -1)
	{
		printf("Error: failed to map framebuffer device to memory.\n");
		exit(4);
	}
	int i;
	printf("into show_bmp function\n");
    //使用cairo画图
//	unsigned char *data = (unsigned char *)malloc(WIDTH * HIGH *4);
    cairo_surface_t *surface = cairo_image_surface_create_for_data((unsigned char*)fbp, CAIRO_FORMAT_ARGB32, WIDTH, HIGH, WIDTH *4);
    cairo_t *oled_screen = cairo_create(surface);
    cairo_draw(oled_screen);
#if 0
    //显示图像
 	for(i = 0 ; i < screensize; i++)
		{
			*(fbp+i)=bird[i];
		}
	//删除对象映射
#endif
	munmap(fbp, screensize);
	close(fbfd);
    cairo_destroy(oled_screen);
    cairo_surface_destroy(surface);

	return 0;
}