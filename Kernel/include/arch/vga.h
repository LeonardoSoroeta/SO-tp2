#ifndef __ARCH_VGA_H__
#define __ARCH_VGA_H__

// http://wiki.osdev.org/Drawing_In_Protected_Mode
#define VGA_WIDTH 1024
#define VGA_HEIGHT 768

#define VGA_DEPTH 24
#define VGA_PITCH VGA_WIDTH*VGA_DEPTH/8
#define VGA_PIXELWIDTH VGA_DEPTH/8

#endif
