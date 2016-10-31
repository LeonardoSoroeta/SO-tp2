// https://github.com/bear24rw/EECE4029/tree/master/hw5_vmm

#ifndef __BUDDY_H__
#define __BUDDY_H__

#include <arch/types.h>

uint64_t buddy_init(uint64_t size);
uint64_t buddy_alloc(uint64_t size);
uint64_t buddy_free(uint64_t idx);
uint64_t buddy_size(uint64_t idx);
void buddy_print(void);
void buddy_kill(void);

#endif
