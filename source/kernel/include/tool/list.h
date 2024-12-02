#ifndef LIST_H
#define LIST_H

#include "comm/types.h"

typedef struct node{
    void *data;
    int order;
    struct node *next;
    struct node *pre;
}node_t;

typedef struct list{
    node_t *first;
    node_t *last;
    int count;
}list_t;

void list_add(list_t *list,node_t *node);
void list_order_add(list_t *list,node_t *node);
node_t *list_poll(list_t *list);
node_t *list_peek(list_t *list);
boot_t list_empty(list_t *list);

#endif
