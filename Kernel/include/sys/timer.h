#ifndef __TIMER_H__
#define __TIMER_H__

#include <arch/types.h>

#define TICK_HZ 500
#define TICK_INTERVAL (1000/TICK_HZ)

#define PIT_A 0x40
#define PIT_B 0x41
#define PIT_C 0x42
#define PIT_CONTROL 0x43

#define PIT_MASK 0xFF
#define PIT_SCALE 1193180
#define PIT_SET 0x36

extern time_t millis_ellapsed;

void wait(time_t millis);
// void waitp(time_t millis);
void init_timer();

#endif