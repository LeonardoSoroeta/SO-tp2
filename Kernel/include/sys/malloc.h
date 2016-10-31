#ifndef __SYS_MALLOC_H__
#define __SYS_MALLOC_H__

#include <arch/types.h>
#include <arch/kmem.h>
#include <arch/scheduler.h>

void init_heap(process_t*);
uint64_t msize(process_t *process);
void *malloc(process_t *, uint64_t size);
void *calloc(process_t *, uint32_t amount, uint64_t size);
void free(process_t *, void* mem);

#endif // __SYS_MALLOC_H__