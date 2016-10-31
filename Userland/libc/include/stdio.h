#ifndef __STD_IO_H__
#define __STD_IO_H__

#define STDIN 	0
#define STDOUT 	1
#define STDERR 	2
#define STDFILE	200
#define STDMOUSE 3
#define SCANF_MAX_STR_BUFFER 64
#include <stdarg.h>

#define EMPTY 0xFF
#define EOF EMPTY

void putc(char c);
void fputc(int fd, char c);

int puts(char *);
int fputs(int fd, char *);
int fputsn(int fd, char *, int n);

int vfprintf(int fd, char * fmt, va_list ap);
int fprintf(int fd, char *fmt, ...);
int printf(char *fmt, ...);


char fgetc(int fd);
char getc(void);
int fgetsn(int fd, char * c, int n);
int fgets(int fd, char * c, unsigned int n);
int gets(char * c, unsigned int n);

void fungetc(int fd, char c);
void ungetc(char c);

int scanf(char *fmt, ...);
int fscanf(int fd, char *fmt, ...);
int vfscanf(int fd, char *fmt, va_list ap);

#endif