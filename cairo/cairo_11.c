//export PATH=/home/linux/git/buildroot-env/output/host/bin:$PATH
//export PKG_CONFIG_PATH=/home/linux/git/buildroot-env/output/build/libgtk3-3.22.30/:$PATH
//aarch64-buildroot-linux-gnu-gcc cairo_11.c -o cairo_11  -lcairo `pkg-config --libs --cflags cairo gtk+-3.0` -I/home/linux/git/buildroot-env/output/host/lib/glib-2.0/include/

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
void show_state(cairo_surface_t *surface);
void show_setting(cairo_surface_t *surface);
void show_shutdown(cairo_surface_t *surface);

void show_main(cairo_surface_t *surface)
{
    cairo_t *cr1 = cairo_create(surface);     //网络工作模式，图片
    cairo_t *cr2 = cairo_create(surface);     //电量，图片
    cairo_t *cr3 = cairo_create(surface);     //工作模式，图片
    cairo_t *cr4 = cairo_create(surface);     //IP时地址，字符串
	cairo_t *cr5 = cairo_create(surface);
	cairo_set_operator(cr5, CAIRO_OPERATOR_CLEAR);
	cairo_rectangle (cr5, 0, 0, 128, 128);
	cairo_fill(cr5);
	cairo_stroke(cr5);
    cairo_surface_t *image1 = cairo_image_surface_create_from_png ("./anywhere_picture/fittings/net_wire.png");
	cairo_surface_t *image2 = cairo_image_surface_create_from_png ("./anywhere_picture/battery/80.png");
	cairo_surface_t *image3 = cairo_image_surface_create_from_png ("./anywhere_picture/fittings/jpeg_l.png");
    cairo_set_source_surface (cr1, image1, 1, 4);
    cairo_set_source_surface (cr2, image2, 108, 4);
    cairo_set_source_surface (cr3, image3, 48, 23);

    cairo_paint(cr1);
    cairo_select_font_face(cr4, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr4, 12.0);
    cairo_set_source_rgb(cr4, 0.0, 0.0, 0.0); 
    cairo_move_to(cr4, 16.0, 13.0);     //座标，以左下角为基准
    cairo_show_text(cr4, "192.168.10.100");
    cairo_paint(cr2);
    cairo_paint(cr3);
	sleep(2);
	show_state(surface);
    cairo_destroy(cr1);
	cairo_destroy(cr2);
	cairo_destroy(cr3);
	cairo_destroy(cr4);
}
void show_state(cairo_surface_t *surface)
{
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
    cairo_surface_t *image1 = cairo_image_surface_create_from_png ("./anywhere_picture/fittings/net_wifi.png");
	cairo_surface_t *image2 = cairo_image_surface_create_from_png ("./anywhere_picture/battery/P_80.png");
	cairo_surface_t *image3 = cairo_image_surface_create_from_png ("./anywhere_picture/fittings/jpeg_s.png");
    cairo_set_source_surface (cr1, image1, 1, 4);
    cairo_set_source_surface (cr2, image2, 108, 4);
    cairo_set_source_surface (cr3, image3, 4, 23);

    cairo_paint(cr1);
    cairo_select_font_face(cr4, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr4, 12.0);
    cairo_set_source_rgb(cr4, 0.0, 0.0, 0.0); 
    cairo_move_to(cr4, 16.0, 13.0);     //IP位置
    cairo_show_text(cr4, "192.168.100.1");
    cairo_paint(cr2);
    cairo_paint(cr3);
    cairo_select_font_face(cr5, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr5, 24.0);
    cairo_set_source_rgb(cr5, 1.0, 1.0, 1.0); 
    cairo_move_to(cr5, 45.0, 40.0);     //剩余空间位置
    cairo_show_text(cr5, "00123");
    cairo_select_font_face(cr6, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr6, 14.0);
    cairo_set_source_rgb(cr6, 1.0, 1.0, 1.0); 
    cairo_move_to(cr6, 1.0, 60.0);     //分辨率位置
    cairo_show_text(cr6, "8K");
    cairo_select_font_face(cr7, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr7, 14.0);
    cairo_set_source_rgb(cr7, 1.0, 1.0, 1.0); 
    cairo_move_to(cr7, 25.0, 60.0);     //拍照模式位置
    cairo_show_text(cr7, "Standard");
    cairo_select_font_face(cr8, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr8, 14.0);
    cairo_set_source_rgb(cr8, 1.0, 1.0, 1.0); 
    cairo_move_to(cr8, 96.0, 60.0);     //存储格式位置
    cairo_show_text(cr8, "JPEG");
	sleep(2);
	show_setting(surface);
    cairo_destroy(cr1);
	cairo_destroy(cr2);
	cairo_destroy(cr3);
	cairo_destroy(cr4);
    cairo_destroy(cr5);
	cairo_destroy(cr6);
	cairo_destroy(cr7);
	cairo_destroy(cr8);
}

void show_setting(cairo_surface_t *surface)
{
	int i = 0;
	while(1)
	{
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

		cairo_select_font_face(cr3, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
		cairo_set_font_size(cr3, 12.0);
		cairo_set_source_rgb(cr3, 1.0, 1.0, 1.0); 
		cairo_move_to(cr3, 8.0, 36.0); 
		
		cairo_select_font_face(cr4, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
		cairo_set_font_size(cr4, 12.0);
		cairo_set_source_rgb(cr4, 1.0, 1.0, 1.0); 
		cairo_move_to(cr4, 8.0, 48.0);
		
		cairo_select_font_face(cr5, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
		cairo_set_font_size(cr5, 12.0);
		cairo_set_source_rgb(cr5, 1.0, 1.0, 1.0); 
		cairo_move_to(cr5, 8.0, 60.0);
		char aa[30] = {0};
		char *aaa = ".";
		sprintf(aa, "%s/anywhere_picture/fittings/jpeg_s_r.png", aaa);
		switch (i)
		{
		case 0:
			image1 = cairo_image_surface_create_from_png (aa);
			image2 = cairo_image_surface_create_from_png ("./anywhere_picture/fittings/angle_r.png");
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
		case 1:
			image1 = cairo_image_surface_create_from_png("./anywhere_picture/fittings/jpeg_s.png");
			image2 = cairo_image_surface_create_from_png("./anywhere_picture/fittings/angle.png");
			cairo_set_source_surface (cr1, image1, 47, 6);
			cairo_set_source_surface (cr2, image2, 73, 7);
			cairo_rectangle (cr7, 0, 26, 128, 12);
			cairo_fill(cr7);
			cairo_stroke(cr7);
			cairo_set_operator(cr3, CAIRO_OPERATOR_CLEAR);
			break;
		case 2:
			image1 = cairo_image_surface_create_from_png("./anywhere_picture/fittings/jpeg_s.png");
			image2 = cairo_image_surface_create_from_png("./anywhere_picture/fittings/angle.png");
			cairo_set_source_surface (cr1, image1, 47, 6);
			cairo_set_source_surface (cr2, image2, 73, 7);
			cairo_rectangle (cr7, 0, 38, 128, 12);
			cairo_fill(cr7);
			cairo_stroke(cr7);
			cairo_set_operator(cr4, CAIRO_OPERATOR_CLEAR);
			break;
		case 3:
			image1 = cairo_image_surface_create_from_png("./anywhere_picture/fittings/jpeg_s.png");
			image2 = cairo_image_surface_create_from_png("./anywhere_picture/fittings/angle.png");
			cairo_set_source_surface (cr1, image1, 47, 6);
			cairo_set_source_surface (cr2, image2, 73, 7);
			cairo_rectangle (cr7, 0, 50, 128, 16);
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
		if (i == 0)
		{
			cairo_rectangle (cr6, 0, 24, 128, 4);
			cairo_fill(cr6);
			cairo_stroke(cr6);
		}
		cairo_show_text(cr3, "Mode:Standard");
		cairo_show_text(cr4, "Shoot:Continuous");
		cairo_show_text(cr5, "Done");

		usleep(1200000);
		
		cairo_destroy(cr1);
		cairo_destroy(cr2);
		cairo_destroy(cr3);
		cairo_destroy(cr4);
		cairo_destroy(cr5);
		cairo_destroy(cr6);
		cairo_destroy(cr7);
		cairo_surface_destroy(image1);
		cairo_surface_destroy(image2);
		if (i == 3)
			show_shutdown(surface);
		i++;
		
		if (i > 3)
			i = 0;
	}
}
void show_shutdown(cairo_surface_t *surface)
{
	cairo_t *cr1 = cairo_create(surface);
	cairo_t *cr2 = cairo_create(surface);
	cairo_set_operator(cr2, CAIRO_OPERATOR_CLEAR);
	cairo_rectangle (cr2, 0, 0, 128, 128);
	cairo_fill(cr2);
	cairo_stroke(cr2);
    cairo_select_font_face(cr1, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr1, 26.0);
    cairo_set_source_rgb(cr1, 1.0, 1.0, 1.0); 
    cairo_move_to(cr1, 14.0, 42.0);     //剩余空间位置
    cairo_show_text(cr1, "Shutdown");
    cairo_destroy(cr1);
	sleep(3);
	show_main(surface);
	return;
}

int main ( int argc, char *argv[] )
{
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
    cairo_t *cr2 = cairo_create(surface);
    cairo_select_font_face(cr1, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr1, 26.0);
    cairo_set_source_rgb(cr1, 1.0, 1.0, 1.0); 
    cairo_move_to(cr1, 32.0, 28.0);     
    cairo_show_text(cr1, "TECHE");
    cairo_select_font_face(cr2, "TerminusTTF", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr2, 18.0);
    cairo_set_source_rgb(cr2, 1.0, 1.0, 1.0); 
    cairo_move_to(cr2, 6.0, 50.0);     
    cairo_show_text(cr2, "360 AnyWhere");

    cairo_destroy(cr1);
    cairo_destroy(cr2);
	sleep(3);
	show_main(surface);

	munmap(fbp, screensize);
	close(fbfd);
    cairo_surface_destroy(surface);

	return 0;
}