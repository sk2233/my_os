#include "tool/list.h"

void list_add(list_t *list,node_t *node){
    if(list->count>0){
        list->last->next=node;
        node->pre=list->last;
        list->last=node;
    } else{
        list->last=list->first=node;
    }
    list->count++;
}

void list_order_add(list_t *list,node_t *node){
    if(list->count>0){
        node_t *temp=list->first;
        while (temp!=NULL&&temp->order<node->order){
            temp=temp->next;
        }
        if(temp!=NULL){
            if(temp==list->first){
                list->first->pre=node;
                node->next= list->first;
                list->first=node;
            } else{
                node->next=temp;
                node->pre=temp->pre;
                node->next->pre=node;
                node->pre->next=node;
            }
        } else{ // 到达末尾了
            list->last->next=node;
            node->pre=list->last;
            list->last=node;
        }
    } else{
        list->last=list->first=node;
    }
    list->count++;
}

node_t *list_poll(list_t *list){
    node_t *res=list->first;
    list->first=list->first->next;
    res->next=NULL;
    list->first->pre=NULL;
    list->count--;
    return res;
}

node_t *list_peek(list_t *list){
    return list->first;
}

boot_t list_empty(list_t *list){
    return list->count==0;
}