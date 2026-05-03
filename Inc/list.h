#ifndef LIST_H_
#define LIST_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    NONE, SLEEPING_LIST, READY_LIST, WAIT_LIST,
} list_owner_t;

typedef struct node_t {
    uint8_t data;
    struct node_t *prev;
    struct node_t *next;
    list_owner_t owner;
} node_t;

typedef struct {
    node_t *head;
    node_t *tail;
    uint32_t size;
} list_t;


void list_node_init (node_t *node);
void list_add_node (list_t *list, node_t *node, uint8_t data, list_owner_t owner);
uint8_t list_remove_node (list_t *list, node_t *node);
void create_list (list_t *list);

#endif /* LIST_H_ */
