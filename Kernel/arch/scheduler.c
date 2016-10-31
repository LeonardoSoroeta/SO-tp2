/**
 * Tried to implement own idea but it failed.
 * Code is based on WyrmOS https://bitbucket.org/RowDaBoat/wyrm/src/
 */
#include <arch/scheduler.h>
#include <sys/timer.h>
#include <arch/interrupts_handler.h>
#include <arch/kmem.h>
#include <arch/pmem.h>
#include <arch/types.h>
#include <stddef.h>
#include <arch/paging.h>
#include <sys/logging.h>
#include <arch/fs.h>
#include <kernel.h>
#include <lib.h>
#include <sys/video.h>
#include <arch/hard.h>
#include <sys/vga.h>
#include <util/util.h>
#include <arch/fs.h>

#define SWITCH_INTERVAL 10

// static void scheduler_switch();
// static void perform_fork();
static void* build_registers(void*, void*);
static void *to_stack_address(void *page);
// static int on_timer_step(syscall_id, ddword, ddword, ddword, ddword);

/**
 * Last for O(1) enqueing
 */
static process_queue_t *last = NULL;
/**
 * Start of process queue
 */
static process_queue_t *queue = NULL;
// volatile static process_t *current;

/**
 * Current task
 */
static process_queue_t *task = NULL;
static process_queue_t *prev = NULL;

static pid_t next_pid = 1000;


static bool will_fork = no;
// static process_t *forked_process;

void ptasks();

// static int _argc;
// static char **_argv;

void print_registers(registers_t *regs);

void print_registers(registers_t *regs) {

	logf("  r8:%x r9:%x r10:%x r11:%x r12:%x r13:%x r14:%x r15:%x\n", regs->r8, regs->r9, regs->r10, regs->r11, regs->r12, regs->r13, regs->r14, regs->r15);
	logf("  a:%x b:%x c:%x d:%x\n", regs->rax, regs->rbx, regs->rcx, regs->rdx);
	logf("  si:%x di:%x\n", regs->rsi, regs->rdi);
	logf("  sp:%x bp:%x\n", regs->rsp, regs->rbp);
	logf("  ip:%x flags:%x\n", regs->rip, regs->eflags);
	logf("  cs:%x ss:%x fs:%x gs:%x base:%x\n", regs->cs, regs->ss, regs->fs, regs->gs);

}

int signal(sig_t sig) {
	task->current->signal_handler = sig;
	return 0;
}

void default_sig_handler(int sig) {
	logs("Signal: ");
	logi(sig);
	lognl();
}

process_queue_t *current_task() {
	return task;
}

process_t *current_process() {
	return task->current;
}


pid_t prepare_fork(void *entry_point, char *name) {

	process_queue_t* new_task;
	process_t* forkee;
	registers_t *regs;
	process_t *forked;

	will_fork = yes;

	forked = (process_t*)kmalloc(sizeof(process_t));

	forked->pid = ++next_pid;
	forked->entry_point = entry_point;
	forked->name = (char*)kcalloc(strlen(name)+2, sizeof(char*));

	strcpy(forked->name, name);

	forked->waiting_for = 1;

	forkee = task->current;

	logf("FORK: %s(%d) => %s(%d)\n", forkee->name, forkee->pid, forked->name, forked->pid);

	forked->user_registers_page = pmalloc(1);
	forked->kernel_registers_page = pmalloc(1);

	forked->malloc_page = pmalloc(10*4096+1);
	init_heap(forked);

	forked->user_registers = to_stack_address(forked->user_registers_page);
	forked->kernel_registers = to_stack_address(forked->kernel_registers_page);

	forked->user_registers = build_registers(forked->entry_point, forked->user_registers);

	logf("urp:%x krp:%x ur:%x kr:%x mp:%x mn:%x\n", forked->user_registers_page, forked->kernel_registers_page, forked->user_registers, forked->kernel_registers, forked->malloc_page, forked->malloc_node);

	// memcpy((void*)forked->user_registers_page, (void*)forkee->user_registers_page, 0x1000);
	// memcpy((void*)forked->kernel_registers_page, (void*)forkee->kernel_registers_page, 0x1000);

	//  to_stack_address(forked->user_registers_page);
	// forked->user_registers = forked->user_registers_page + (forkee->user_registers - forkee->user_registers_page);
	//  to_stack_address(forked->kernel_registers_page);
	// forked->kernel_registers = forked->kernel_registers_page + (forkee->kernel_registers - forkee->kernel_registers_page);

	regs = (registers_t*)forked->user_registers;
	// forkee_regs = (registers_t*)forkee->user_registers;


	regs->rip = (uint64_t)forked->entry_point;
	// regs->rsp = (uint64_t)forked->user_registers + offsetof(registers_t, rsp);
	// regs->rbp = (uint64_t)forked->user_registers + offsetof(registers_t, rbp);
	// regs->rsp = (uint64_t)forked->user_registers_page + ( _get_r11() + 8*14 - (uint64_t)forkee->user_registers_page );
	// regs->rbp = (uint64_t)forked->user_registers_page + ( _get_r12() + 8*8 - (uint64_t)forkee->user_registers_page );
	regs->rax = 0;

	forked->state = READY;
	forked->signal_handler = &default_sig_handler;
	forked->parent = forkee;
	forked->children = list_create();
	forked->fd = list_create();
	//list_copy(forkee->fd);

	foreach(node, forkee->fd) {
		fopenp( ((fd_t*)node->value)->file->name, forked );
	}

	logs("open fds:");
	foreach(node, forked->fd) {
		logc(' ');
		logi( ((fd_t*)node->value)->fd );
	}
	lognl();



	// fopenp("stdin", forked);
	// fopenp("stdout", forked);
	// fopenp("stderr", forked);
	// fopenp("stdmouse", forked);

	list_insert(task->current->children, (void*)forked);

	new_task = (process_queue_t*)kmalloc(sizeof(process_queue_t));
	new_task->current = forked;
	new_task->next = task->next;
	task->next = new_task;

	ptasks();

	return next_pid;
}

#if 0
static void perform_fork() {

	// int i = 0;
	
	

	

}
#endif

int waitpid(pid_t pid, int *exit_code) {

	task->current->state = WAITING;

	task->current->waiting_for = pid;
	task->current->wait_result = (uintptr_t)exit_code;

	// _cli();


	// logs(">>>>>>>>\n");
	// _sim_interrupt();
	// _sim_interrupt();

	// _sti();

	return 0;
}

int kill(pid_t pid) {

	process_queue_t* start = queue;
	// process_queue_t* aux = queue;

	do {

		if (start->current->pid == pid) {
			start->current->state = ENDED;
			start->current->signal_handler(SIGKILL);
			return 0;
		}

		start = start->next;
	} while (start != queue);

	return 1;

}

int sendsig(pid_t pid, int sig) {

	process_queue_t* start = queue;
	// process_queue_t* aux = queue;

	do {

		if (start->current->pid == pid) {
			start->current->signal_handler(sig);
			return 0;
		}

		start = start->next;
	} while (start != queue);

	return 1;

}

int stop(pid_t pid) {

	process_queue_t* start = queue;

	do {

		if (start->current->pid == pid) {
			if (start->current->state == WAITING) {
				return 2;
			}
			start->current->state = WAITING;
			start->current->signal_handler(SIGSTOP);
			return 0;
		}

		start = start->next;
	} while (start != queue);

	return 1;

}

int hold(pid_t pid) {

	process_queue_t* start = queue;

	do {

		if (start->current->pid == pid) {
			if (start->current->state == ONHOLD) {
				return 2;
			}
			start->current->state = ONHOLD;
			start->current->signal_handler(SIGHOLD);
			return 0;
		}

		start = start->next;
	} while (start != queue);

	return 1;

}

int cont(pid_t pid) {

	process_queue_t* start = queue;
	// process_queue_t* aux = queue;

	do {

		if (start->current->pid == pid) {
			if (start->current->state == RUNNING) {
				return 2;
			}
			start->current->state = RUNNING;
			start->current->signal_handler(SIGCONT);
			return 0;
		}

		start = start->next;
	} while (start != queue);

	return 1;

}

int exit(int code) {

	process_queue_t *start = queue;

	// process_t* process;

	// process = task->current;

	pfree(task->current->user_registers_page);
	pfree(task->current->kernel_registers_page);
	pfree(task->current->malloc_page);

	task->current->state = ENDED;
	if (task->current->parent->state == ENDED) {
		logs("parent ENDED\n");
		// task->current->parent->state = READY;
	}
	task->current->parent->state = READY;

	// logs("parent ");
	// logi(task->current->parent->pid);
	// lognl();

	// logs("proc ");
	// logs(task->current->name);
	// logc(' ');
	// loghex((int64_t)task->current->user_registers);
	// logs(" exit w/code ");
	// logi(code);
	// lognl();


	logs("<exit ");
	logi(code);
	lognl();

	ptasks();

	do {
		start = start->next;
	} while (start->current->waiting_for != task->current->pid && start != queue);

	if (start != queue) {
		start->current->wait_result = code;
	}

	// _cli();

	_sim_interrupt();

	// _sti();

	// _sim_interrupt();

	ptasks();

	
	

	logs("exit>\n");
	// logi(code);
	// lognl();

	return code;

}

pid_t getpid() {
	return task->current->pid;
}

pid_t getppid() {
	return task->current->parent->pid;
}

int getresult() {
	return task->current->wait_result;
}

void ptasks() {
	process_queue_t* start = queue;
	// process_queue_t* aux = queue;

	do {
		logi(start->current->pid);
		logs("->");
		start = start->next;
	} while (start != queue);
	lognl();
}

void ps() {

	process_queue_t *start = queue;
	int aux;

	vstring("A: ACTIVE  W: WAITING\n");
	// vstring("\n");
	vstring("O: ORPHAN  E: ENDED\n");
	// vstring("\n");
	vstring("R: READY   H: HOLD\n");
	// vstring("\n");
	vstring("I: IO      S: SLEEPING\n");
	// vstring("\n");

	vstring("PPID\tPID\tNAME\t\t\t\tSTATUS\tMEMORY\n");
	vstring("-------------------------------------\n");

	do {

		if (start->current->pid == 0) {
			vstring("-   ");
		} else {
			vputi(start->current->parent->pid);
			if (start->current->parent->pid == 0) {
				vstring("   ");
			}
			
		}
		vstring("  ");

		vputi(start->current->pid);
		if (start->current->pid == 0) {
			vstring("   ");
		}
		vchar(' ');

		vstring(start->current->name);

		aux = 12 - strlen(start->current->name) + 3;

		while (--aux) {
			vchar(' ');
		}

		if (start->current->state == WAITING) {
			vchar('W');
		} else if (start->current->state == RUNNING) {
			if (start->current->parent->state == ENDED) {
				vchar('O');
			} else {
				vchar('A');
			}
		} else if (start->current->state == READY) {
			vchar('R');
		} else if (start->current->state == ENDED) {
			vchar('E');
		} else if (start->current->state == ORPHAN) {
			vchar('O');
		} else if (start->current->state == ONHOLD) {
			vchar('H');
		} else if (start->current->state == IO) {
			vchar('I');
		}

		vstring("     ");

		vputi(msize(start->current));

		vstring(" bytes\n");

		start = start->next;
	} while(start != queue);

}

pid_t scheduler_enqueue(void* entry_point, char *name, int argc, char **argv) {

	process_t *new_process;
	process_queue_t *new_task;

	// _cli();

	logs("add ");

	new_process = (process_t*)kmalloc(sizeof(process_t));

	new_process->pid = ++next_pid;
	new_process->name = name;

	new_process->user_registers_page = pmalloc(1);
	new_process->kernel_registers_page = pmalloc(1);

	new_process->malloc_page = pmalloc(3*4096+1);
	init_heap(new_process);

	new_process->entry_point = entry_point;
	new_process->user_registers = to_stack_address(new_process->user_registers_page);
	new_process->kernel_registers = to_stack_address(new_process->kernel_registers_page);

	new_process->user_registers = build_registers(new_process->entry_point, new_process->user_registers);

	loghex((int64_t)new_process->user_registers_page);
	logc(' ');
	loghex((int64_t)new_process->kernel_registers_page);
	logc(' ');
	loghex((int64_t)new_process->user_registers);
	logc(' ');
	loghex((int64_t)new_process->kernel_registers);
	lognl();

	registers_t *regs = (registers_t*)new_process->user_registers;

	regs->rdi = (uint64_t)argc;
	regs->rsi = (uint64_t)argv;

	new_process->state = READY;
	new_process->signal_handler = &default_sig_handler;
	new_process->parent = task->current;
	new_process->children = list_create();
	new_process->fd = list_create();

	fopenp("stdin", new_process);
	fopenp("stdout", new_process);
	fopenp("stderr", new_process);
	fopenp("stdmouse", new_process);

	logs("open fds:");
	foreach(node, new_process->fd) {
		logc(' ');
		logi( ((fd_t*)node->value)->fd );
	}
	lognl();

	new_process->waiting_for = 1;
	new_process->wait_result = 0;

	task->current->state = WAITING;
	task->current->waiting_for = new_process->pid;
	// task->current->children = list_create();
	list_insert(task->current->children, (void*)new_process);

	new_task = (process_queue_t*)kmalloc(sizeof(process_queue_t));
	new_task->next = task->next;
	new_task->current = new_process;
	task->next = new_task;

	// _sti();

	ptasks();

	logs(";;add\n");

	_sim_interrupt();

	return new_process->pid;
}

void init_scheduling() {

	listener_t tt_listener;
	process_t *process;

	logs("init_scheduling\n");

	process = (process_t*)kmalloc(sizeof(process_t));

	process->pid = 0;
	process->name = "kernel.bin";

	process->user_registers_page = pmalloc(1);//(void*)0x1900000;
	process->kernel_registers_page = pmalloc(1);//(void*)0x1906000;

	process->malloc_page = pmalloc(3*4096+1); // 8 KiB
	init_heap(process);

	process->entry_point = (void*)0x120000;
	process->user_registers = to_stack_address(process->user_registers_page);
	process->kernel_registers = to_stack_address(process->kernel_registers_page);

	process->user_registers = build_registers(process->entry_point, process->user_registers);

	loghex((int64_t)process->user_registers_page);
	logc(' ');
	loghex((int64_t)process->kernel_registers_page);
	logc(' ');
	loghex((int64_t)process->user_registers);
	logc(' ');
	loghex((int64_t)process->kernel_registers);
	lognl();

	process->state = RUNNING;
	process->signal_handler = &default_sig_handler;
	process->parent = NULL;
	process->children = list_create();
	process->fd = list_create();

	fopenp("stdin", process);
	fopenp("stdout", process);
	fopenp("stderr", process);
	fopenp("stdmouse", process);

	logs("open fds:");
	foreach(node, process->fd) {
		logc(' ');
		logi( ((fd_t*)node->value)->fd );
	}
	lognl();

	task = (process_queue_t*)kmalloc(sizeof(process_queue_t));

	logs("allocated task\n");

	task->current = process;
	task->next = task;

	queue = task;
	last = task;

	logs("round robin setup\n");
	// ptasks();

	tt_listener = &on_timer_step;
	add_listener(TIMER_TICK, tt_listener);
	logs("listening to ticks\n");
}

static void *to_stack_address(void *page) {
	return (uint8_t*)page + 0x1000 - 0x10;
}

void *current_entry_point() {
	return task->current->entry_point;
}

void scheduler_switch() {

	process_t *process;
	process_queue_t* sleep = queue;

	sleep = task;

	do {

		if (sleep->current->state == SLEEP) {
			logf("%d is asleep ttl %d\n", sleep->current->pid, sleep->ttl);
			sleep->ttl -= TICK_INTERVAL;
			if (sleep->ttl <= 0) {
				sleep->current->state = RUNNING;
			}
		}

		sleep = sleep->next;
	} while (sleep != queue);
	



	// _cli();

	// if (will_fork) {
	// 	perform_fork();
	// 	will_fork = no;
	// }


	// process_queue_t *prev = NULL;
	
	// if (will_fork) {
	// 	logs("> \n");
	// 	will_fork = no;
	// 	return;
	// }

	// task = task->next;

		// logs("&f=");
	// logs(task->current->name);
	// logs(" (");
		// logi(task->current->pid);
	// logs(" ");
		// logc(' ');
		// loghex((int64_t)((registers_t*)task->current->user_registers)->rsp);
	// lognl();

	// logs("> ");
	do {
		if (task->current->state == ENDED) {

			if (prev) {
				logs("end ");
				logi(task->current->pid);
				logc(' ');
				logi(prev->current->pid);
				lognl();
				prev->next = task->next;
			}
		}

		if (task->current->state == IO) {
			task->ttl--;
			if (task->ttl == 0) {
				task->current->state = RUNNING;
			}
		}

		// logi(task->current->pid);
		// logc(' ');
		// logi(task->current->state);
		// logs(" | ");
		prev = task;
		task = task->next;
		// logs(task->current->name);
		// logc(' ');
		// logi(task->current->pid);
		// lognl();
		// if (!task)
			// task = queue;
		// logs(task->current->name);
		// lognl();
	} while (task->current->state == WAITING || task->current->state == ENDED || task->current->state == IO || task->current->state == ONHOLD || task->current->state == SLEEP);

	// lognl();

		// logs(" | t=");
	// logs(task->current->name);
	// logs(" (");
		// logi(task->current->pid);
		// logc(' ');
		// loghex((int64_t)((registers_t*)task->current->user_registers)->rsp);
		// lognl();

	process = task->current;

	if (process->state == READY) {
		process->state = RUNNING;
	}

	// _sti();
}

static void* build_registers(void *entry_point, void *user_stack) {
	registers_t *registers;

	// registers = (registers_t*)kcalloc(1, sizeof(registers_t));
	registers = (registers_t*)user_stack - 1;

	registers->gs =	0x001;
	registers->fs =	0x002;
	registers->r15 =	0x003;
	registers->r14 =	0x004;
	registers->r13 =	0x005;
	registers->r12 =	0x006;
	registers->r11 =	0x007;
	registers->r10 =	0x008;
	registers->r9 =	0x009;
	registers->r8 =	0x00A;
	registers->rsi =	0x00B;
	registers->rdi =	0x00C;
	registers->rbp =	0x00D;
	registers->rdx =	0x00E;
	registers->rcx =	0x00F;
	registers->rbx =	0x010;
	registers->rax =	0x011;
	registers->rip =	(uint64_t)entry_point;
	registers->cs =	0x008;
	registers->eflags = 0x202;
	registers->rsp =	(uint64_t)&(registers->base);
	registers->ss = 	0x000;
	registers->base =	0x000;

	return registers;
}

ddword on_timer_step(syscall_id id, ddword arg1, ddword arg2, ddword arg3, ddword arg4) {

	// if (will_fork) {
	// 	perform_fork();
	// 	will_fork = no;
	// }

	// if (millis_ellapsed % SWITCH_INTERVAL == 0) {
		scheduler_switch();
	// }

	return 0;
}

void *drop_to_kernel(void *rsp) {
	
	if (!task)
		return rsp;

	// logf("%d u2k\n", task->current->pid);
	task->current->user_registers = rsp;

	return task->current->kernel_registers;
	// return getStackBase();
}

void *back_to_user() {
	
	if (!task) {
		return 0;
	}

	// logf("%d k2u\n", task->current->pid);
	// logc(':');
	// loghex((int64_t)task->current->user_registers);
	// lognl();
	return (void*)task->current->user_registers;
}

void *int80_drop_to_kernel() {

	return task->current->kernel_registers;
}

void *int80_back_to_user() {

	return task->current->user_registers;
}



