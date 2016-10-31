#include <util/util.h>
#include <arch/kmem.h>

string itoa(int32_t value, char *str, uint8_t base) {
	
	char *rc, *ptr, *low;

	if (base < 2 || base > 36) {
		*str = 0;
		return str;
	}
	rc = ptr = str;
	if (value < 0 && base == 10) {
		*ptr++ = '-';
	}
	low = ptr;
	do {
		*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];
		value /= base;
	} while (value);
	*ptr-- = 0;

	while (low < ptr) {
		char tmp = *low;
		*low++ = *ptr;
		*ptr-- = tmp;
	}
	return rc;
}

int intlen(int64_t n) {
	int l = 0;
	while (n != 0) {
		n /= 10;
		l++;
	}
	return l;
}

int strlen(char* s) {
	int l = 0;
	while (*s != 0) {
		l++;
		s++;
	}
	return l;
}

char* strcpy(char *dst, char* src) {
  
  while (*src != '\0') {
    *dst = *src;
    dst++;
    src++;
  }

  *dst = '\0';

  return dst;
}

char* strdup(char* src) {

  char *out;

  out = (char*)kcalloc(strlen(src)+1, sizeof(char));

  strcpy(out, src);

  return out;

}

uint8_t to_bcd8(uint8_t n) {
	return ((n & 0xf0)>>4)*10 + (n & 0x0f);
}

uint16_t to_bcd16(uint16_t n) {
	return ((n & 0xf000)>>16)*1000 + ((n & 0x0f00)>>8) * 100 + to_bcd8(n & 0x00ff);
}


#define INT_DIGITS 19		/* enough for 64 bit integer */

char *itoa10(int64_t i) {
  /* Room for INT_DIGITS digits, - and '\0' */
  static char buf[INT_DIGITS + 2];
  char *p = buf + INT_DIGITS + 1;	/* points to terminating '\0' */
  if (i >= 0) {
    do {
      *--p = '0' + (i % 10);
      i /= 10;
    } while (i != 0);
    return p;
  }
  else {			/* i < 0 */
    do {
      *--p = '0' - (i % 10);
      i /= 10;
    } while (i != 0);
    *--p = '-';
  }
  return p;
}

char *itoa16(int64_t i) {
  /* Room for INT_DIGITS digits, - and '\0' */
  static char buf[INT_DIGITS + 2];
  char *p = buf + INT_DIGITS + 1;	/* points to terminating '\0' */
  uint8_t rem;
  do {
  	rem = i % 16;
  	if (rem >= 10)
  		*--p = 'A' + rem - 10;
  	else
  		*--p = '0' + rem;
    i /= 16;
  } while (i != 0);
  return p;
}

char *itoa2(int64_t i) {
  /* Room for INT_DIGITS digits, - and '\0' */
  static char buf[64 + 2];
  char *p = buf + 64 + 1;	/* points to terminating '\0' */
  do {
  	*--p = '0' + (i % 2);
    i /= 2;
  } while (i != 0);
  return p;
}

// https://github.com/jitrc/eklavya-ros-pkg/blob/master/eklavya_bringup/src/VectorNavLib/include/math.h
double sin(double x) {
  double xi, y, q, q2;
  int sign;

  xi = x; sign = 1;
  while (xi < -1.57079632679489661923) xi += 6.28318530717958647692;
  while (xi > 4.71238898038468985769) xi -= 6.28318530717958647692;
  if (xi > 1.57079632679489661923) {
    xi -= 3.141592653589793238462643;
    sign = -1;
  }
  q = xi / 1.57079632679; q2 = q * q;
  y = ((((.00015148419  * q2
        - .00467376557) * q2
        + .07968967928) * q2
        - .64596371106) * q2
        +1.57079631847) * q;
  return(sign < 0? -y : y);
}

int strcmp(char* s1, char* s2) {

  while (*s1 != '\0' || *s2 != '\0') {
    if (*s1 != *s2) {
      return *s1-*s2;
    }
    s1++;
    s2++;
  }
  if (*s1 != '\0')
    return 1;
  if (*s2 != '\0')
    return -1;
  return 0;
}
