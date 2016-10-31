/* https://github.com/stevej/osdev/blob/master/kernel/ds/list.c */
/* vim: tabstop=4 shiftwidth=4 noexpandtab
 * 
 * General-purpose list implementations.
 */
#include <arch/types.h>
#include <arch/kmem.h>
#include <ds/list.h>

void list_destroy(list_t * list) {
	/* Free all of the contents of a list */
	node_t * n = list->head;
	while (n) {
		kfree(n->value);
		n = n->next;
	}
}

void list_free(list_t * list) {
	/* Free the actual structure of a list */
	node_t * n = list->head;
	while (n) {
		node_t * s = n->next;
		kfree(n);
		n = s;
	}
}

void list_append(list_t * list, node_t * node) {
	node->next = nil;
	/* Insert a node onto the end of a list */
	if (!list->tail) {
		list->head = node;
		node->prev = nil;
	} else {
		list->tail->next = node;
		node->prev = list->tail;
	}
	list->tail = node;
	list->length++;
}

void list_insert(list_t * list, void * item) {
	/* Insert an item into a list */
	node_t * node = kmalloc(sizeof(node_t));
	node->value = item;
	node->next  = nil;
	node->prev  = nil;
	list_append(list, node);
}

void list_append_after(list_t * list, node_t * before, node_t * node) {
	if (!list->tail) {
		list_append(list, node);
		return;
	}
	if (before == nil) {
		node->next = list->head;
		list->head->prev = node;
		list->head = node;
		list->length++;
		return;
	}
	if (before == list->tail) {
		list->tail = node;
	} else {
		before->next->prev = node;
		node->next = before->next;
	}
	node->prev = before;
	before->next = node;
	list->length++;
}

void list_insert_after(list_t * list, node_t * before, void * item) {
	node_t * node = kmalloc(sizeof(node_t));
	node->value = item;
	node->next  = nil;
	node->prev  = nil;
	list_append_after(list, before, node);
}

list_t * list_create() {
	/* Create a fresh list */
	list_t * out = kmalloc(sizeof(list_t));
	out->head = nil;
	out->tail = nil;
	out->length = 0;
	return out;
}

node_t * list_find(list_t * list, void * value) {
	foreach(item, list) {
		if (item->value == value) {
			return item;
		}
	}
	return nil;
}

node_t * list_get(list_t * list, uint64_t index) {
	uint64_t idx = 0;
	foreach(item, list) {
		if (idx == index) {
			return item;
		}
		idx++;
	}
	return nil;
}

void list_remove(list_t * list, uint64_t index) {
	/* remove index from the list */
	if (index > list->length) return;
	uint64_t i = 0;
	node_t * n = list->head;
	while (i < index) {
		n = n->next;
		i++;
	}
	list_delete(list, n);
}

void list_delete(list_t * list, node_t * node) {
	/* remove node from the list */
	if (node == list->head) {
		list->head = node->next;
	}
	if (node == list->tail) {
		list->tail = node->prev;
	}
	if (node->prev) {
		node->prev->next = node->next;
	}
	if (node->next) {
		node->next->prev = node->prev;
	}
	node->prev = nil;
	node->next = nil;
	list->length--;
}

node_t * list_pop(list_t * list) {
	/* Remove and return the last value in the list
	 * If you don't need it, you still probably want to free it!
	 * Try free(list_pop(list)); !
	 * */
	if (!list->tail) return nil;
	node_t * out = list->tail;
	list_delete(list, list->tail);
	return out;
}

node_t * list_dequeue(list_t * list) {
	if (!list->head) return nil;
	node_t * out = list->head;
	list_delete(list, list->head);
	return out;
}

list_t * list_copy(list_t * original) {
	/* Create a new copy of original */
	list_t * out = list_create();
	node_t * node = original->head;
	while (node) {
		list_insert(out, node->value);
	}
	return out;
}

void list_merge(list_t * target, list_t * source) {
	/* Destructively merges source into target */
	if (target->tail) {
		target->tail->next = source->head;
	} else {
		target->head = source->head;
	}
	if (source->tail) {
		target->tail = source->tail;
	}
	target->length += source->length;
	kfree(source);
}
