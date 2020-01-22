//export PATH=/home/wang/hi3559av_buitroot/buildroot-env/output/host/bin:$PATH
//export PKG_CONFIG_PATH=/home/linux/git/buildroot-env/output/build/libgtk3-3.22.30/:$PATH
//aarch64-buildroot-linux-gnu-gcc cairo_test.c -o cairo_test  -lcairo `pkg-config --libs --cflags cairo gtk+-3.0` -I/home/linux/git/buildroot-env/output/host/lib/glib-2.0/include/

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

#define WIDTH (128)
#define HIGH (64)

typedef struct _cairo_linuxfb_device {
	int fb_fd;
	unsigned char *fb_data;
	long fb_screensize;
	struct fb_var_screeninfo fb_vinfo;
	struct fb_fix_screeninfo fb_finfo;
} cairo_linuxfb_device_t;

/* Destroy a cairo surface */
void cairo_linuxfb_surface_destroy(void *device)
{
	cairo_linuxfb_device_t *dev = (cairo_linuxfb_device_t *)device;

	if (dev == NULL)
		return;

	munmap(dev->fb_data, dev->fb_screensize);
	close(dev->fb_fd);
	free(dev);
}

int main ( int argc, char *argv[] )
{
#if 0
	int i = 1;
	int fbfd = 0;
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	long int screensize = 0;
	int bits_per_pixel = 1;
	cairo_linuxfb_device_t *device;
	device = malloc(sizeof(*device));
	//打开显示设备
//	fbfd = open("/dev/fb0", O_RDWR);
	fbfd = open("./111", O_RDWR);
	if (fbfd < 0)
	{
		printf("fbfd is %d\n",fbfd);
		printf("Error: cannot open framebuffer device.\n");
		exit(1);
	}
	//计算屏幕的总大小（字节）
	screensize = 128 * 64 * bits_per_pixel/8;
	//对象映射
	char *fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
	if ((int)fbp == -1)
	{
		printf("Error: failed to map framebuffer device to memory.\n");
		exit(4);
	}
#endif
	printf("into show_bmp function\n");
#if 0
    cairo_surface_t *surface = cairo_image_surface_create_for_data((unsigned char*)fbp, CAIRO_FORMAT_A1, WIDTH, HIGH, 
																		cairo_format_stride_for_width(CAIRO_FORMAT_A1,WIDTH));
    cairo_surface_set_user_data(surface, NULL, device, &cairo_linuxfb_surface_destroy);
#else
	cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_A1, 128, 64);
#endif

	cairo_t *cr1 = cairo_create(surface);
    cairo_t *cr2 = cairo_create(surface);

	cairo_rectangle (cr1, 0, 15, 127, 14);
	cairo_fill(cr1);
	cairo_stroke(cr1);
	cairo_select_font_face(cr2, "serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr2, 12.0);
    cairo_set_source_rgba(cr2, 1.0, 1.0, 1.0, 1); 
    cairo_move_to(cr2, 16.0, 13.0);     //座标，以左下角为基准
    cairo_show_text(cr2, "192.168.100.100");

	cairo_set_operator(cr2, CAIRO_OPERATOR_CLEAR);
	cairo_set_source_rgba(cr2, 1.0, 1.0, 1.0, 1); 
	cairo_move_to(cr2, 16.0, 30.0);     //座标，以左下角为基准
    cairo_show_text(cr2, "192.168.100.100");

	cairo_destroy(cr1);
    cairo_destroy(cr2);
	cairo_surface_write_to_png(surface, "test.png");

//	munmap(fbp, screensize);
//	close(fbfd);
    cairo_surface_destroy(surface);

	return 0;
}