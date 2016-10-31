#ifndef __MOUSE_H__
#define __MOUSE_H__

#include <arch/types.h>

void init_mouse();
error_t add_mouse_listener(listener_t listener);

void mouse_set_visible(bool);
void mouse_set_sensitivity(uint8_t s);

#endif