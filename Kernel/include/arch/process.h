#ifndef __ARCH_PROCESS_H__
#define __ARCH_PROCESS_H__

#include <arch/types.h>
#include <ds/list.h>
#include <arch/kmem.h>

typedef uint64_t pid_t;

typedef void (*sig_t)(int);

#define SIGHOLD 2
#define SIGKILL 9
#define SIGSTOP 17
#define SIGCONT 19

typedef struct {

	uint64_t gs;
	uint64_t fs;
	uint64_t r15;
	uint64_t r14;
	uint64_t r13;
	uint64_t r12;
	uint64_t r11;
	uint64_t r10;
	uint64_t r9;
	uint64_t r8;
	uint64_t rsi;
	uint64_t rdi;
	uint64_t rbp;
	uint64_t rdx;
	uint64_t rcx;
	uint64_t rbx;
	uint64_t rax;
	uint64_t rip;
	uint64_t cs;
	uint64_t eflags;
	uint64_t rsp;
	uint64_t ss;
	uint64_t base;

} registers_t;

typedef enum {
	RUNNING = 0,
	READY,
	WAITING,
	IO,
	ENDED,
	ORPHAN,
	ONHOLD,
	SLEEP
} process_state;

typedef struct process_t {
	
	pid_t pid;
	char *name;

	void *entry_point;

	void *user_registers;
	void *kernel_registers;

	void *user_registers_page;
	void *kernel_registers_page;

	registers_t *registers;

	process_state state;

	struct process_t *parent;
	list_t *children;
	// struct process_t **children;

	pid_t waiting_for;
	int wait_result;

	list_t *fd;

	void *malloc_page;

	mem_chunk_list_t *malloc_node;

	sig_t signal_handler;

} process_t;

typedef struct process_queue_t {
	
	process_t *current;
	struct process_queue_t *next;

	int16_t ttl;
	uint16_t tag;

} process_queue_t;

#endif