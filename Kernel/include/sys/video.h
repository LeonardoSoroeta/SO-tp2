#ifndef __VIDEO_H__
#define __VIDEO_H__

#include <arch/video.h>
#include <arch/types.h>

void clc();
void clr(uint8_t row);
void move_up();

void set_colour(uint8_t bg, uint8_t fg);
void set_colour_raw(uint8_t colour);
uint8_t get_fg_colour();
uint8_t get_bg_colour();

void unset_cursor(uint8_t x, uint8_t y);
void set_cursor(uint8_t x, uint8_t y);
void set_blink(uint16_t position);

void putcxy(char c, uint8_t x, uint8_t y);
void putsxy(char* c, uint8_t x, uint8_t y);
void putc(char c);
void prints(string s);
void println(string s);
void printf(char*, ...);
//void printf(const char * string, int mod, ...);

void putnum(int64_t i, uint8_t base);
void putbin(int64_t i);
void puti(int64_t i);
void puthex(int64_t i);

uint8_t get_x();
uint8_t get_y();

#endif

