//export PATH=/home/linux/git/buildroot-env/output/host/bin:$PATH
//export PKG_CONFIG_PATH=/home/linux/git/buildroot-env/output/build/libgtk3-3.22.30/:$PATH
//export PKG_CONFIG_PATH=/home/linux/git/buildroot-env/output/build/cairo-1.15.12/src/:$PATH
//aarch64-buildroot-linux-gnu-gcc cairo_fb.c -o cairo_fb  -lcairo `pkg-config --libs --cflags cairo gtk+-3.0` -I/home/linux/git/buildroot-env/output/host/lib/glib-2.0/include/

#include <gtk/gtk.h>
#include <glib.h>
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

void cairo_draw(cairo_t *cr1)
{
    cairo_select_font_face(cr1, "serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr1, 20.0);
    cairo_set_source_rgb(cr1, 0.0, 0.0, 0.0); 
    cairo_move_to(cr1, 10.0, 34.0); 
    cairo_show_text(cr1, "anywhere");
    
    return ;
}

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
	int i = 1;
	int fbfd = 0;
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	long int screensize = 0;
	int bits_per_pixel = 1;
	cairo_linuxfb_device_t *device;
	device = malloc(sizeof(*device));
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
	char *fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
	if ((int)fbp == -1)
	{
		printf("Error: failed to map framebuffer device to memory.\n");
		exit(4);
	}

	printf("into show_bmp function\n");
    
    cairo_surface_t *surface = cairo_image_surface_create_for_data((unsigned char*)fbp, CAIRO_FORMAT_A1, WIDTH, HIGH, 
																		cairo_format_stride_for_width(CAIRO_FORMAT_A1,WIDTH));
    cairo_surface_set_user_data(surface, NULL, device, &cairo_linuxfb_surface_destroy);
	cairo_t *cr1 = cairo_create(surface);
	
	cairo_t *cr3 = cairo_create(surface);
	cairo_t *cr4 = cairo_create(surface);
	cairo_t *cr5 = cairo_create(surface);
	cairo_t *cr6 = cairo_create(surface);
	cairo_t *cr7 = cairo_create(surface);
	cairo_t *cr8 = cairo_create(surface);
	cairo_surface_t *image1;
	cairo_surface_t *image2;
	cairo_surface_t *image3;
	cairo_surface_t *image4;
	image1 = cairo_image_surface_create_from_png ("./anywhere_picture/fittings/wifib.png");
	image2 = cairo_image_surface_create_from_png ("./anywhere_picture/battery_white/40.png");
	image3 = cairo_image_surface_create_from_png ("./anywhere_picture/fittings/spb2.png");
	image4 = cairo_image_surface_create_from_png ("./anywhere_picture/fittings/black.png");
	cairo_set_source_surface (cr4, image1, 1, 4);
	cairo_set_source_surface (cr5, image2, 108, 4);
	cairo_set_source_surface (cr6, image3, 6, 24);
//	cairo_set_source_surface (cr8, image4, 0, 50);
//	gtk_widget_draw(window, cr1);    //Draws widget to cr
#if 1
	cairo_select_font_face(cr1, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr1, 12.0);
    cairo_set_source_rgb(cr1, 0.0, 0.0, 0.0); 
    cairo_move_to(cr1, 16.0, 13.0);     //座标，以左下角为基准
    cairo_show_text(cr1, "192.168.100.100");

	
//    cairo_show_text(cr2, "NoSpace");

	cairo_select_font_face(cr3, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr3, 14.0);
    cairo_set_source_rgba(cr3, 1.0, 1.0, 1.0, 1); 
    cairo_move_to(cr3, 1.0, 60.0);     //座标，以左下角为基准
    cairo_show_text(cr3, "8K Stardard JPEG");

	char buf[10] = {0};
	while(1)
	{
		sprintf(buf, "%05d", i);
		cairo_t *cr7 = cairo_create(surface);
		cairo_select_font_face(cr7, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    	cairo_set_font_size(cr7, 22.0);
    	cairo_set_source_rgb(cr7, 0.0, 0.0, 0.0); 
    	cairo_move_to(cr7, 48.0, 39.0);     //座标，以左下角为基准
		cairo_show_text(cr7, buf);
		cr1 = cairo_create(surface);
		cr3 = cairo_create(image4);
		cairo_select_font_face(cr1, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    	cairo_set_font_size(cr1, 12.0);
    	cairo_set_source_rgb(cr1, 0.0, 0.0, 0.0); 
    	cairo_move_to(cr1, 16.0, 13.0);     //座标，以左下角为基准
		if(i % 4 == 0) 
		{
    		cairo_show_text(cr1, "192.168.100.1");
			image1 = cairo_image_surface_create_from_png ("./anywhere_picture/fittings/yxb.png");
			cairo_set_source_surface (cr4, image1, 1, 4);
		}
		else
		{
			cairo_show_text(cr1, "192.168.1.1");
			image1 = cairo_image_surface_create_from_png ("./anywhere_picture/fittings/wifib.png");
			cairo_set_source_surface (cr4, image1, 1, 4);
		}
		cairo_set_source_surface (cr3, image4, 0, 50);
		cairo_select_font_face(cr3, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    	cairo_set_font_size(cr3, 14.0);
    	cairo_set_source_rgba(cr3, 0.0, 0.0, 0.0, 0);
    	cairo_move_to(cr3, 1.0, 24.0);     //座标，以左下角为基准
    	cairo_show_text(cr3, "8K Stardard RAW");
		cairo_clip(cr3);
		cairo_set_operator(cr3, CAIRO_OPERATOR_OUT);

		cairo_paint (cr3);
		cairo_paint (cr4);
		cairo_paint (cr5);
		cairo_paint (cr6);
		sleep(1);
		cairo_set_operator(cr7, CAIRO_OPERATOR_CLEAR);
		cairo_paint(cr7);
		cairo_destroy(cr1);
		cairo_destroy(cr3);
		cairo_destroy(cr7);
		i++;
	}
#endif 
//	gtk_main();
	munmap(fbp, screensize);
	close(fbfd);
    cairo_destroy(cr1);
    cairo_surface_destroy(surface);

	return 0;
}