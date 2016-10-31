#ifndef __MATH_H__
#define __MATH_H__

// math
// https://github.com/jitrc/eklavya-ros-pkg/blob/master/eklavya_bringup/src/VectorNavLib/include/math.h
#define TWOPI	6.28318530717958647692
#define PI	3.141592653589793238462643
#define HALFPI	1.57079632679489661923
#define QUARTPI 0.78539816339744830962
double sin(double);
#define cos(x)		(sin(x+HALFPI)) 	/* Cosinus [radians] */

double sqrt(double x);

#endif
