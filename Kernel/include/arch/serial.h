#ifndef __ARCH_SERIAL_H__
#define __ARCH_SERIAL_H__

#define SERIAL_PORT_A 0x3F8
#define SERIAL_PORT_B 0x2F8

void init_serial();

void serial_send(int device, char out);
void serial_string(int device, char * out);

#endif // __ARCH_SERIAL_H__