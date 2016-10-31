#ifndef __SYSCALLS_H__
#define __SYSCALLS_H__

#include <arch/types.h>
#include <arch/interrupts_handler.h>

#define STDIN		0
#define STDOUT 	1
#define STDERR 	2
#define STDFILE	200
#define STDMOUSE 3

#define REQUEST_CLEAR_SCREEN 0
#define REQUEST_SET_COLOR 1
#define REQUEST_RESET 2
#define REQUEST_HIDE 3
#define REQUEST_SHOW 4
#define REQUEST_SENSITIVTY 5

void init_syscalls();

#endif