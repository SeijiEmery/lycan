//
//  SuperList.c
//  Lycan
//
//  Created by Seiji Emery on 2/24/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include <stdlib.h>

#include "SuperList.h"

// Create a new list
ListNode *List_new() {
    ListNode *list = malloc(sizeof(ListNode));
    list->next = NULL;
    list->prev = NULL;
    return list;
}

// Free the entire list
void List_free(ListNode *list) {
    ListNode *next;
    while (list != NULL) {
        next = list->next;
        free(list);
        list = next;
    }
    free(list);
}

// Push data onto the front of a list
ListNode *List_fpush(ListNode *list, void* data) {
    ListNode *newNode = malloc(sizeof(ListNode));
    ListNode *first = List_first(list);
    newNode->data = data;
    newNode->prev = NULL;
    newNode->next = first;
    first->prev = newNode;
    return newNode;
}

ListNode *List_rpush(ListNode *list, void* data) {
    ListNode *newNode = malloc(sizeof(ListNode));
    ListNode *last = List_last(list);
    newNode->data = data;
    newNode->prev = last;
    newNode->next = NULL;
    last->next = newNode;
    return newNode;
}
void *List_fpop(ListNode *list) {
    ListNode *first = List_first(list);
    void *data;
    if (first == NULL)
        return NULL;
    if (first->next != NULL)
        first->next->prev = NULL;
    data = first->data;
    free(first);
    return data;
}

void *List_rpop(ListNode *list) {
    ListNode *last = List_last(list);
    void *data;
    if (last == NULL)
        return NULL;
    if (last->prev != NULL)
        last->prev->next = NULL;
    data = last->data;
    free(last);
    return data;
}

ListNode *List_first(ListNode *list) {
    ListNode *first = list;
    if (first == NULL)
        return NULL;
    while (first->prev != NULL)
        first = first->prev;
    return first;
}

ListNode *List_last (ListNode *list) {
    ListNode *last = list;
    if (last == NULL)
        return NULL;
    while (last->next != NULL)
        last = last->next;
    return last;
}

ListNode *List_next(ListNode *list) {
    if (list == NULL)
        return NULL;
    return list->next;
}
ListNode *List_prev(ListNode *list) {
    if (list == NULL)
        return NULL;
    return list->prev;
}

ListNode *List_insert(ListNode *prev, void *data) {
    ListNode *newNode = malloc(sizeof(ListNode));
    newNode->data = data;
    newNode->prev = prev;
    if (prev != NULL) {
        newNode->next = prev->next;
        prev->next->prev = newNode;
        prev->next = newNode;
    }
    else
        newNode->next = NULL;
    return newNode;
}

void* List_remove(ListNode *item) {
    void *data;
    if (item == NULL)
        return NULL;
    if (item->prev != NULL)
        item->prev->next = item->next;
    if (item->next != NULL)
        item->next->prev = item->prev;
    free(item);
    return data;
}

