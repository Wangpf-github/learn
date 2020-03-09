//aarch64-buildroot-linux-gnu-gcc led_test.c -o led_test `pkg-config --cflags --libs glib-2.0 gobject-2.0 libgpiod` -I. -L. -lled

#include <led_light.h>

LedLight *led1, *led2;
struct gpiod_line *gpio_line1;
struct gpiod_line *gpio_line2;
struct gpiod_chip *gpio_chip;

void recive_signal1()
{
    printf("Recive signal named 'blink_end' 1.\n");
}

void recive_signal2()
{
    printf("Recive signal named 'blink_end' 2.\n");
}
int i;

gboolean led_timeout_test(gpointer param)
{
    i++;
    if(i == 6)
    {
        g_object_set(G_OBJECT(led1), "pattern", LED_PATTERN_NONE, NULL);
#if 0
        g_object_set(G_OBJECT(led1), "BlinkDefaultState", LED_PATTERN_BLINK,
                                     "BlinkCount", 30,
                                     "BlinkInterval", 400,
                                     "BlinkDuration", 200,
                                     NULL);
#endif
    }
    printf("------------------\n");
    return TRUE;
}

int main()
{
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

    led1 = g_object_new(TYPE_LED_LIGHT, "gpio", gpio_line1,
                                       "state", l_state,
                                       "BlinkDefaultState", l_default,
                                       "BlinkCount", 10,
                                       "BlinkInterval", 1000,
                                       "BlinkDuration", 200,
                                       NULL); 

    led2 = g_object_new(TYPE_LED_LIGHT, "gpio", gpio_line2,
                                       "state", l_state,
                                       "BlinkDefaultState", l_default,
                                       "BlinkCount", 100,
                                       "BlinkInterval", 1000,
                                       "BlinkDuration", 100,
                                       NULL);

    g_signal_connect(led1, "blink-end", G_CALLBACK (recive_signal1), NULL);
    g_signal_connect(led2, "blink-end", G_CALLBACK (recive_signal2), NULL);

    g_timeout_add(1000, led_timeout_test, NULL);

    g_object_set(G_OBJECT(led1), "pattern", l_pattern, NULL);

    loop = g_main_loop_new(NULL, TRUE);
    g_main_loop_run(loop);

}