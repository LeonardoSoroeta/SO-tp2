#include <arch/interrupts_handler.h>
#include <sys/timer.h>
#include <sys/video.h>
#include <arch/ports.h>
#include <arch/scheduler.h>
#include <sys/logging.h>

typedef struct {

	process_queue_t* task;
	time_t eta;
	
} sleeper_t;

// list_t *sleepers;

time_t millis_ellapsed = 0;

static void timer_phase(int hz);
static ddword on_tick(syscall_id id, ddword arg1, ddword arg2, ddword arg3, ddword);

static ddword on_tick(syscall_id id, ddword arg1, ddword arg2, ddword arg3, ddword arg4) {
	// Ignoro los parametros

	// sleeper_t *sleeper;

	// if (a) {
		// logs("tick\n");
	// }

	millis_ellapsed += TICK_INTERVAL;

	// foreach(node, sleepers) {
	// 	sleeper = (sleeper_t*)node->value;
	// 	if (sleeper->eta <= millis_ellapsed) {
	// 		sendsig(sleeper->task->current->pid, 87);

	// 		list_delete(sleepers, node);
	// 		return 0;

	// 	}
	// }

	return 0;
}

/*
 * Set the phase (in hertz) for the Programmable
 * Interrupt Timer (PIT).
 * https://github.com/stevej/osdev/blob/master/kernel/devices/timer.c
 */
static void timer_phase(int hz) {
	int divisor = PIT_SCALE / hz;
	_outportb(PIT_CONTROL, PIT_SET);
	_outportb(PIT_A, divisor & PIT_MASK);
	_outportb(PIT_A, (divisor >> 8) & PIT_MASK);
}

void wait(time_t millis) {

	time_t eta = millis_ellapsed + millis;

	while ( eta > millis_ellapsed) {
		// logs("wait\n");
		// logi(eta-millis_ellapsed);
		// lognl();
	}
}

// void waitp(time_t millis) {

// 	sleeper_t* sleeper;

// 	time_t eta;

// 	eta = millis_ellapsed + millis;

// 	sleeper = (sleeper_t*)kmalloc(sizeof(sleeper_t));

// 	sleeper->eta = eta;
// 	sleeper->task = current_task();

	
// }



void init_timer() {

	listener_t tt_listener;

	// sleepers = list_create();


	timer_phase(TICK_HZ);

	tt_listener = &on_tick;

	add_listener(TIMER_TICK, tt_listener);
}

