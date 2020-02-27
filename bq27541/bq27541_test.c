#include "bq27541_battery.h"

void judeg_battery_event(Bq27541Battery *bq, BatteryAlert alert, gpointer data)
{
    switch (alert)
    {
    case BATTERY_CAPACITY_MAX:
        printf("-----Capacity over max!-----\n");
        break;
    case BATTERY_CAPACITY_MIN:
        printf("-----Capacity less then min!-----\n");
        break;
    case BATTERY_TEMP_MAX:
        printf("-----Temperature is too high!-----\n");
        break;
    case BATTERY_TEMP_MIN:
        printf("-----Temperature is too low!-----\n");
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
    g_object_get(G_OBJECT(bq), "capacity", &bcapacity, NULL);
        printf("capacity is %d%%\n", bcapacity);

        g_object_get(G_OBJECT(bq), "status", &bstate, NULL);
        switch (bstate)
        {
        case BATTERY_CHARGING:
            printf("State is charging\n");
            break;
        case BATTERY_DISCHARGING:
            printf("State is discharging\n");
            break;
        case BATTERY_NOT_CHARGING:
            printf("State is not-charging\n");
            break;
        case BATTERY_FULL:
            printf("State is FULL\n");
            break;
        
        default:
            break;
        }
        
        g_object_get(G_OBJECT(bq), "temp", &btemp, NULL);
        printf("Temperature is %d.\n", btemp);
        printf("#######################################\n");
}

int main()
{
    GMainLoop *loop;
    Bq27541Battery *bq;

    bq = g_object_new(BQ27541_TYPE_BATTERY, "path", "/sys/class/power_supply/bq27541-0",
                                                 "CapacityAlertMax", 95,
                                                 "CapacityAlertMin", 5,
                                                 "TempAlertMax",  500,
                                                 "TempAlertMin", 0, NULL);

    g_signal_connect(bq, "buttery_alert", G_CALLBACK (judeg_battery_event), NULL);

    g_timeout_add(1000, battery_test, bq);

    loop =  g_main_loop_new(NULL, TRUE);
    g_main_loop_run(loop);

    return 0;
}