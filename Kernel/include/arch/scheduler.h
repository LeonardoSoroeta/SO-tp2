#ifndef __ARCH_SCHEDULER_H__
#define __ARCH_SCHEDULER_H__

#include <arch/process.h>
#include <arch/interrupts_handler.h>
#include <sys/malloc.h>
#include <arch/kmem.h>

void init_scheduling();

pid_t scheduler_enqueue(void *, char *, int, char**);

pid_t prepare_fork(void*, char*);
int waitpid(pid_t pid, int *exit_code);
pid_t getpid(void);
pid_t getppid(void);
int getresult(void);
int exit(int code);
int kill(pid_t);
int stop(pid_t);
int hold(pid_t);
int cont(pid_t);
int sendsig(pid_t, int);
int signal(sig_t);

void ps();

process_queue_t *current_task();
process_t *current_process();

void *current_entry_point(void);

void *drop_to_kernel(void *rsp);
void *back_to_user(void);

void _sim_interrupt(void);
void _ctx_switch(void);

void scheduler_switch(void);

ddword on_timer_step(syscall_id id, ddword arg1, ddword arg2, ddword arg3, ddword arg4);

void *int80_drop_to_kernel();
void *int80_back_to_user();

extern void _start_scheduling(void);

#endif //__ARCH_SCHEDULER_H__
