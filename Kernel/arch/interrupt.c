#include <arch/interrupts_handler.h>
#include <arch/idt.h>

#include <arch/arch.h>
#include <sys/video.h>

#include <sys/timer.h>
#include <arch/hard.h>

#include <sys/logging.h>

#include <arch/hard.h>

#include <arch/scheduler.h>
#include <sys/sound.h>

extern void _load_handler(byte entry, ddword handler, ddword idt_start);

// Handlers
extern void _keyboard_interrupt_handler();
extern void _mouse_interrupt_handler();
extern void _backup_interrupt_handler();
extern void _syscall_interrupt_handler();
extern void _tt_interrupt_handler();
extern void _page_fault_handler();
extern void _gpf_handler();

static listener_pool_t listeners_map[MOUSE - KEYBOARD + 1];

void init_interrupts(void) {

	uint8_t i = 0;

	/**
	 *
	 *	Cargo las rutinas de atencion de interrupcion
	 * 
	 */

 	idt_set_handler(0x0D, (ddword)&_gpf_handler);
	idt_set_handler(0x0E, (ddword)&_page_fault_handler);
	idt_set_handler(0x20, (ddword)&_tt_interrupt_handler);
	idt_set_handler(0x21, (ddword)&_keyboard_interrupt_handler);
	idt_set_handler(0x79, (ddword)&_backup_interrupt_handler);
	idt_set_handler(0x80, (ddword)&_syscall_interrupt_handler);
  idt_set_handler(0x2c, (ddword)&_mouse_interrupt_handler);
  
	for (; i < MOUSE - KEYBOARD; i++) {
		listeners_map[i].size = 0;
	}

	return;
}

error_t add_listener(interrupt_id id, listener_t listener) {

	if ( listeners_map[id].size >= MAX_LISTENERS_ALLOWED ) {
		return 1;
	}

	listeners_map[id].listeners[listeners_map[id].size++] = listener;

	return 0;
}

uint64_t on_interrupt(interrupt_id id, ddword sid, ddword arg1, ddword arg2, ddword arg3, ddword arg4) {

	uint8_t i = 0;
	uint64_t res = 0;

	for (; i < listeners_map[id].size; i++) {
		res = listeners_map[id].listeners[i]((syscall_id)sid, arg1, arg2, arg3, arg4);
	}

	return res;
}

void on_timer_tick(ddword id, ddword arg1, ddword arg2, ddword arg3, ddword arg4) {

	on_interrupt(TIMER_TICK, id, arg1, arg2, arg3, arg4);
}

void on_keyboard(ddword id, ddword arg1, ddword arg2, ddword arg3, ddword arg4) {

	on_interrupt(KEYBOARD, id, arg1, arg2, arg3, arg4);
}

void on_mouse(ddword id, ddword arg1, ddword arg2, ddword arg3, ddword arg4) {
	on_interrupt(MOUSE, id, arg1, arg2, arg3, arg4);
}

void on_gpf(ddword rdi, ddword rsi, ddword rdx, ddword r10, ddword r8, ddword r9) {

	uint64_t r11 = _get_r11();
	uint64_t r12 = _get_r12();
	// uint64_t rax = _get_rax();

	logs("general protection fault handler\n");

	logf("\n r11:%x r12:%x rdi:%x rsi:%x rdx:%x r10:%x r8:%x r9:%x\n", r11, r12, rdi, rsi, rdx, r10, r8, r9);

	ps();

	pause();	

}

static char* syscalls[] = {"READ", "WRITE", "OPTS", "GET_TIME", "SET_TIME", "BEEP", "PLAY_SOUND", "EVENT", "WAIT", "SHUTDOWN", "HALT", "FORK", "GETPID", "GETPPID", "GETRESULT", "WAITPID", "INIT_PROC", "VIDMODE", "MALLOC", "CALLOC", "FREE", "PS", "KILL", "STOP", "HOLD", "CONT", "SIGNAL", "BEGINCTX", "MKFIFO", "LS", "FOPEN", "FSIZE", "SIZE", "FCLOSE", "CLOSE", "PIPE", "DUP2", "FSEEK", "SEEK", "SCREENSHOT", "GETPNAME", "FSTART", "START", "YIELD", "EXIT"};

uint64_t on_syscall(ddword id, ddword rsi, ddword rdx, ddword r10, ddword r8) {

	// id -> rdi

	logs("task: ");
	logs(current_process()->name);
	logs(" rsp: ");
	loghex(_get_esp());
	logs(" rbp: ");
	loghex(_get_ebp());
	logs(" rdi: ");
	logs(syscalls[id]);
	// loghex((int64_t)id);
	logs(" rsi: ");
	loghex((int64_t)rsi);
	logs(" rdx: ");
	loghex((int64_t)rdx);
	logs(" r10: ");
	loghex((int64_t)r10);
	logs(" r8: ");
	loghex((int64_t)r8);
	lognl();

	return on_interrupt(SYSCALL, id, rsi, rdx, r10, r8);
}

