//编译：gcc 2_bibtex.c -o 2_bibtex `pkg-config --cflags --libs gobject-2.0 gtk+-3.0`

#include "2_bibtex.h"

G_DEFINE_TYPE(KbBibtex, kb_bibtex, G_TYPE_OBJECT);

#define KB_BIBTEX_GET_PRIVATE(object) G_TYPE_INSTANCE_GET_PRIVATE((object), KB_TYPE_BIBTEX, KbBibtexPrivate)
KbBibtex *entry;
typedef struct _KbBibtexPrivate
{
    GString *title;
    GString *author;
    GString *publisher;
    guint year;
}KbBibtexPrivate;

enum
{
    PROPERTY_0,
    PROPERTY_TITLE,
    PROPERTY_AUTHOR,
    PROPERTY_PUBLISHER,
    PROPERTY_YEAR,
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
        if(priv->publisher)
            g_string_free(priv->publisher, TRUE);
        priv->publisher = g_string_new(g_value_get_string(value));
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
        g_value_set_string(value, priv->publisher->str);
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
    properties[PROPERTY_PUBLISHER] = g_param_spec_string("publisher", "Publisher", "Bibliography publisher", NULL, G_PARAM_READWRITE);
    properties[PROPERTY_YEAR] = g_param_spec_uint("year", "Year", "Bibliography year", 0, G_MAXUINT32, 0, G_PARAM_READWRITE);

    g_object_class_install_properties(base_class, N_PROPERTY, properties);
}

static void kb_bibtex_init(KbBibtex *self)
{

}

void change_title(GtkWidget *widget, gpointer data)
{
    g_object_set(G_OBJECT (entry), "title", "wang", NULL);
}

void title_change(GtkWidget *widget, gpointer data)
{
    g_print("title changed,hahaha!\n");
}

void author_change(GtkWidget *widget, gpointer data)
{
    g_print("author changed!\n");
}

int main(int argc, char *argv[])
{
    g_type_init ();

    entry = g_object_new (KB_TYPE_BIBTEX,
                                        "title", "The {\\TeX}Book",
                                        "author", "Knuth, D. E.",
                                        "publisher", "Addison-Wesley Professional",
                                        "year", 1984,
                                        NULL); 
    gchar *title, *author, *publisher;
    guint year;
    g_object_get (G_OBJECT (entry),
                          "title", &title,
                          "author", &author,
                         "publisher", &publisher,
                          "year", &year,
                         NULL);
    g_print ("    Title: %s\n" "   Author: %s\n" "Publisher: %s\n" "     Year: %d\n", title, author, publisher, year);
    GtkWidget *window;
    GtkWidget *label;
    GtkWidget *button;
    gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_window_set_title(GTK_WINDOW(window), "AnyWhere");
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_window_set_default_size(GTK_WINDOW(window), 128, 64);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    button = gtk_button_new_with_label ("select");
    gtk_container_add(GTK_CONTAINER(window), button);
    g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK(change_title), NULL);

    label = gtk_label_new("For test!");
    gtk_container_add(GTK_CONTAINER(window), label);
    g_signal_connect(entry, "notify::title", G_CALLBACK(title_change), NULL);
//    g_signal_connect(G_OBJECT(window), "notify::", G_CALLBACK(author_change), NULL);
    gtk_widget_show_all(window);
    gtk_main();
    g_free (title);
    g_free (author);
    g_free (publisher);
    g_object_unref (entry);
    return 0;
}