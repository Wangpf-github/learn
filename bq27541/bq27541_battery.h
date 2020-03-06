#ifndef _BQ27541_BATTERY_H
#define _BQ27541_BATTERY_H

#include <glib-object.h>
#include <glib.h>

#define BQ27541_TYPE_BATTERY (bq27541_battery_get_type())
#define BQ27541_BATTERY(object) G_TYPE_CHECK_INSTANCE_CAST((object), BQ27541_TYPE_BATTERY, Bq27541Battery)

typedef struct _Bq27541BatteryPrivate Bq27541BatteryPrivate;

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

GType bq27541_battery_get_type(void) G_GNUC_CONST;

#endif