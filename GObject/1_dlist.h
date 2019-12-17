#ifndef FIRST_H_
#define FIRST_H_
/* 使用GOBJECT的好处：
 * 基于引用计数的内存管理
 * 对象的构造函数与析构函数
 * 可设置对象属性的 set/get 函数
 * 易于使用的信号机制 
 */

#include <glib-object.h>
#include <glib.h>
#include <stdio.h>
#include <string.h>

#define MY_TYPE_OBJECT (my_object_get_type())
//将私有成员MyObjectPrivate注册到GObject中，后续通过宏MY_OBJECT_GET_PRIVATE作为获取私有成员变量的唯一入口,如果需要生效，需要在class_init中注册此变量
#define MY_OBJECT_GET_PRIVATE(obj) (\
        G_TYPE_INSTANCE_GET_PRIVATE ((obj), MY_TYPE_OBJECT, MyObjectPrivate))
/*定义强制转换宏，可以将基类GObject转换为本子类*/
#define MY_OBJECT(object) G_TYPE_CHECK_INSTANCE_CAST((object), MY_TYPE_OBJECT, MyObject)

typedef struct _DListnode DListnode;    //双向链表中单个节点结构体
struct _DListnode
{
    /* public */
    DListnode *prev;
    DListnode *next;
    /* private */
    gint data;
};

typedef struct _MyObject   //实例结构体
{
    GObject parent_instance;
}MyObject;

typedef struct _MyObjectClass   //类结构体
{
    GObjectClass parent_class;
}MyObjectClass;

typedef struct _MyObjectPrivate   //私有结构体
{
    DListnode *head;
    DListnode *tail;
}MyObjectPrivate;

GType my_object_get_type(void);
#endif