//编译：aarch64-buildroot-linux-gnu-gcc button_test.c -o button_test -lpthread `pkg-config --cflags --libs glib-2.0 gobject-2.0 libgpiod` -I. -L. -lpushbutton
///home/wang/hi3559av_buitroot/buildroot-env/output/build/libgpiod-1.4.1/include
#include <push_button.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

PushButton *bu;
struct gpiod_line *gpio_led;
int i = 0;

void judeg_button_event(PushButton *self, ButtonEvent event, gint release, gpointer data)
{
    if (event == BUTTON_EVENT_RELEASE)
    {
        printf("release timeout count is %d\n", release);
        gpiod_line_set_value(gpio_led, 0);
    }
    else if(event == BUTTON_EVENT_PRESS)
    {
        printf("press timeout count is %d\n", release);
        gpiod_line_set_value(gpio_led, 1);
    }
    
}

gboolean button_test(gpointer param)
{
    if(i++ == 10)
    {
        g_object_unref(param);
        return FALSE;
    }

    printf("button test -- %d!\n", i);
    return TRUE;
}

int main()
{
    struct gpiod_line *gpio_button;
    struct gpiod_chip *gpio_chip;
    GMainLoop *loop;

    gpio_chip = gpiod_chip_open_by_number(19);
    gpio_button = gpiod_chip_get_line(gpio_chip, 5);   
    gpio_led = gpiod_chip_get_line(gpio_chip, 7);
    if (gpio_chip == NULL || gpio_button == NULL || gpio_led == NULL)
    {
        printf("open gpio line error\n");
    }

    gpiod_line_request_output(gpio_led, "gpio-test2", GPIOD_LINE_ACTIVE_STATE_LOW);

    bu = g_object_new(TYPE_PUSH_BUTTON, "gpio", gpio_button, 
                                        "ReleaseTimeout", 1,
                                        "PressTimeout", 1,
                                        NULL);

    g_signal_connect(bu, "button-event", G_CALLBACK (judeg_button_event), NULL);

    g_timeout_add(1000, button_test, bu);

    loop = g_main_loop_new(NULL, TRUE);
    g_main_loop_run(loop);
}