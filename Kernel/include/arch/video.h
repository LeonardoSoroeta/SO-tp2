#ifndef __ARCH_VIDEO_H__
#define __ARCH_VIDEO_H__

#define VIDEO_START_ADDR 0xB8000

#define BLINK_LOW_PORT		0x3D4
#define BLINK_HIGH_PORT		0x3D5

#define BUFFER_COL 80
#define BUFFER_ROW 25

#define BUFFER_CHAR		0
#define BUFFER_ATTRIB	1

#define COLOR_BLACK					(char)0
#define COLOR_BLUE					(char)1
#define COLOR_GREEN					(char)2
#define COLOR_CYAN					(char)3
#define COLOR_RED						(char)4
#define COLOR_MAGENTA				(char)5
#define COLOR_BROWN					(char)6
#define COLOR_LIGHT_GREY			(char)7
#define COLOR_DARK_GREY				(char)8
#define COLOR_LIGHT_BLUE			(char)9
#define COLOR_LIGHT_GREEN			(char)10
#define COLOR_LIGHT_CYAN			(char)11
#define COLOR_LIGHT_RED				(char)12
#define COLOR_LIGHT_MAGENTA		(char)13
#define COLOR_LIGHT_YELLOW			(char)14
#define COLOR_WHITE					(char)15

#define CHAR_SPACE ' '
#define CHAR_NEWLINE '\n'
#define CHAR_BACKSPACE '\b'

#define MIN_VISIBLE_ASCII 32
#define MAX_VISIBLE_ASCII 126

#endif // __ARCH_VIDEO_H__