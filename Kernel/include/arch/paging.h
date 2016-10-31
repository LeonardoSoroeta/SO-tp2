#ifndef __ARCH_PAGING_H__
#define __ARCH_PAGING_H__

#include <arch/types.h>

void init_paging(void);

void on_page_fault(uint64_t cr2, uint64_t, uint64_t);

void give_new_page();

#endif // __ARCH_PAGING_H__