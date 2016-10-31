#include <arch/kmem.h>
#include <arch/hard.h>
#include <string.h>
#include <arch/types.h>
#include <sys/video.h>
#include <sys/logging.h>

/*
 *
 *   Heap grows from the bottom
 *   starting at 0x40000000
 *   (nearly 1GiB)
 *
 *   
 * 
 * 
 */

#define TOTAL_MEMORY (_get_memory_size() * 1024 * 1024)
// #define AVAILABLE_MEMORY 0x8000000 // 128 MiB for Kernel RAM heap
#define AVAILABLE_MEMORY 0x300000 // 3 MiB for Kernel RAM heap
// #define MEMORY_START TOTAL_MEMORY - AVAILABLE_MEMORY
#define MEMORY_START 0xD00000 // 12th Mib
// #define MEMORY_END TOTAL_MEMORY - 1
#define MEMORY_END MEMORY_START + AVAILABLE_MEMORY - 1

static mem_chunk_list_t* allocated = NULL;

static inline int64_t abs(int64_t n);

static inline int64_t abs(int64_t n) {
  return n > 0 ? n : -n;
}

void pkalloc() {

	mem_chunk_list_t* curr = allocated;

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

void* kmalloc(uint64_t size) {

	mem_chunk_t *chunk;
	mem_chunk_list_t* curr, *prev;

	// logs("kmalloc:: allocating ");
	// logi(size);
	// logs(" bytes");
	// lognl();

	curr = allocated;
	prev = NULL;

	while (curr != NULL && (curr->chunk->used || curr->chunk->size < size)) {
		prev = curr;
		curr = curr->next;
	}

	if (curr) {
		// encajar en el diome

		curr->chunk->size = size;
		curr->chunk->used = yes;

		return curr->chunk->start;
	}

	// if ( (uint64_t)prev->chunk->start - size < TOTAL_MEMORY - AVAILABLE_MEMORY ) {
	if ( (uint64_t)prev->chunk->start + size > MEMORY_END ) {
		// no more memory
		return NULL;
	}

	// chunk = (mem_chunk_t*)(prev->chunk->start - sizeof(mem_chunk_t));
	chunk = (mem_chunk_t*)(prev->chunk->start + prev->chunk->size);

	memset(chunk, 0, sizeof(mem_chunk_t));
	// chunk->start = (void*)(chunk - sizeof(mem_chunk_list_t) - size);
	chunk->start = (void*)(((uint64_t)chunk) + sizeof(mem_chunk_t) + sizeof(mem_chunk_list_t));
	chunk->size = size;
	chunk->used = yes;

	// curr = (mem_chunk_list_t*)(chunk - sizeof(mem_chunk_list_t));
	curr = (mem_chunk_list_t*)( ((uint64_t)chunk) + sizeof(mem_chunk_t));
	memset(curr, 0, sizeof(mem_chunk_list_t));
	curr->chunk = chunk;
	curr->next = NULL;

	prev->next = curr;

	return chunk->start;
}

void *kcalloc(uint32_t amount, uint64_t size) {
	// kiss
	void *mem;
	if ( (mem = kmalloc(size*amount))  ) {
		memset(mem, 0, size*amount);
	}
	return mem;
}

void kfree(void* mem) {

	mem_chunk_list_t* curr, *prev;
	bool append_to_prev = no;

	prev = NULL;
	curr = allocated;

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
}

void init_kheap() {

	mem_chunk_t *chunk;
	uint16_t size;

	logs("Setting up Kernel Heap\n");
	logs("Availabe Memory: ");
	loghex(AVAILABLE_MEMORY);
	logs(" starting at ");
	loghex(MEMORY_START);
	logs(" ending at ");
	loghex(MEMORY_END);
	lognl();

	size = sizeof(mem_chunk_t) + sizeof(mem_chunk_list_t);

	// chunk = (mem_chunk_t*)(long int)(TOTAL_MEMORY - size);
	chunk = (mem_chunk_t*)(long int)(MEMORY_START);
	memset(chunk, 0, size);

	chunk->start = (void*)chunk;
	chunk->used = yes;
	chunk->size = size;

	// allocated = (void*)(TOTAL_MEMORY - sizeof(mem_chunk_list_t));
	allocated = (void*)(MEMORY_START + sizeof(mem_chunk_t));
	allocated->next = NULL;
	allocated->chunk = chunk;

}
