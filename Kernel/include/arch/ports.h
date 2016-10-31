#ifndef __ARCH_PORTS__
#define __ARCH_PORTS__

extern int _outport(int value, int port);
extern int _inport(int port);
void _outportb(unsigned short port, unsigned char value);
unsigned char _inportb(unsigned short port);

#endif // __ARCH_PORTS__