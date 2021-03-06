#include <stdio.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define OLED_ADDRESS    0x78
#define I2C_RETRIES     0x0701  /* number of times a device address should
				   be polled when not acknowledging */
#define I2C_TIMEOUT     0x0702  /* set timeout in units of 10 ms */
/* NOTE: Slave address is 7 or 10 bits, but 10-bit addresses
 * are NOT supported! (due to code brokenness)
 */
#define I2C_SLAVE       0x0703  /* Use this slave address */
#define I2C_SLAVE_FORCE 0x0706  /* Use this slave address, even if it
				   is already in use by a driver! */
#define I2C_TENBIT      0x0704  /* 0 for 7 bit addrs, != 0 for 10 bit */

#define I2C_FUNCS       0x0705  /* Get the adapter functionality mask */

#define I2C_RDWR        0x0707  /* Combined R/W transfer (one STOP only) */

#define I2C_PEC         0x0708  /* != 0 to use PEC with SMBus */
#define I2C_SMBUS       0x0720
#define I2C_16BIT_REG   0x0709  /* 16BIT REG WIDTH */
#define I2C_16BIT_DATA  0x070a  /* 16BIT DATA WIDTH */
int i2c_start(char *fbp);
int main()
{
	int fbfd;
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	long int screensize = 0;
	char *fbp = NULL;
	int i;
	fbfd = open("/dev/fb0", O_RDWR);
	ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo);
	ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo);
	screensize = 128*8;

	fbp = (char *)mmap(NULL, screensize, PROT_READ|PROT_WRITE, MAP_SHARED, fbfd, 0);
	printf("mmap success\n");
	printf("%p\n",fbp);
//	memset(fbp,0,screensize);
	printf("memset success\n");
	for (i=0;i < screensize/4; i++ )
	{       
		*(fbp+i) = 0xff;
	}
	printf("map success\n");

	i2c_start(fbp);
	munmap(fbp, screensize);
	close(fbfd);
}
int i2c_start(char *fbp)
{
	int i2c_fd = -1;
	unsigned int index = 0;
	char buf[2] = {0};
	i2c_fd = open("/dev/i2c-0",O_RDWR);
	int ret = ioctl(i2c_fd, I2C_SLAVE_FORCE, OLED_ADDRESS); //
	if (ret < 0) {
		printf("CMD_SET_DEV error!\n");
		close(i2c_fd);
		return -1;
	}
	ret = ioctl(i2c_fd, I2C_16BIT_REG, 0);  //0 mean 8 bit
	if (ret < 0) {
		printf("CMD_SET_REG_WIDTH error!\n");
		close(i2c_fd);
		return -1;
	}
	ret = ioctl(i2c_fd, I2C_16BIT_DATA, 0);   //
	if (ret < 0) {
		printf("CMD_SET_DATA_WIDTH error!\n");
		close(i2c_fd);
		return -1;
	}
	write(i2c_fd, fbp, 1024);   //写入数据
	if(ret < 0)
	{
		printf("I2C_WRITE error!\n");
		close(i2c_fd);
		return -1;
	}
}











