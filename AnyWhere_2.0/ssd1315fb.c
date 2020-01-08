/*
 * Driver for the Solomon SSD1315 OLED controller
 *
 * Copyright 2012 Free Electrons
 *
 * Licensed under the GPLv2 or later.
 */

#include <linux/backlight.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <linux/i2c.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/pwm.h>
#include <linux/uaccess.h>

#define SSD1315FB_DATA			0x40
#define SSD1315FB_COMMAND		0x00

#define SSD1315FB_SET_ADDRESS_MODE	0x20
#define SSD1315FB_SET_LOW_COLUMN_ADDR  (0x00)
#define SSD1315FB_SET_HIGH_COLUMN_ADDR  (0x10)
#define SSD1315FB_SET_START_LINE_ADDR  (0x40)
#define SSD1315FB_128  0xff
#define SSD1315FB_NORMAL  0xa6   //A6/A7:显示设置：A6表示1是像素点白，0是像素点黑，A7相反
#define SSD1315FB_DUTY  0x3f
#define SSD1315FB_SCAN_DIRACTION  0xc8     //C0和C8 为两个相反的扫描方向
#define SSD1315FB_SET_AREA_COLOR_OFF 0xd8
#define SSD1315FB_SET_ADDRESS_MODE_VERTICAL	(0x01)
#define SSD1315FB_SET_ADDRESS_MODE_PAGE		(0x02)
#define SSD1315FB_SET_COL_RANGE		0x21
#define SSD1315FB_SET_PAGE_RANGE	0x22
#define SSD1315FB_CONTRAST		0x81
#define	SSD1315FB_CHARGE_PUMP		0x8d
#define SSD1315FB_SEG_REMAP_ON		0xa1   // A0和A1 设置左右翻转
#define SSD1315FB_DISPLAY_OFF		0xae
#define SSD1315FB_SET_MULTIPLEX_RATIO	0xa8
#define SSD1315FB_DISPLAY_ON		0xaf
#define SSD1315FB_START_PAGE_ADDRESS	0xb0
#define SSD1315FB_SET_DISPLAY_OFFSET	0xd3
#define	SSD1315FB_SET_CLOCK_FREQ	0xd5
#define	SSD1315FB_SET_PRECHARGE_PERIOD	0xd9
#define	SSD1315FB_SET_COM_PINS_CONFIG	0xda
#define	SSD1315FB_SET_VCOMH		0xdb     //定义初始化值

#define MAX_CONTRAST 255

#define REFRESHRATE 20

static u_int refreshrate = REFRESHRATE;
module_param(refreshrate, uint, 0);

struct ssd1315fb_par;

struct ssd1315fb_deviceinfo {
	u32 default_vcomh;
	u32 default_dclk_div;
	u32 default_dclk_frq;
	int need_pwm;
	int need_chargepump;
};

struct ssd1315fb_par {
	u32 com_invdir;
	u32 com_lrremap;
	u32 com_offset;
	u32 com_seq;
	u32 contrast;
	u32 dclk_div;
	u32 dclk_frq;
	const struct ssd1315fb_deviceinfo *device_info;
	struct i2c_client *client;
	u32 height;
	struct fb_info *info;
	u32 page_offset;
	u32 prechargep1;
	u32 prechargep2;
	struct pwm_device *pwm;
	u32 pwm_period;
	int reset;
	u32 seg_remap;
	u32 vcomh;
	u32 width;
};

struct ssd1315fb_array {
	u8	type;
	u8	data[0];
};

const u8 start[]={
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x06,0x06,0xFE,0xFE,0xFE,0x06,0x06,0x06,0x00,
0x00,0xFE,0xFE,0xFE,0x86,0x86,0x86,0x86,0x06,0x06,0x00,0x00,0xFC,0xFE,0xFE,0x06,
0x06,0x06,0x1E,0x1E,0x1C,0x00,0x00,0xFE,0xFE,0xFE,0x80,0x80,0x80,0xFE,0xFE,0xFE,
0x00,0x00,0xFE,0xFE,0xFE,0x86,0x86,0x86,0x86,0x06,0x06,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x7F,0x7F,0x00,0x00,0x00,0x00,
0x00,0x7F,0x7F,0x7F,0x61,0x61,0x61,0x61,0x60,0x60,0x00,0x00,0x3F,0x7F,0x7F,0x60,
0x60,0x60,0x78,0x78,0x38,0x00,0x00,0x7F,0x7F,0x7F,0x01,0x01,0x01,0x7F,0x7F,0x7F,
0x00,0x00,0x7F,0x7F,0x7F,0x61,0x61,0x61,0x61,0x60,0x60,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0xC0,0x20,0x20,0x20,0x20,0xC0,0x00,0x00,0x80,0x40,0x20,
0x20,0x20,0x00,0x00,0x00,0xC0,0x20,0x20,0x20,0x20,0xC0,0x00,0x00,0xC0,0x20,0x20,
0x20,0x20,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x30,0x40,0x42,0x42,0x42,0x3D,0x00,0x00,0x3F,0x42,0x42,
0x42,0x42,0x3C,0x00,0x00,0x3F,0x48,0x44,0x42,0x41,0x3F,0x00,0x00,0x7F,0x04,0x04,
0x04,0x04,0x7F,0x00,0x00,0x7F,0x01,0x01,0x01,0x01,0x7E,0x00,0x00,0x3F,0x40,0x40,
0x40,0x40,0xFF,0x00,0x3F,0x40,0x40,0x7C,0x40,0x40,0x3F,0x00,0x00,0x7F,0x01,0x01,
0x01,0x01,0x7E,0x00,0x00,0x3E,0x49,0x49,0x49,0x49,0x0E,0x00,0x00,0x7F,0x02,0x01,
0x01,0x01,0x01,0x00,0x00,0x3E,0x49,0x49,0x49,0x49,0x0E,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x02,
0x02,0x02,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

static const struct fb_fix_screeninfo ssd1315fb_fix = {
	.id		= "Solomon SSD1315",
	.type		= FB_TYPE_PACKED_PIXELS,
	.visual		= FB_VISUAL_MONO10,
	.xpanstep	= 0,
	.ypanstep	= 0,
	.ywrapstep	= 0,
	.accel		= FB_ACCEL_NONE,
};

static const struct fb_var_screeninfo ssd1315fb_var = {
	.bits_per_pixel	= 1,
};

static struct ssd1315fb_array *ssd1315fb_alloc_array(u32 len, u8 type)
{
	struct ssd1315fb_array *array;

	array = kzalloc(sizeof(struct ssd1315fb_array) + len, GFP_KERNEL);
	if (!array)
		return NULL;

	array->type = type;

	return array;
}

static int ssd1315fb_write_array(struct i2c_client *client,
				 struct ssd1315fb_array *array, u32 len)
{
	int ret;

	len += sizeof(struct ssd1315fb_array);

	ret = i2c_master_send(client, (u8 *)array, len);
	if (ret != len) {
		dev_err(&client->dev, "Couldn't send I2C command.\n");
		return ret;
	}

	return 0;
}

static inline int ssd1315fb_write_cmd(struct i2c_client *client, u8 cmd)
{
	struct ssd1315fb_array *array;
	int ret;

	array = ssd1315fb_alloc_array(1, SSD1315FB_COMMAND);
	if (!array)
		return -ENOMEM;

	array->data[0] = cmd;

	ret = ssd1315fb_write_array(client, array, 1);
	kfree(array);

	return ret;
}
#if 1
void ssd1315fb_set_postion(struct i2c_client *client, uint8_t x, uint8_t y)
{
	ssd1315fb_write_cmd(client, 0xb0+y);
	ssd1315fb_write_cmd(client, ((x&0xf0)>>4)|0x10);
	ssd1315fb_write_cmd(client, (x&0x0f));
}
#endif

static void ssd1315fb_update_display(struct ssd1315fb_par *par)
{
	struct ssd1315fb_array *array;
	u8 *vmem = par->info->screen_base;
	int i, j, k;
	int x = 0, y = 0;

	array = ssd1315fb_alloc_array(par->width * par->height / 8, SSD1315FB_DATA);       //得到的申请到的数据内存空间
	if (!array)
		return;

	/*
	 * The screen is divided in pages, each having a height of 8
	 * pixels, and the width of the screen. When sending a byte of
	 * data to the controller, it gives the 8 bits for the current
	 * column. I.e, the first byte are the 8 bits of the first
	 * column, then the 8 bits for the second column, etc.
	 *
	 *
	 * Representation of the screen, assuming it is 5 bits
	 * wide. Each letter-number combination is a bit that controls
	 * one pixel.
	 *
	 * A0 A1 A2 A3 A4
	 * B0 B1 B2 B3 B4
	 * C0 C1 C2 C3 C4
	 * D0 D1 D2 D3 D4
	 * E0 E1 E2 E3 E4
	 * F0 F1 F2 F3 F4
	 * G0 G1 G2 G3 G4
	 * H0 H1 H2 H3 H4
	 *
	 * If you want to update this screen, you need to send 5 bytes:
	 *  (1) A0 B0 C0 D0 E0 F0 G0 H0
	 *  (2) A1 B1 C1 D1 E1 F1 G1 H1
	 *  (3) A2 B2 C2 D2 E2 F2 G2 H2
	 *  (4) A3 B3 C3 D3 E3 F3 G3 H3
	 *  (5) A4 B4 C4 D4 E4 F4 G4 H4
	 */

	ssd1315fb_set_postion(par->client, x, y);
	for (i = 0; i < (par->height / 8); i++) {
		for (j = 0; j < par->width; j++) {
			array->data[0] = 0;
			for (k = 0; k < 8; k++) {
				u32 page_length = par->width * i;
				u32 index = page_length + (par->width * k + j) / 8;
				u8 byte = *(vmem + index);
				u8 bit = byte & (1 << (j % 8));
				bit = bit >> (j % 8);
				array->data[0] |= bit << k;
			}    //array是传递消息的结构体
			ssd1315fb_write_array(par->client, array, 1);  //通过I2C写数据
			x++;
			if(x > 127)
			{
				x = 0;
				y++;
				ssd1315fb_set_postion(par->client, x, y);
			}
		}
	}

	kfree(array);
}


static ssize_t ssd1315fb_write(struct fb_info *info, const char __user *buf,
		size_t count, loff_t *ppos)
{
	struct ssd1315fb_par *par = info->par;
	unsigned long total_size;
	unsigned long p = *ppos;
	u8 __iomem *dst;

	total_size = info->fix.smem_len;

	if (p > total_size)
		return -EINVAL;

	if (count + p > total_size)
		count = total_size - p;

	if (!count)
		return -EINVAL;

	dst = (void __force *) (info->screen_base + p);

	if (copy_from_user(dst, buf, count))
		return -EFAULT;

	ssd1315fb_update_display(par);

	*ppos += count;

	return count;
}

static int ssd1315fb_blank(int blank_mode, struct fb_info *info)
{
	struct ssd1315fb_par *par = info->par;

	if (blank_mode != FB_BLANK_UNBLANK)
		return ssd1315fb_write_cmd(par->client, SSD1315FB_DISPLAY_OFF);
	else
		return ssd1315fb_write_cmd(par->client, SSD1315FB_DISPLAY_ON);
}
#if 0
static void ssd1315fb_fillrect(struct fb_info *info, const struct fb_fillrect *rect)
{
	struct ssd1315fb_par *par = info->par;
	sys_fillrect(info, rect);
	ssd1315fb_update_display(par);
}

static void ssd1315fb_copyarea(struct fb_info *info, const struct fb_copyarea *area)
{
	struct ssd1315fb_par *par = info->par;
	sys_copyarea(info, area);
	ssd1315fb_update_display(par);
}

static void ssd1315fb_imageblit(struct fb_info *info, const struct fb_image *image)
{
	struct ssd1315fb_par *par = info->par;
//	sys_imageblit(info, image);
	ssd1315fb_update_display(par);
}
#endif
static struct fb_ops ssd1315fb_ops = {
	.owner		= THIS_MODULE,
//	.fb_read	= fb_sys_read,
	.fb_write	= ssd1315fb_write,
	.fb_blank	= ssd1315fb_blank,
//	.fb_fillrect	= ssd1315fb_fillrect,
//	.fb_copyarea	= ssd1315fb_copyarea,
//	.fb_imageblit	= ssd1315fb_imageblit,
};

static void ssd1315fb_deferred_io(struct fb_info *info,
				struct list_head *pagelist)
{
	ssd1315fb_update_display(info->par);
}

static int ssd1315fb_init(struct ssd1315fb_par *par)
{
	int ret, i;
	int x = 0, y = 0;
	u32 precharge, dclk, com_invdir, compins;
	struct pwm_args pargs;
	struct ssd1315fb_array *array;

	array = ssd1315fb_alloc_array(par->width * par->height / 8, SSD1315FB_DATA);       //得到的申请到的数据内存空间
	if (!array)
		return -1;

	if (par->device_info->need_pwm) {
		par->pwm = pwm_get(&par->client->dev, NULL);
		if (IS_ERR(par->pwm)) {
			dev_err(&par->client->dev, "Could not get PWM from device tree!\n");
			return PTR_ERR(par->pwm);
		}

		/*
		 * FIXME: pwm_apply_args() should be removed when switching to
		 * the atomic PWM API.
		 */
		pwm_apply_args(par->pwm);

		pwm_get_args(par->pwm, &pargs);

		par->pwm_period = pargs.period;
		/* Enable the PWM */
		pwm_config(par->pwm, par->pwm_period / 2, par->pwm_period);
		pwm_enable(par->pwm);

		dev_dbg(&par->client->dev, "Using PWM%d with a %dns period.\n",
			par->pwm->pwm, par->pwm_period);
	};

	/* Set initial contrast */
	ret = ssd1315fb_write_cmd(par->client, SSD1315FB_DISPLAY_OFF);
	if (ret < 0)
		return ret;

	ret = ssd1315fb_write_cmd(par->client, SSD1315FB_SET_LOW_COLUMN_ADDR);
	if (ret < 0)
		return ret;

	ret = ssd1315fb_write_cmd(par->client, SSD1315FB_SET_HIGH_COLUMN_ADDR);
	if (ret < 0)
		return ret;

	ret = ssd1315fb_write_cmd(par->client, SSD1315FB_START_PAGE_ADDRESS);
	if (ret < 0)
		return ret;
	
	/* Set initial contrast */
	ret = ssd1315fb_write_cmd(par->client, SSD1315FB_CONTRAST);
	if (ret < 0)
		return ret;

	ret = ssd1315fb_write_cmd(par->client, 0x8f);
	if (ret < 0)
		return ret;

	/* Set segment re-map */
	if (par->seg_remap) {
		ret = ssd1315fb_write_cmd(par->client, SSD1315FB_SEG_REMAP_ON);
		if (ret < 0)
			return ret;
	};	

	if (par->seg_remap) {
		ret = ssd1315fb_write_cmd(par->client, SSD1315FB_NORMAL);
		if (ret < 0)
			return ret;
	};

	/* Set COM direction */
	com_invdir = 0xc0 | (par->com_invdir & 0x1) << 3;
	ret = ssd1315fb_write_cmd(par->client,  SSD1315FB_SCAN_DIRACTION);
	if (ret < 0)
		return ret;

	/* Set multiplex ratio value */
	ret = ssd1315fb_write_cmd(par->client, SSD1315FB_SET_MULTIPLEX_RATIO);
	if (ret < 0)
		return ret;

	ret = ssd1315fb_write_cmd(par->client, 0x3f);
	if (ret < 0)
		return ret;

	/* set display offset value */
	ret = ssd1315fb_write_cmd(par->client, SSD1315FB_SET_DISPLAY_OFFSET);
	if (ret < 0)
		return ret;

	ret = ssd1315fb_write_cmd(par->client, 0x00);
	if (ret < 0)
		return ret;

	/* Set clock frequency */
	ret = ssd1315fb_write_cmd(par->client, SSD1315FB_SET_CLOCK_FREQ);
	if (ret < 0)
		return ret;

	dclk = ((par->dclk_div - 1) & 0xf) | (par->dclk_frq & 0xf) << 4;
	ret = ssd1315fb_write_cmd(par->client, 0xF0);
	if (ret < 0)
		return ret;

	/* Set precharge period in number of ticks from the internal clock */
	ret = ssd1315fb_write_cmd(par->client, SSD1315FB_SET_PRECHARGE_PERIOD);
	if (ret < 0)
		return ret;

	precharge = (par->prechargep1 & 0xf) | (par->prechargep2 & 0xf) << 4;
	ret = ssd1315fb_write_cmd(par->client, 0x22);
	if (ret < 0)
		return ret;

	/* Set COM pins configuration */
	ret = ssd1315fb_write_cmd(par->client, SSD1315FB_SET_COM_PINS_CONFIG);
	if (ret < 0)
		return ret;

	compins = 0x02 | !(par->com_seq & 0x1) << 4
				   | (par->com_lrremap & 0x1) << 5;
	ret = ssd1315fb_write_cmd(par->client, 0x12);
	if (ret < 0)
		return ret;

	/* Set VCOMH */
	ret = ssd1315fb_write_cmd(par->client, SSD1315FB_SET_VCOMH);
	if (ret < 0)
		return ret;

	ret = ssd1315fb_write_cmd(par->client, 0x30);
	if (ret < 0)
		return ret;

	/* Turn on the DC-DC Charge Pump */
	ret = ssd1315fb_write_cmd(par->client, SSD1315FB_CHARGE_PUMP);
	if (ret < 0)
		return ret;

	ret = ssd1315fb_write_cmd(par->client, 0x14);
//BIT(4) | (par->device_info->need_chargepump ? BIT(2) : 0));
	if (ret < 0)
		return ret;

	/* Switch to horizontal addressing mode 
	ret = ssd1315fb_write_cmd(par->client, SSD1315FB_SET_ADDRESS_MODE);
	if (ret < 0)
		return ret;

	ret = ssd1315fb_write_cmd(par->client,
				  SSD1315FB_SET_ADDRESS_MODE_HORIZONTAL);
	if (ret < 0)
		return ret;

	 Set column range 
	ret = ssd1315fb_write_cmd(par->client, SSD1315FB_SET_COL_RANGE);
	if (ret < 0)
		return ret;

	ret = ssd1315fb_write_cmd(par->client, 0x0);
	if (ret < 0)
		return ret;

	ret = ssd1315fb_write_cmd(par->client, par->width - 1);
	if (ret < 0)
		return ret;

	 Set page range */
	ret = ssd1315fb_write_cmd(par->client, SSD1315FB_SET_PAGE_RANGE);
	if (ret < 0)
		return ret;

	ret = ssd1315fb_write_cmd(par->client, 0x0);
	if (ret < 0)
		return ret;

	ret = ssd1315fb_write_cmd(par->client,
				  par->page_offset + (par->height / 8) - 1);
	if (ret < 0)
		return ret;

	/* Turn on the display */
	ret = ssd1315fb_write_cmd(par->client, SSD1315FB_DISPLAY_ON);
	if (ret < 0)
		return ret;
	
	ssd1315fb_set_postion(par->client, x, y);
	for (i = 0; i < 1024; i++)
	{
		array->data[0] = start[i];
		ssd1315fb_write_array(par->client, array, 1);
		x++;
			if(x > 127)
			{
				x = 0;
				y++;
				ssd1315fb_set_postion(par->client, x, y);
			}
	}

	gpio_request(62, NULL);
	gpio_direction_output(62, 1);
	gpio_set_value(62, 1);

	return 0;
}

static int ssd1315fb_update_bl(struct backlight_device *bdev)
{
	struct ssd1315fb_par *par = bl_get_data(bdev);
	int ret;
	int brightness = bdev->props.brightness;

	par->contrast = brightness;

	ret = ssd1315fb_write_cmd(par->client, SSD1315FB_CONTRAST);
	if (ret < 0)
		return ret;
	ret = ssd1315fb_write_cmd(par->client, par->contrast);
	if (ret < 0)
		return ret;
	return 0;
}

static int ssd1315fb_get_brightness(struct backlight_device *bdev)
{
	struct ssd1315fb_par *par = bl_get_data(bdev);

	return par->contrast;
}

static int ssd1315fb_check_fb(struct backlight_device *bdev,
				   struct fb_info *info)
{
	return (info->bl_dev == bdev);
}

static const struct backlight_ops ssd1315fb_bl_ops = {
	.options	= BL_CORE_SUSPENDRESUME,
	.update_status	= ssd1315fb_update_bl,
	.get_brightness	= ssd1315fb_get_brightness,
	.check_fb	= ssd1315fb_check_fb,
};

static struct ssd1315fb_deviceinfo ssd1315fb_ssd1305_deviceinfo = {
	.default_vcomh = 0x34,
	.default_dclk_div = 1,
	.default_dclk_frq = 7,
};

static struct ssd1315fb_deviceinfo ssd1315fb_ssd1306_deviceinfo = {
	.default_vcomh = 0x20,
	.default_dclk_div = 1,
	.default_dclk_frq = 8,
	.need_chargepump = 1,
};

static struct ssd1315fb_deviceinfo ssd1315fb_ssd1315_deviceinfo = {
	.default_vcomh = 0x30,
	.default_dclk_div = 1,
	.default_dclk_frq = 8,
};

static struct ssd1315fb_deviceinfo ssd1315fb_ssd1309_deviceinfo = {
	.default_vcomh = 0x34,
	.default_dclk_div = 1,
	.default_dclk_frq = 10,
};

static const struct of_device_id ssd1315fb_of_match[] = {
	{
		.compatible = "solomon,ssd1305fb-i2c",
		.data = (void *)&ssd1315fb_ssd1305_deviceinfo,
	},
	{
		.compatible = "solomon,ssd1306fb-i2c",
		.data = (void *)&ssd1315fb_ssd1306_deviceinfo,
	},
	{
		.compatible = "solomon,ssd1315fb-i2c",
		.data = (void *)&ssd1315fb_ssd1315_deviceinfo,
	},
	{
		.compatible = "solomon,ssd1309fb-i2c",
		.data = (void *)&ssd1315fb_ssd1309_deviceinfo,
	},
	{},
};
MODULE_DEVICE_TABLE(of, ssd1315fb_of_match);

static int ssd1315fb_probe(struct i2c_client *client,
			   const struct i2c_device_id *id)
{
	struct backlight_device *bl;
	char bl_name[12];
	struct fb_info *info;
	struct device_node *node = client->dev.of_node;
	struct fb_deferred_io *ssd1315fb_defio;
	u32 vmem_size;
	struct ssd1315fb_par *par;
	u8 *vmem;
	int ret;

	if (!node) {
		dev_err(&client->dev, "No device tree data found!\n");
		return -EINVAL;
	}

	info = framebuffer_alloc(sizeof(struct ssd1315fb_par), &client->dev);
	if (!info) {
		dev_err(&client->dev, "Couldn't allocate framebuffer.\n");
		return -ENOMEM;
	}

	par = info->par;
	par->info = info;
	par->client = client;

	par->device_info = of_device_get_match_data(&client->dev);

	par->reset = of_get_named_gpio(client->dev.of_node,
					 "reset-gpios", 0);
	if (!gpio_is_valid(par->reset)) {
		ret = -EINVAL;
		goto fb_alloc_error;
	}

	if (of_property_read_u32(node, "solomon,width", &par->width))
		par->width = 128;

	if (of_property_read_u32(node, "solomon,height", &par->height))
		par->height = 64;

	if (of_property_read_u32(node, "solomon,page-offset", &par->page_offset))
		par->page_offset = 1;

	if (of_property_read_u32(node, "solomon,com-offset", &par->com_offset))
		par->com_offset = 0;

	if (of_property_read_u32(node, "solomon,prechargep1", &par->prechargep1))
		par->prechargep1 = 2;

	if (of_property_read_u32(node, "solomon,prechargep2", &par->prechargep2))
		par->prechargep2 = 2;

	par->seg_remap = !of_property_read_bool(node, "solomon,segment-no-remap");
	par->com_seq = of_property_read_bool(node, "solomon,com-seq");
	par->com_lrremap = of_property_read_bool(node, "solomon,com-lrremap");
	par->com_invdir = of_property_read_bool(node, "solomon,com-invdir");

	par->contrast = 127;
	par->vcomh = par->device_info->default_vcomh;

	/* Setup display timing */
	par->dclk_div = par->device_info->default_dclk_div;
	par->dclk_frq = par->device_info->default_dclk_frq;

	vmem_size = par->width * par->height / 8;

	vmem = (void *)__get_free_pages(GFP_KERNEL | __GFP_ZERO,
					get_order(vmem_size));
	if (!vmem) {
		dev_err(&client->dev, "Couldn't allocate graphical memory.\n");
		ret = -ENOMEM;
		goto fb_alloc_error;
	}

	ssd1315fb_defio = devm_kzalloc(&client->dev, sizeof(struct fb_deferred_io), GFP_KERNEL);
	if (!ssd1315fb_defio) {
		dev_err(&client->dev, "Couldn't allocate deferred io.\n");
		ret = -ENOMEM;
		goto fb_alloc_error;
	}

	ssd1315fb_defio->delay = HZ / refreshrate;
	ssd1315fb_defio->deferred_io = ssd1315fb_deferred_io;

	info->fbops = &ssd1315fb_ops;
	info->fix = ssd1315fb_fix;
	info->fix.line_length = par->width / 8;
	info->fbdefio = ssd1315fb_defio;

	info->var = ssd1315fb_var;
	info->var.xres = par->width;
	info->var.xres_virtual = par->width;
	info->var.yres = par->height;
	info->var.yres_virtual = par->height;

	info->var.red.length = 1;
	info->var.red.offset = 0;
	info->var.green.length = 1;
	info->var.green.offset = 0;
	info->var.blue.length = 1;
	info->var.blue.offset = 0;

	info->screen_base = (u8 __force __iomem *)vmem;
	info->fix.smem_start = __pa(vmem);
	info->fix.smem_len = vmem_size;

	fb_deferred_io_init(info);

	ret = devm_gpio_request_one(&client->dev, par->reset,
				    GPIOF_OUT_INIT_HIGH,
				    "oled-reset");
	if (ret) {
		dev_err(&client->dev,
			"failed to request gpio %d: %d\n",
			par->reset, ret);
		goto reset_oled_error;
	}

	i2c_set_clientdata(client, info);

	/* Reset the screen */
	gpio_set_value(par->reset, 0);
	udelay(4);
	gpio_set_value(par->reset, 1);
	udelay(4);

	ret = ssd1315fb_init(par);
	if (ret)
		goto reset_oled_error;

	ret = register_framebuffer(info);
	if (ret) {
		dev_err(&client->dev, "Couldn't register the framebuffer\n");
		goto panel_init_error;
	}

	snprintf(bl_name, sizeof(bl_name), "ssd1315fb%d", info->node);
	bl = backlight_device_register(bl_name, &client->dev, par,
				       &ssd1315fb_bl_ops, NULL);
	if (IS_ERR(bl)) {
		ret = PTR_ERR(bl);
		dev_err(&client->dev, "unable to register backlight device: %d\n",
			ret);
		goto bl_init_error;
	}

	bl->props.brightness = par->contrast;
	bl->props.max_brightness = MAX_CONTRAST;
	info->bl_dev = bl;

	dev_info(&client->dev, "fb%d: %s framebuffer device registered, using %d bytes of video memory\n", info->node, info->fix.id, vmem_size);

	return 0;

bl_init_error:
	unregister_framebuffer(info);
panel_init_error:
	if (par->device_info->need_pwm) {
		pwm_disable(par->pwm);
		pwm_put(par->pwm);
	};
reset_oled_error:
	fb_deferred_io_cleanup(info);
fb_alloc_error:
	framebuffer_release(info);
	return ret;
}

static int ssd1315fb_remove(struct i2c_client *client)
{
	struct fb_info *info = i2c_get_clientdata(client);
	struct ssd1315fb_par *par = info->par;

	ssd1315fb_write_cmd(par->client, SSD1315FB_DISPLAY_OFF);

	backlight_device_unregister(info->bl_dev);

	unregister_framebuffer(info);
	if (par->device_info->need_pwm) {
		pwm_disable(par->pwm);
		pwm_put(par->pwm);
	};
	fb_deferred_io_cleanup(info);
	__free_pages(__va(info->fix.smem_start), get_order(info->fix.smem_len));
	framebuffer_release(info);

	return 0;
}

static const struct i2c_device_id ssd1315fb_i2c_id[] = {
	{ "ssd1305fb", 0 },
	{ "ssd1306fb", 0 },
	{ "ssd1315fb", 0 },
	{ "ssd1309fb", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, ssd1315fb_i2c_id);

static struct i2c_driver ssd1315fb_driver = {
	.probe = ssd1315fb_probe,
	.remove = ssd1315fb_remove,
	.id_table = ssd1315fb_i2c_id,
	.driver = {
		.name = "ssd1315fb",
		.of_match_table = ssd1315fb_of_match,
	},
};

module_i2c_driver(ssd1315fb_driver);

MODULE_DESCRIPTION("FB driver for the Solomon SSD1315 OLED controller");
MODULE_AUTHOR("Maxime Ripard <maxime.ripard@free-electrons.com>");
MODULE_LICENSE("GPL");
