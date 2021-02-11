#include <stdint.h>
#include <stdlib.h>

#ifndef LLIST_H
#define LLIST_H

struct linked_node {
    void *value;
    struct linked_node *next;
};

typedef struct ll {
    struct linked_node *head;
    struct linked_node *tail;
    struct linked_node *iter_prev;
    struct linked_node *iter;
    size_t len;
} ll_t;

ll_t *new_ll(void);                  		// Create a new blank list
void del_ll(ll_t *list);             		// Delete list
void ll_add(ll_t *list, void *data); 		// Add value to end of list
void ll_add_start(ll_t *list, void *data); 	// Add value to end of list
size_t ll_len(ll_t *list);           		// Get length of linked list

void ll_iter_rewind(ll_t *list);          // Restart iterator
void *ll_iter_next(ll_t *list);           // Get value at iterator
void ll_remove_iter(ll_t *list);          // Remove at iterator
void ll_add_iter(ll_t *list, void *data); // Add at iterator

#endif