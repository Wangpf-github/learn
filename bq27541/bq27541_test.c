//???aarch64-buildroot-linux-gnu-gcc bq27541_test.c -o battery_test `pkg-config --cflags --libs glib-2.0 gobject-2.0` -I. -L. -lbq27541

#include <bq27541_battery.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

void judeg_battery_event(Bq27541Battery *bq, BatteryAlert alert, gpointer data)
{
    switch (alert)
    {
    case BATTERY_ALERT_CAPACITY_MAX:
        printf("-----Capacity over max!-----\n");
        break;
    case BATTERY_ALERT_CAPACITY_MIN:
        printf("-----Capacity less than min!-----\n");
        break;
    case BATTERY_ALERT_TEMP_MAX:
        printf("-----Temperature is too high!-----\n");
        break;
    case BATTERY_ALERT_TEMP_MIN:
        printf("-----Temperature is too low!-----\n");
        break;
    
    default:
        break;
    }

    return;
}

void judeg_battery_level(Bq27541Battery *bq, gint level, gpointer data)
{
    switch (level)
    {
    case BATTERY_CAPACITY_LEVEL_LOW:
        printf("+++++++++Level less than 10%%!++++++\n");
        break;
    case BATTERY_CAPACITY_LEVEL_CRITICAL:
        printf("+++++++++Level less than 5%%!++++++\n");
        break;
    
    default:
        break;
    }

    return;
}

gboolean battery_test(gpointer bq)
{
    BatteryStatus bstate;
    gint bcapacity;
    gint btemp;
    gint blevel;
    g_object_get(G_OBJECT(bq), "capacity", &bcapacity, NULL);
    printf("capacity is %d%%\n", bcapacity);

    g_object_get(G_OBJECT(bq), "status", &bstate, NULL);
    switch (bstate)
    {
    case BATTERY_STATUS_UNKNOWN:
        printf("State is unknown\n");
        break;
    case BATTERY_STATUS_CHARGING:
        printf("State is charging\n");
        break;
    case BATTERY_STATUS_DISCHARGING:
        printf("State is discharging\n");
        break;
    case BATTERY_STATUS_NOT_CHARGING:
        printf("State is not-charging\n");
        break;
    case BATTERY_STATUS_FULL:
        printf("State is FULL\n");
        break;
    
    default:
        break;
    }
    
    g_object_get(G_OBJECT(bq), "temp", &btemp, NULL);
    printf("Temperature is %d.\n", btemp);

    g_object_get(G_OBJECT(bq), "CapacityLevel", &blevel, NULL);
    switch (blevel)
    {
    case BATTERY_CAPACITY_LEVEL_FULL:
        printf("Level is full\n");
        break;
    case BATTERY_CAPACITY_LEVEL_NORMAL:
        printf("Level is normal\n");
        break;
    case BATTERY_CAPACITY_LEVEL_LOW:
        printf("Level is low\n");
        break;
    case BATTERY_CAPACITY_LEVEL_CRITICAL:
        printf("Level is critical\n");
        break;
    case BATTERY_CAPACITY_LEVEL_UNKNOWN:
        printf("Level is UNKNOWN\n");
        break;
    
    default:
        break;
    }

    printf("#######################################\n");
}

int main()
{
    GMainLoop *loop;
    Bq27541Battery *bq;

    bq = g_object_new(TYPE_BQ27541_BATTERY, "path", "/sys/class/power_supply/bq27541-0",
                                            "CapacityAlertMax", 95,
                                            "CapacityAlertMin", 10,
                                            "TempAlertMax",     430,
                                            "TempAlertMin",     50, 
                                            NULL);

    g_signal_connect(bq, "battery-alert", G_CALLBACK (judeg_battery_event), NULL);
    g_signal_connect(bq, "battery-level", G_CALLBACK (judeg_battery_level), NULL);

    g_timeout_add(1000, battery_test, bq);

    loop =  g_main_loop_new(NULL, TRUE);
    g_main_loop_run(loop);

    return 0;
}