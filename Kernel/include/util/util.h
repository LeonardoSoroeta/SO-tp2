#ifndef __UTIL_H__
#define __UTIL_H__

#include <arch/types.h>

string itoa(int32_t value, char *str, uint8_t base);
string itoa2(int64_t i);
string itoa10(int64_t);
string itoa16(int64_t i);
int intlen(int64_t n);
int strlen(char* s);
char* strcpy(char *dst, char* src);
char* strdup(char *src);

uint8_t to_bcd8(uint8_t);
uint16_t to_bcd16(uint16_t);


// math
// https://github.com/jitrc/eklavya-ros-pkg/blob/master/eklavya_bringup/src/VectorNavLib/include/math.h
#define TWOPI	6.28318530717958647692
#define PI	3.141592653589793238462643
#define HALFPI	1.57079632679489661923
#define QUARTPI 0.78539816339744830962
double sin(double);
#define cos(x)		(sin(x+HALFPI)) 	/* Cosinus [radians] */


int strcmp(char* s1, char* s2);

#endif