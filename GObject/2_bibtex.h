#ifndef _2_BIBTEX_H
#define _2_BIBTEX_H

#include <glib-object.h>
#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define KB_TYPE_BIBTEX (kb_bibtex_get_type())
//强制转换，GObject基类转换为本子类
#define KB_BIBTEX(object) G_TYPE_CHECK_INSTANCE_CAST((object), KB_TYPE_BIBTEX, KbBibtex)

typedef struct _KbBibtex
{
    GObject parent;
}KbBibtex;

typedef struct _KbBibtexClass
{
    GObjectClass parent_class;
}KbBibtexClass;

GType kb_bibtex_get_type(void);


#endif