//export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:usr/lib/x86_64-linux-gnu/pkgconfig
//编译：gcc first.c -o first `pkg-config --cflags --libs gobject-2.0`

#include "1_dlist.h"

/* 
 * 此宏有三个功能：
 * 1，函数GType my_object_get_type (void)的实现，GObject帮我们具体实现
 * 2，注册函数前缀my_object到GObject
 * 3，注册实例结构体名称MyObject到GObject
 * 通过此宏的调用，GObject知道了我们的三个信息：类型，类名称，函数前缀
 */
G_DEFINE_TYPE(MyObject, my_object, G_TYPE_OBJECT);

enum propertyMyObjct
{
    PROPERTY_MYLIST_0,   //用来站位
    PROPERTY_MYLIST_HEAD,
    PROPERTY_MYLIST_HEAD_PREV,
    PROPERTY_MYLIST_HEAD_NEXT,
    PROPERTY_MYLIST_TAIL,
    PROPERTY_MYLIST_TAIL_PREV,
    PROPERTY_MYLIST_TAIL_NEXT
};

static void my_object_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    MyObject *self = MY_OBJECT(object);
    MyObjectPrivate *priv = MY_OBJECT_GET_PRIVATE(self);    //私有属性注册到GObjiect中

    switch (property_id)
    {
    case PROPERTY_MYLIST_HEAD:
        priv->head = g_value_get_pointer(value);
        break;
    case PROPERTY_MYLIST_HEAD_PREV:
        priv->head->prev = g_value_get_pointer(value);
        break;
    case PROPERTY_MYLIST_HEAD_NEXT:
        priv->head->next = g_value_get_pointer(value);
        break;
    case PROPERTY_MYLIST_TAIL:
        priv->tail = g_value_get_pointer(value);
        break;
    case PROPERTY_MYLIST_TAIL_PREV:
        priv->tail->prev = g_value_get_pointer(value);
        break;
    case PROPERTY_MYLIST_TAIL_NEXT:
        priv->tail->next = g_value_get_pointer(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);   //发出一个标准提醒信号
        break;
    }
}

static void my_object_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    MyObject *self = MY_OBJECT(object);
    MyObjectPrivate *priv = MY_OBJECT_GET_PRIVATE(self);    //注册到GObjiect中

    switch (property_id)
    {
    case PROPERTY_MYLIST_HEAD:
        g_value_set_pointer(value, priv->head);
        break;
    case PROPERTY_MYLIST_HEAD_PREV:
        g_value_set_pointer(value, priv->head->prev);
        break;
    case PROPERTY_MYLIST_HEAD_NEXT:
        g_value_set_pointer(value, priv->head->next);
        break;
    case PROPERTY_MYLIST_TAIL:
        g_value_set_pointer(value, priv->tail);
        break;
    case PROPERTY_MYLIST_TAIL_PREV:
        g_value_set_pointer(value, priv->tail->prev);
        break;
    case PROPERTY_MYLIST_TAIL_NEXT:
        g_value_set_pointer(value, priv->tail->next);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);   //发出一个标准提醒信号
        break;
    }
}

static void my_object_init(MyObject *self)    //P:my  T:object
{
    g_print("\tobject struct init\n");
}
  /* 本函数只执行一次 */
static void my_object_class_init(MyObjectClass *class)
{
    g_type_class_add_private(class, sizeof(MyObjectPrivate));
    GObjectClass *base_class = G_OBJECT_CLASS (class);      //将子类强制转换为GObject基类
    base_class->set_property = my_object_set_property;
    base_class->get_property = my_object_get_property;

    GParamSpec *pspec;
    pspec = g_param_spec_pointer ("head", 
                                  "Head node", 
                                  "The head node of the double list", 
                                  G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT);
    g_object_class_install_property (base_class, PROPERTY_MYLIST_HEAD, pspec);
    pspec = g_param_spec_pointer ("head-prev",
                                  "The previous node of the head node",
                                  "The previous node of the head node of the double list",
                                  G_PARAM_READABLE | G_PARAM_WRITABLE);
    g_object_class_install_property (base_class, PROPERTY_MYLIST_HEAD_PREV, pspec);
    pspec = g_param_spec_pointer ("head-next",
                                  "The next node of the head node",
                                  "The next node of the head node of the double list",
                                  G_PARAM_READABLE | G_PARAM_WRITABLE);
    g_object_class_install_property (base_class, PROPERTY_MYLIST_HEAD_NEXT, pspec);
    pspec = g_param_spec_pointer ("tail",
                                  "Tail node",
                                  "The tail node of the double list",
                                  G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT);
    g_object_class_install_property (base_class, PROPERTY_MYLIST_TAIL, pspec);
    pspec = g_param_spec_pointer ("tail-prev",
                                  "The previous node of the tail node",
                                  "The previous node of the tail node of the double list",
                                  G_PARAM_READABLE | G_PARAM_WRITABLE);
    g_object_class_install_property (base_class, PROPERTY_MYLIST_TAIL_PREV, pspec);
    pspec = g_param_spec_pointer ("tail-next",
                                  "The next node of the tail node",
                                  "The next node of the tail node of the double list",
                                  G_PARAM_READABLE | G_PARAM_WRITABLE);
    g_object_class_install_property (base_class, PROPERTY_MYLIST_TAIL_NEXT, pspec);

    g_print("\tclass struct init\n");
}

int main()
{
    g_type_init();  //初始化GObject系统库管理

    MyObject *dlist;
    dlist = g_object_new(MY_TYPE_OBJECT, "head", NULL, "tail", NULL, NULL);

    GValue val = {0};
    g_value_init(&val, G_TYPE_POINTER);
    g_object_get_property(G_OBJECT(dlist), "tail", &val);           //获取tail的属性
    g_object_set_property(G_OBJECT(dlist), "head_prev", &val);      //将获取的属性设置给head_prev, 实现将双向链表首位相连

    g_value_unset(&val);
    g_object_unref(dlist);
    return 0;
}