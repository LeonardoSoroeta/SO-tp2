#ifndef __ARCH_HARD_H__
#define __ARCH_HARD_H__

#include <arch/types.h>

extern uint32_t _get_memory_size(void);
extern uint32_t* _get_vga_address(void);

extern uint64_t _get_esp(void);
extern uint64_t _get_ebp(void);
extern uint64_t _get_r9(void);
extern uint64_t _get_r11(void);
extern uint64_t _get_r12(void);

extern void os_dump_regs();

#endif // __ARCH_HARD_H__