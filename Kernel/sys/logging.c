#include <sys/logging.h>
#include <arch/serial.h>
#include <util/util.h>
#include <stdarg.h>

#define DEFAULT_DEVICE SERIAL_PORT_A

static void log_number(va_list ap, char c);
static void logvfprintf(char * fmt, va_list ap);

void logi(int64_t i) {
	serial_string(DEFAULT_DEVICE, itoa10(i));
}

void loghex(int64_t i) {
	serial_string(DEFAULT_DEVICE, "0x");
	serial_string(DEFAULT_DEVICE, itoa16(i));
}

void logd(double d) {

}

void logc(char c) {
	serial_send(DEFAULT_DEVICE, c);
}

void logs(char* s) {
	serial_string(DEFAULT_DEVICE, s);
}

void lognl() {
	serial_send(DEFAULT_DEVICE, '\n');
}

void logf(char * fmt, ...) {
	va_list ap;
	
	va_start(ap, fmt);
	logvfprintf(fmt, ap);
	va_end(ap);
}

static void log_number(va_list ap, char c) {

	switch (c) {
		case 's':
			logs(va_arg(ap, char *));
			break;

		case 'd':
			logi(va_arg(ap, int));
			break;

		case 'o':
			loghex(va_arg(ap, int));
			break;

		case 'x':
			loghex(va_arg(ap, int));
			break;

		case 'c':
			logc(va_arg(ap, int));
			break;

		default:
			logc(c);
		
	}
}

static void logvfprintf(char * fmt, va_list ap) {
	char c;
	char * ptr = fmt;
	int in_fmt = 0;

	while ((c = *ptr++) != (char) 0) {
		if (in_fmt && c == '%') {
			in_fmt = 0;
			logc(c);
		} else if (in_fmt) {
			log_number(ap, c);
			in_fmt = 0;
		} else if (c == '%') {
			in_fmt = c;
		} else {
			logc(c);
		}
	}
}