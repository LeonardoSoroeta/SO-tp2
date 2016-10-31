#include <arch/kmem.h>
#include <arch/hard.h>
#include <string.h>
#include <arch/types.h>
#include <sys/video.h>
#include <sys/logging.h>
#include <arch/scheduler.h>

void print_malloc(process_t* process);

static inline int64_t abs(int64_t n);

static inline int64_t abs(int64_t n) {
  return n > 0 ? n : -n;
}

uint64_t msize(process_t *process) {

	mem_chunk_list_t* curr;
	uint64_t size = 0;

	curr = process->malloc_node;

	while (curr != NULL) {
		size += curr->chunk->size;
		curr = curr->next;
	}

	return size - 40;

}

void* malloc(process_t *process, uint64_t size) {

	mem_chunk_t *chunk;
	mem_chunk_list_t* curr, *prev;

	logf("malloc(%d)::allocating %d bytes\n", process->pid, size);

	curr = process->malloc_node;
	prev = NULL;

	while (curr != NULL && (curr->chunk->used || curr->chunk->size < size)) {
		prev = curr;
		curr = curr->next;
	}

	if (curr) {
		// encajar en el diome

		logs("diome\n");

		curr->chunk->size = size;
		curr->chunk->used = yes;

		return curr->chunk->start;
	}

	// if ( (uint64_t)prev->chunk->start - size < TOTAL_MEMORY - AVAILABLE_MEMORY ) {
	if ( (uint64_t)prev->chunk->start + size > ( (uint64_t)process->malloc_page + 0x3000 ) ) {
		// no more memory
		logs("NO MORE MEMORY ");
		loghex((int64_t)prev->chunk->start);
		logc(' ');
		loghex((int64_t)process->malloc_page);
		lognl();
		return NULL;
	}

	logf("malloc(%d)::p0:%x ps:%d\n", process->pid, (uint64_t)prev->chunk->start, prev->chunk->size);

	// chunk = (mem_chunk_t*)(prev->chunk->start - sizeof(mem_chunk_t));
	chunk = (mem_chunk_t*)(prev->chunk->start + prev->chunk->size);

	logf("malloc(%d)::chunk=%x\n", process->pid, (uint64_t)chunk);

	memset(chunk, 0, sizeof(mem_chunk_t));
	// chunk->start = (void*)(chunk - sizeof(mem_chunk_list_t) - size);
	chunk->start = (void*)( ((uint64_t)chunk) + sizeof(mem_chunk_t) + sizeof(mem_chunk_list_t));
	chunk->size = size;
	chunk->used = yes;

	// curr = (mem_chunk_list_t*)(chunk - sizeof(mem_chunk_list_t));
	curr = (mem_chunk_list_t*)( ((uint64_t)chunk) + sizeof(mem_chunk_t));
	memset(curr, 0, sizeof(mem_chunk_list_t));
	curr->chunk = chunk;
	curr->next = NULL;

	prev->next = curr;

	logf("malloc(%d):: start:%x+%d\n", process->pid, (uint64_t)chunk->start, size);

	return chunk->start;
}

void *calloc(process_t *process, uint32_t amount, uint64_t size) {
	// kiss
	void *mem;
	if ( (mem = malloc(process, size*amount))  ) {
		memset(mem, 0, size*amount);
	}
	return mem;
}

void free(process_t* process, void* mem) {

	mem_chunk_list_t* curr, *prev;
	bool append_to_prev = no;

	logf("malloc(%d)::", process->pid);
	print_malloc(process);

	prev = NULL;
	curr = process->malloc_node;

	while (curr != NULL && abs(mem - curr->chunk->start) > curr->chunk->size) {
		prev = curr;
		curr = curr->next;
	}

	if (curr != NULL) {
		curr->chunk->used = no;
		if (prev && !prev->chunk->used) {
			prev->chunk->size += curr->chunk->size;
			prev->next = curr->next;
			append_to_prev = yes;
		}
		if (curr->next && !curr->next->chunk->used) {
			if (append_to_prev) {
				prev->chunk->size += curr->next->chunk->size;
				prev->next = curr->next->next;
			} else {
				curr->chunk->size += curr->next->chunk->size;
				curr->next = curr->next->next;
			}
		}
	}
	logf("malloc(%d)::", process->pid);
	print_malloc(process);
}

void print_malloc(process_t* process) {

	mem_chunk_list_t* curr = process->malloc_node;

	while (curr != NULL) {
		if (curr->chunk->used) {
			loghex((int64_t)curr->chunk->start);
			logc(':');
			logi(curr->chunk->size);
		} else {
			logs("E:");
			logi(curr->chunk->size);
		}
		logs("->");
		curr = curr->next;
	}
	logc('\n');
}

void init_heap(process_t *process) {

	mem_chunk_t *chunk;
	uint16_t size;

	logf("Setting up %s's heap.\nAvailabe Memory: 0x3000 from %x to %x\n", process->name, (uint64_t)process->malloc_page, (uint64_t)(process->malloc_page+0x3000-1));

	// logs("Setting up ");
	// logs(process->name);
	// logs(" heap.\nAvailabe Memory: ");
	// loghex(0x3000);
	// logs(" from ");
	// loghex((int64_t)process->malloc_page);
	// logs(" to ");
	// loghex((int64_t)(process->malloc_page+0x3000-1));
	// lognl();

	size = sizeof(mem_chunk_t) + sizeof(mem_chunk_list_t);

	// chunk = (mem_chunk_t*)(long int)(TOTAL_MEMORY - size);
	chunk = (mem_chunk_t*)process->malloc_page;
	memset(chunk, 0, size);

	chunk->start = (void*)chunk;
	chunk->used = yes;
	chunk->size = size;

	// allocated = (void*)(TOTAL_MEMORY - sizeof(mem_chunk_list_t));
	process->malloc_node = (void*)(process->malloc_page + sizeof(mem_chunk_t));
	process->malloc_node->next = NULL;
	process->malloc_node->chunk = chunk;

	logf("malloc(%d)::chunk %d bytes\n", process->pid, size);

}
