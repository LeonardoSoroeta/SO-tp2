#ifndef __SOUND_H__
#define __SOUND_H__

#include <arch/types.h>
#include <arch/arch.h>

void play(uint32_t);
void pause();
void beep();
void beeplf(unsigned int length, unsigned int freq);

void init_sound();

#endif