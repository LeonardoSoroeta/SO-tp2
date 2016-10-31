#include <math.h>

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

double sqrt(double x) {
	double res;
	__asm__ __volatile__ ("fsqrt" : "=t" (res) : "0" (x));
	return res;
}
