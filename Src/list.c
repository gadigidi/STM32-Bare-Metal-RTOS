#include "list.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "stm32f446xx.h"

void list_node_init (node_t *node){
    node->prev = NULL;
    node->next = NULL;
    node->owner = NONE;
}

//Nodes added only at end of list
void list_add_node (list_t *list, node_t *node, uint8_t data, list_owner_t owner){
    node->data = data;
    node->owner = owner;
    if (list->size == 0){ //if new node is first node in list
        list->head = node;
        list->tail = node;
        list->size++;
        return;
    }
    else { //Add new node to tail
        node->prev = list->tail;
        list->tail->next = node;
        list->tail = node;
        list->size++;
        return;
    }
}

//Nodes can be removed from anywhere in the list
uint8_t list_remove_node (list_t *list, node_t *node){
    if ((list->size == 0) || (node->owner == NONE)){
        return 0xFF;
    }
    if (node->owner == NONE){
        return 0xFF;
    }
    else if (list->size == 1){ //Node is only node in list
        list->head = NULL;
        list->tail = NULL;
        list->size--;
        uint8_t data = node->data;
        list_node_init (node);
        return data;
    }
    else if (node == list->head){ //Node is first in list (of 2 or more nodes)
        list->head = node->next;
        node->next->prev = NULL;
        list->size--;
        uint8_t data = node->data;
        list_node_init (node);
        return data;
    }
    else if (node == list->tail){ //Node is last in list (of 2 or more nodes)
        list->tail = node->prev;
        node->prev->next = NULL;
        list->size--;
        uint8_t data = node->data;
        list_node_init (node);
        return data;
    }
    else{ //Node is middle node
        node->next->prev = node->prev;
        node->prev->next = node->next;
        list->size--;
        uint8_t data = node->data;
        list_node_init (node);
        return data;
    }
}

void create_list(list_t *list){
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

