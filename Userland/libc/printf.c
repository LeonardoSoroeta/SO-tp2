#include <stdio.h>
#include <stdarg.h>

static int print_str(int fd, char * str);
static int print_dec(int fd, int n);
static int print_oct(int fd, int n);
static int print_hex(int fd, int n);
static int print_chr(int fd, int c);
static char* print_to_base(int value, char * buffer, int base, int* digits);

int printf(char * fmt, ...) {
	va_list ap;
	int written;
	
	va_start(ap, fmt);
	written = vfprintf(STDOUT, fmt, ap);
	va_end(ap);
	return written;
}

int fprintf(int fd, char * fmt, ...) {
	va_list ap;
	int written;
	
	va_start(ap, fmt);
	written = vfprintf(fd, fmt, ap);
	va_end(ap);
	return written;
}

int print_number(int fd, va_list ap, char c) {
	int written = 0;

	switch (c) {
		case 's':
		written = print_str(fd, va_arg(ap, char *));
		break;

		case 'd':
		written = print_dec(fd, va_arg(ap, int));
		break;

		case 'o':
		written = print_oct(fd, va_arg(ap, int));
		break;

		case 'x':
		written = print_hex(fd, va_arg(ap, int));
		break;

		case 'c':
		written = print_chr(fd, va_arg(ap, int));
		break;

		default:
		fputc(fd, c);
		written++;
	}
	return written;
}

int vfprintf(int fd, char * fmt, va_list ap) {
	char c;
	char * ptr = fmt;
	int in_fmt = 0;
	int written = 0;

	while ((c = *ptr++) != (char) 0) {
		if (in_fmt && c == '%') {
			written++;
			in_fmt = 0;
			fputc(fd, c);
		} else if (in_fmt) {
			written += print_number(fd, ap, c);
			in_fmt = 0;
		} else if (c == '%') {
			in_fmt = c;
		} else {
			fputc(fd, c);
			written++;
		}
	}

	return written;
}

static int print_str(int fd, char * str) {
	int count = 0;
	char c;
	while ((c = *str++) != 0) {
		fputc(fd, c);
		count++;
	}
	return count;
}

static int print_chr(int fd, int c) {
	fputc(fd, (char)(c & 0xFF));
	return 1;
}

static int print_dec(int fd, int n) {
	int count;
	char buffer[20] = {0};
	char *str;
	str = print_to_base(n, buffer, 10, &count);
	fputsn(fd, str, count);
	return count;
}

static int print_oct(int fd, int n) {
	int count;
	char buffer[20] = {0};
	char *str;
	str = print_to_base(n, buffer, 8, &count);
	fputsn(fd, str, count);
	return count;
}

static int print_hex(int fd, int n) {
	int count;
	char buffer[20] = {0};
	char *str;
	str = print_to_base(n, buffer, 16, &count);
	fputsn(fd, str, count);
	return count;
}


/* taken from naiveConsole :) */
static char* print_to_base(int value, char * buffer, int base, int *digits) {
	char *p = buffer + 19;
	// char *p1, *p2;
	int d = 0;
	int rem;

	//Calculate characters for each digit
	do {
		rem = value % base;
		*--p = rem + ((rem < 10) ? '0' : 'A' - 10);
		d++;
	} while (value /= base);

	*digits = d;

	return p;
}
