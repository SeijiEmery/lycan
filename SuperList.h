//
//  SuperList.h
//  Lycan
//
//  Created by Seiji Emery on 2/24/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

/*
 A polymorphic super-list that can store any data type
 (implemented via void pointers, so you will need to have
 a mechanism to keep track of the type of your objects
 - either use just one data type (eg. char* or int), or
 use polymorphic data structures that keep track of their
 own type^))
 Implements list, stack, and queue functionality.
 
 Note: this is not the fastest implementation - these lists
 are simply linked series of nodes; iteration is used to 
 find the first/last node 
 
 ^ To do so, make a super class that contains type info,
 and subclass all other types via struct inheiritance.
*/

#ifndef _SUPER_LIST_H_
#define _SUPER_LIST_H_

typedef struct ListNode {
    struct ListNode *prev, *next;
    void *data;
}ListNode;

#define List ListNode

//
// == Constructor/deconstructor methods ==
//

// Create a new list
ListNode *List_new();

// Free an entire list
void List_free(ListNode *container);

//
// == List methods ==
//

// Push some data onto the front of the list, and return the new first node
ListNode *List_fpush(ListNode *list, void* data);

// Push some data onto the end of the list, and return the new last node
ListNode *List_rpush(ListNode *list, void* data);

// Remove the first node from the list and return its contents
void *List_fpop(ListNode *list);

// Remove the last node from the list and return its contents
void *List_rpop(ListNode *list);

// Get the first node of the list
ListNode *List_first(ListNode *list);

// Get the last node of the list
ListNode *List_last(ListNode *list);

//
// == Node methods ==
//

// Get the next node
#define List_next(item) item == NULL ? NULL : item->next
//ListNode *List_next(ListNode *item);

// Get the previous node
#define List_prev(item) item == NULL ? NULL : item->prev
//ListNode *List_prev(ListNode *item);

// Insert some data at an arbitrary position in the list
// (after $prev and before $prev->next)
ListNode *List_insert(ListNode *prev, void *data);

// Remove an arbitrary node from the list and return its contents
void *List_remove(ListNode *item);

#endif