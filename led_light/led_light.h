#ifndef _LED_LIGHT_H
#define _LED_LIGHT_H

#include <glib-object.h>
#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <gpiod.h>

#define LED_TYPE_LIGHT (led_light_get_type())
#define LED_LIGHT(object) G_TYPE_CHECK_INSTANCE_CAST((object), LED_TYPE_LIGHT, LedLight)

typedef struct _LedLight
{
    GObject parent;
}LedLight;

typedef struct _LedLightClass
{
    GObjectClass parent_class;
}LedLightClass;

GType led_light_get_type(void);

typedef enum _LedState
{
    LED_STATE_OFF,
    LED_STATE_ON
}LedState;

typedef enum _LedPattern
{
    LED_PATTERN_NONE,
    LED_PATTERN_BLINK
}LedPattern;

#endif