#include <stdlib.h>

#include "linked_list.h"

ll_t *new_ll(void) {
    ll_t *list = (ll_t *)malloc(sizeof(ll_t));

    list->head = NULL;
    list->tail = NULL;
    list->iter_prev = NULL;
    list->iter = NULL;
    list->len = 0;

    return list;
}

void del_ll(ll_t *list) {
    list->iter = list->head;
    while (list->iter != NULL) {
        list->head = list->iter;
        list->iter = list->iter->next;
        free(list->head);
    }
    free(list);
}

void ll_add(ll_t *list, void *data) {
    struct linked_node *next = malloc(sizeof(struct linked_node));

    if (list->tail != NULL)
        list->tail->next = next;

    list->tail = next;

    next->value = data;
    next->next = NULL;

    if (list->head == NULL)
        list->head = next;

    list->len++;
}

void ll_add_start(ll_t *list, void *data) {
    struct linked_node *next = malloc(sizeof(struct linked_node));

    next->next = list->head;
    list->head = next;

    if (list->tail == NULL)
        list->tail = next;

    list->len++;
}

size_t ll_len(ll_t *list) {
    return list->len;
}

void ll_iter_rewind(ll_t *list) {
    list->iter_prev = NULL;
    list->iter = list->head;
}

void *ll_iter_next(ll_t *list) {
    if (list->iter != NULL) {
        list->iter_prev = list->iter;
        list->iter = list->iter->next;
        return list->iter_prev->value;
    }
    else {
        return NULL;
    }
}

void ll_remove_iter(ll_t *list) {
    if (list->iter_prev != NULL && list->iter != NULL) {
        list->iter_prev->value = list->iter->value;
        list->iter_prev->next = list->iter->next;
        free(list->iter);
        list->iter = list->iter_prev;
        list->iter_prev = NULL;
        list->len--;
    }
}

void ll_add_iter(ll_t *list, void *data) {
    if (list->iter == NULL) {
        // New or end of list
        ll_add(list, data);
    } else if (list->iter_prev == NULL) {
        // Beginning of existing list
        struct linked_node *next = malloc(sizeof(struct linked_node));
        next->next = list->head;
        next->value = data;
        list->head = next;
        list->len++;
    } else {
        // General Case
        struct linked_node *next = malloc(sizeof(struct linked_node));
        next->next = list->iter;
        next->value = data;
        list->iter_prev->next = next;
        list->iter = next;
        list->len++;
    }
}
