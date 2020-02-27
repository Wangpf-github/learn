#include "led_light.h"

LedLight *led1, *led2;

void recive_signal()
{
    printf("Recive signal named 'blink_end'.\n");
}

gboolean led_timeout_test(gpointer param)
{
    printf("------------------\n");
    return TRUE;
}

int main()
{
    struct gpiod_line *gpio_line1;
    struct gpiod_line *gpio_line2;
    struct gpiod_chip *gpio_chip;
    LedState l_state;
    LedPattern l_pattern;
    LedState l_default;
    GMainLoop *loop;

    l_state = LED_STATE_OFF;
    l_pattern = LED_PATTERN_BLINK;
    l_default = LED_STATE_ON;

    gpio_chip = gpiod_chip_open_by_number(19);
    gpio_line1 = gpiod_chip_get_line(gpio_chip, 6);
    gpio_line2 = gpiod_chip_get_line(gpio_chip, 7);
    if (gpio_chip == NULL || gpio_line1 == NULL || gpio_line2 == NULL)
    {
        printf("open gpio line error\n");
    }
    gpiod_line_request_output(gpio_line1, "gpio-test",GPIOD_LINE_ACTIVE_STATE_LOW);
    gpiod_line_set_value(gpio_line1, 0);
    gpiod_line_request_output(gpio_line2, "gpio-test",GPIOD_LINE_ACTIVE_STATE_LOW);
    gpiod_line_set_value(gpio_line2, 0);

    led1 = g_object_new(LED_TYPE_LIGHT, "gpio", gpio_line1,
                                       "state", l_state,
                                       "BlinkDefaultState", l_default,
                                       "BlinkCount", 100,
                                       "BlinkInterval", 1000,
                                       "BlinkDuration", 100,
                                       NULL); 

    led2 = g_object_new(LED_TYPE_LIGHT, "gpio", gpio_line2,
                                       "state", l_state,
                                       "BlinkDefaultState", l_default,
                                       "BlinkCount", 100,
                                       "BlinkInterval", 1000,
                                       "BlinkDuration", 100,
                                       NULL);

    g_signal_connect(led1, "blink_end", G_CALLBACK (recive_signal), NULL);
    g_signal_connect(led2, "blink_end", G_CALLBACK (recive_signal), NULL);

    g_timeout_add(1000, led_timeout_test, NULL);

    g_object_set(G_OBJECT(led1), "pattern", l_pattern, NULL);
    printf("-------thread test------\n");

    loop = g_main_loop_new(NULL, TRUE);
    g_main_loop_run(loop);
#if 0
    sleep(6);

    l_pattern = LED_PATTERN_NONE;
    l_state = LED_STATE_ON;
    g_object_set(G_OBJECT(led1), "state", l_state, "pattern", l_pattern, NULL);
    sleep(9);

    l_state = LED_STATE_OFF;
    g_object_set(G_OBJECT(led1), "state", l_state, NULL);

    sleep(2);

    g_object_set(G_OBJECT(led2),"BlinkCount", 5, "BlinkInterval", 1000, "BlinkDuration", 500, NULL);
    l_default = LED_STATE_ON;
    l_pattern = LED_PATTERN_BLINK;
    g_object_set(G_OBJECT(led2), "BlinkDefaultState", l_default, NULL);
    g_object_set(G_OBJECT(led2), "pattern", l_pattern, NULL);

    sleep(30);
#endif
}