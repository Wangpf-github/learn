#ifndef _PUSH_BUTTON_H
#define _PUSH_BUTTON_H

#include <glib-object.h>
#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <gpiod.h>
#include <unistd.h>

#define PUSH_TYPE_BUTTON (push_button_get_type())
#define PUSH_BUTTON(object) G_TYPE_CHECK_INSTANCE_CAST((object), PUSH_TYPE_BUTTON, PushButton)

typedef struct _PushButton
{
    GObject parent;
}PushButton;

typedef struct _PushButtonClass
{
    GObjectClass parent_class;
}PushButtonClass;

typedef enum _ButtonState
{
    BUTTON_STATE_PRESSED,
    BUTTON_STATE_RELEASED
}ButtonState;

typedef enum _ButtonEvent
{
    BUTTON_EVENT_PRESS,
    BUTTON_EVENT_RELEASE
}ButtonEvent;

typedef struct gpiod_line GpiodLine;

GType push_button_get_type();

#endif