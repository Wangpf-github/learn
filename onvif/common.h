//export PATH=/home/wang/Documents/Hi3536/host/bin:$PATH
#ifndef _COMMON_H_
#define _COMMON_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

//time
#define ONVIF_TIME_OUT 60000      //60s
//port
#define ONVIF_UDP_PORT 3702
#define ONVIF_TCP_PORT 5000
//ip
#define ONVIF_UDP_IP "239.255.255.250"
#define ONVIF_TCP_IP "192.168.100.86"
//frame size
#define ONVIF_FRAME_WIDTH 1920 //1280
#define ONVIF_FRAME_HEIGHT 960 //720


#endif