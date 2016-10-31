/* https://github.com/stevej/osdev/blob/master/kernel/ds/list.h */

/* vim: tabstop=4 shiftwidth=4 noexpandtab
 * 
 * General-purpose list implementations.
 */
#ifndef LIST_H
#define LIST_H

#include <arch/types.h>

typedef struct node {
	struct node * next;
	struct node * prev;
	void * value;
} node_t;

typedef struct {
	node_t * head;
	node_t * tail;
	uint64_t length;
} list_t;

void list_destroy(list_t * list);
void list_free(list_t * list);
void list_append(list_t * list, node_t * item);
void list_insert(list_t * list, void * item);
list_t * list_create();
node_t * list_find(list_t * list, void * value);
node_t * list_get(list_t * list, uint64_t index);
void list_remove(list_t * list, uint64_t index);
void list_delete(list_t * list, node_t * node);
node_t * list_pop(list_t * list);
node_t * list_dequeue(list_t * list);
list_t * list_copy(list_t * original);
void list_merge(list_t * target, list_t * source);

void list_append_after(list_t * list, node_t * before, node_t * node);
void list_insert_after(list_t * list, node_t * before, void * item);

#define foreach(i, list) for (node_t * i = list->head; i != nil; i = i->next)

#endif
