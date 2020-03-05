#ifndef _BQ27541_BATTERY_H
#define _BQ27541_BATTERY_H

#include <glib-object.h>
#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>

#define BQ27541_TYPE_BATTERY (bq27541_battery_get_type())
//强制转换，GObject基类转换为本子类
#define BQ27541_BATTERY(object) G_TYPE_CHECK_INSTANCE_CAST((object), BQ27541_TYPE_BATTERY, Bq27541Battery)

typedef struct _Bq27541Battery
{
    GObject parent;
}Bq27541Battery;

typedef struct _Bq27541BatteryClass
{
    GObjectClass parent_class;
}Bq27541BatteryClass;

typedef enum _BatteryStatus
{
    BATTERY_UNKNOWN,
    BATTERY_CHARGING,
    BATTERY_DISCHARGING,
    BATTERY_NOT_CHARGING,
    BATTERY_FULL
}BatteryStatus;

typedef enum _BatteryAlert
{
    BATTERY_CAPACITY_MAX,
    BATTERY_CAPACITY_MIN,
    BATTERY_TEMP_MAX,
    BATTERY_TEMP_MIN
}BatteryAlert;

typedef enum _BatteryLevel
{
    BATTERY_CAPACITY_LEVEL_FULL,
    BATTERY_CAPACITY_LEVEL_NORMAL,
    BATTERY_CAPACITY_LEVEL_LOW,
    BATTERY_CAPACITY_LEVEL_CRITICAL
}BatteryLevel;

GType bq27541_battery_get_type(void);

#endif