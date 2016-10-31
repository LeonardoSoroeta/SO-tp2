#ifndef __SYS_VGA_H__
#define __SYS_VGA_H__

#include <arch/vga.h>
#include <arch/types.h>

#define COLOR_RGB(r, g, b) (((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 255))
#define COLOR_R(color) ((color >> 16) & 255)
#define COLOR_G(color) ((color >> 8) & 255)
#define COLOR_B(color) ((color >> 0) & 255)

#define ATTR_R 0
#define ATTR_G 1
#define ATTR_B 2

#define INIT_X 8
#define INIT_Y 10

#define MAXH ((VGA_WIDTH-2*INIT_X)/2)
#define MAXV (VGA_HEIGHT-2*INIT_Y)

#define MAXCHARH ((VGA_WIDTH-2*INIT_X)/(CHAR_WIDTH+CHAR_SPACING))
#define MAXCHARV ((VGA_HEIGHT-2*INIT_Y)/(CHAR_HEIGHT+LINE_SPACING))

#define CHAR_HEIGHT 8
#define CHAR_WIDTH 8

#define LINE_SPACING 4
#define CHAR_SPACING 0

#define LINES(n) (n*(CHAR_HEIGHT+LINE_SPACING))
#define CHARS(n) (n*(CHAR_WIDTH+CHAR_SPACING))

#define BLACK_COLOR 0x000000
#define WHITE_COLOR 0xffffff
#define RED_COLOR 0x0000ff
#define GREEN_COLOR 0x00ff00
#define BLUE_COLOR 0xff0000

void init_vga();
unsigned char* get_buffer();

void vclc();
void vclcmid();
uint32_t vgetcolor(uint16_t x, uint16_t y);
void vputpixel(uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b);
void vputpixelc(uint16_t x, uint16_t y, uint32_t color);
// Geometry
void vrectfill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color);
void vrect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color);
void vline(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color);
void vellipse(uint16_t, uint16_t, uint16_t, uint16_t, uint32_t);
void vellipsefill(uint16_t xc, uint16_t yc, uint16_t w, uint16_t h, uint32_t color);
void vcircle(uint16_t h, uint16_t k, uint16_t r, uint32_t color);
void vcirclefill(uint16_t h, uint16_t k, uint16_t r, uint32_t color);
void vpoly(uint16_t xc, uint16_t yc, uint8_t sides, uint16_t r, uint32_t color);

void vclc();
void vsetbgcolor(uint32_t color);
void vsetfgcolor(uint32_t color);
// Text
void vcharxy(uint16_t x, uint16_t y, unsigned char c);
void vchar(unsigned char c);
void vstring(char *str);
void vstringxy(uint16_t x, uint16_t y, char* str);

void vcopy(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t x2, uint16_t y2);

void vnl();
void vclc();
void vclr(uint16_t row);
void vmove_up();

void vputnum(int64_t i, uint8_t base);
void vputbin(int64_t i);
void vputi(int64_t i);
void vputhex(int64_t i);

void vset_cursor(uint16_t x, uint16_t y);
void vunset_cursor(uint16_t x, uint16_t y);

uint32_t* take_screenshot();

void vprintf(char * fmt, ...);

#endif // __SYS_VGA_H__