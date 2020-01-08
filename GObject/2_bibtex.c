//编译：gcc 2_bibtex.c -o 2_bibtex `pkg-config --cflags --libs glib-2.0 gobject-2.0`

#include "2_bibtex.h"

G_DEFINE_TYPE(KbBibtex, kb_bibtex, G_TYPE_OBJECT);

#define KB_BIBTEX_GET_PRIVATE(object) G_TYPE_INSTANCE_GET_PRIVATE((object), KB_TYPE_BIBTEX, KbBibtexPrivate)
//G_DEFINE_TYPE_WITH_CODE (PMDList, pm_dlist, G_TYPE_OBJECT, {G_ADD_PRIVATE(PMDList); \
        g_printf("This is in Kbbitex type add private, %d\n", PMDList_private_offset);});

KbBibtex *entry;
typedef struct _KbBibtexPrivate
{
    GString *title;
    GString *author;
    gchar *publisher;
    guint year;
    GList *dlist;
}KbBibtexPrivate;

enum
{
    PROPERTY_0,
    PROPERTY_TITLE,
    PROPERTY_AUTHOR,
    PROPERTY_PUBLISHER,
    
    PROPERTY_YEAR,
    PROPERTY_LIST,
    N_PROPERTY
};

static void kb_bibtex_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    KbBibtex *self = KB_BIBTEX(object);
    KbBibtexPrivate *priv = KB_BIBTEX_GET_PRIVATE(self);

    switch (property_id)
    {
    case PROPERTY_TITLE:
        if(priv->title)
            g_string_free(priv->title, TRUE);
        priv->title = g_string_new(g_value_get_string(value));
        break;
    case PROPERTY_AUTHOR:
        if(priv->author)
            g_string_free(priv->author, TRUE);
        priv->author = g_string_new(g_value_get_string(value));
        break;
    case PROPERTY_PUBLISHER:
        priv->publisher = g_value_get_pointer(value);
        break;
    case PROPERTY_LIST:
        priv->dlist = g_list_copy(g_value_get_pointer(value));
        break;
    case PROPERTY_YEAR:
        priv->year = g_value_get_uint(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void kb_bibtex_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    KbBibtex *self = KB_BIBTEX(object);
    KbBibtexPrivate *priv = KB_BIBTEX_GET_PRIVATE(self);

    switch(property_id)
    {
    case PROPERTY_TITLE:
        g_value_set_string(value, priv->title->str);
        break;
    case PROPERTY_AUTHOR:
        g_value_set_string(value, priv->author->str);
        break;
    case PROPERTY_PUBLISHER:
        g_value_set_pointer(value, priv->publisher);
        break;
    case PROPERTY_LIST:
        g_value_set_pointer(value, priv->dlist);
        break;
    case PROPERTY_YEAR:
        g_value_set_uint(value, priv->year);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void kb_bibtex_class_init(KbBibtexClass *kclass)
{
    g_type_class_add_private(kclass, sizeof(KbBibtexPrivate));

    GObjectClass *base_class = G_OBJECT_CLASS(kclass);
    base_class->get_property = kb_bibtex_get_property;
    base_class->set_property = kb_bibtex_set_property;

    GParamSpec *properties[N_PROPERTY] = {NULL, };
    properties[PROPERTY_TITLE] = g_param_spec_string("title", "Title", "Bibliography", NULL, G_PARAM_READWRITE);
    properties[PROPERTY_AUTHOR] = g_param_spec_string("author", "Author", "Bibliography author", NULL, G_PARAM_READWRITE);
    properties[PROPERTY_PUBLISHER] = g_param_spec_pointer("publisher", "Publisher", "Bibliography publisher",G_PARAM_READWRITE);
    properties[PROPERTY_YEAR] = g_param_spec_uint("year", "Year", "Bibliography year", 0, G_MAXUINT32, 0, G_PARAM_READWRITE);
    properties[PROPERTY_LIST] = g_param_spec_pointer("list", NULL, NULL, G_PARAM_READWRITE);

    g_object_class_install_properties(base_class, N_PROPERTY, properties);
}

static void kb_bibtex_init(KbBibtex *self)
{

}
GList *list1 = NULL;
void change_title()
{
    g_object_set(G_OBJECT (entry), "title", "wang", NULL);
}

gpointer thread_func(gpointer data)
{
    gint num;
    while (1)
    {
        g_print("PLease input num\n");
        scanf("%d", &num);
        switch (num)
        {
        case 0:
            g_object_set(G_OBJECT (entry),"title", "Linux", NULL);
            break;
        case 1:
            g_object_set(G_OBJECT (entry),"author", "Linus", NULL);
            break;
        case 2:
            g_object_set(G_OBJECT (entry),"publisher", "China public", NULL);
            break;
        case 3:
            g_object_set(G_OBJECT (entry),"list", list1, NULL);
            break;

        default:
            break;
        }
    }
}

void print_title()
{
    g_print("title changed,hahaha!\n");
}

void print_author()
{
    g_print("author changed,hahaha!\n");
}
void print_publisher()
{
    g_print("publisher changed,hahaha!\n");
}
void print_list()
{
    g_print("list changed,hahaha!\n");
}

int main(int argc, char *argv[])
{
    g_type_init ();
    
    list1 = g_list_prepend(list1, "111");
    list1 = g_list_append(list1, "222");

    g_print("data is %s\n", (gchar *)g_list_nth_data(list1, 1));

    entry = g_object_new (KB_TYPE_BIBTEX,
                                        "title", "The {\\TeX}Book",
                                        "author", "Knuth, D. E.",
                                        "publisher", "Addison-Wesley Professional",
//                                        "list", list1,
                                        "year", 1984,
                                        NULL); 
    gchar *title, *author, *publisher;
    GList *llist;
    guint year;
    guint my_year = 2018;
    g_object_set(G_OBJECT (entry),"year", my_year, NULL);
    g_object_set(G_OBJECT (entry),"list", list1, NULL);
    g_object_get (G_OBJECT (entry),
                          "title", &title,
                          "author", &author,
                         "publisher", &publisher,
                         "list", &llist,
                          "year", &year,
                         NULL);
    
    g_print ("Title: %s\n" "Author: %s\n" "Publisher: %s\n" "Year: %d\n" "list :%s\n", 
                                                    title, author, publisher, year, (gchar *)g_list_nth_data(llist, 1));

    g_signal_connect(entry, "notify::title", G_CALLBACK(print_title), NULL);
    g_signal_connect(entry, "notify::author", G_CALLBACK(print_author), NULL);
    g_signal_connect(entry, "notify::publisher", G_CALLBACK(print_publisher), NULL);
    g_signal_connect(entry, "notify::list", G_CALLBACK(print_list), NULL);

    GThread *thread = g_thread_new("func", thread_func, NULL);

    GMainLoop *loop =  g_main_loop_new(NULL, TRUE);
    g_main_loop_run(loop);

    g_free (title);
    g_free (author);
    g_free (publisher);
    g_object_unref (entry);
    return 0;
}