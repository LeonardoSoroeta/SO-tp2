#ifndef __ARCH_MEM_H__
#define __ARCH_MEM_H__

#include <arch/types.h>

typedef struct {
	void *start;
	uint64_t size;
	bool used;
} mem_chunk_t;

typedef struct mem_chunk_list_t {
	mem_chunk_t* chunk;
	struct mem_chunk_list_t* next;
} mem_chunk_list_t;

void pkalloc();

void init_kheap();
void *kmalloc(uint64_t size);
void *kcalloc(uint32_t amount, uint64_t size);
void kfree(void* mem);

#if 0

typedef struct {
	void *start;
	uint64_t size;
	bool used;
} mem_chunk_t;

typedef struct mem_chunk_list_t {
	mem_chunk_t* chunk;
	struct mem_chunk_list_t* next;
} mem_chunk_list_t;

#endif

#endif // __ARCH_MEM_H__