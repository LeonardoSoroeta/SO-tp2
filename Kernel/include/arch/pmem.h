#ifndef __SYS_MEM_H__
#define __SYS_MEM_H__

uint64_t fit_align_4k(uint64_t size);

void* pmalloc(uint64_t size);
void* pcalloc(uint32_t amount, uint64_t size);
void pfree(void *ptr);
void init_mem(void);

#endif // __SYS_MEM_H__