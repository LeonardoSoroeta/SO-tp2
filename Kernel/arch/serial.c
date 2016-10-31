#include <arch/serial.h>
#include <arch/ports.h>
#include <string.h>
#include <arch/types.h>

#define PORT 0x3f8   /* COM1 */

static void install_serial(int device);
static int serial_transmit_empty(int device);

#if 0
static char serial_recv_async(int device);
static char serial_recv(int device);
#endif

static void install_serial(int device) {
	_outportb(device + 1, 0x00); // Disable all interrupts
	_outportb(device + 3, 0x80); // Enable DLAB (set baud rate divisor)
	_outportb(device + 0, 0x03); // Set divisor to 3 (lo byte) 38400 bps
	_outportb(device + 1, 0x00); //                  (hi byte)
	_outportb(device + 3, 0x03); // 8 bits, no parity, one stop bit
	_outportb(device + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
	_outportb(device + 4, 0x0B); // IRQs enabled, RTS/DSR set
}

void init_serial() {

	install_serial(SERIAL_PORT_A);
	install_serial(SERIAL_PORT_B);
}

#if 0

static char serial_recv(int device) {
	while (serial_rcvd(device) == 0) ;
	return _inportb(device);
}

static char serial_recv_async(int device) {
	return _inportb(device);
}

#endif

static int serial_transmit_empty(int device) {
	return _inportb(device + 5) & 0x20;
}

void serial_send(int device, char out) {
	while (serial_transmit_empty(device) == 0);
	_outportb(device, out);
}

void serial_string(int device, char * out) {
	while (*out != 0) {
		serial_send(device, *out);
		out++;
	}
}