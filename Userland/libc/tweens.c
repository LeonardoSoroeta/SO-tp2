/* http://gizma.com/easing/ */
#include <math.h>
#include <tweens.h>

/**
 * For a given time t, gives the position in the tween
 * @param  t time
 * @param  b initial value
 * @param  c delta (ending position - initial value)
 * @param  d duration
 * @return   position for the tween
 */
double ease_linear(double t, double b, double c, double d) {
	return c*t/d + b;
}

double ease_in_quad(double t, double b, double c, double d) {
	t /= d;
	return c*t*t + b;
}

double ease_out_quad(double t, double b, double c, double d) {
	t /= d;
	return -c * t*(t-2) + b;
}

double ease_inout_quad(double t, double b, double c, double d) {
	t /= d/2;
	if (t < 1) return c/2*t*t + b;
	t--;
	return -c/2 * (t*(t-2) - 1) + b;
}

double ease_in_cubic(double t, double b, double c, double d) {
	t /= d;
	return c*t*t*t + b;
}

double ease_out_cubic(double t, double b, double c, double d) {
	t /= d;
	t--;
	return -c * (t*t*+1) + b;
}

double ease_inout_cubic(double t, double b, double c, double d) {
	t /= d/2;
	if (t < 1) return c/2*t*t*t + b;
	t--;
	return c/2 * (t*t*t +2) + b;
}

double ease_in_sine(double t, double b, double c, double d) {
	return -c * cos(t/d * (HALFPI)) + c + b;
}

double ease_out_sine(double t, double b, double c, double d) {
	return c * sin(t/d * (HALFPI)) + b;
}

double ease_inout_sine(double t, double b, double c, double d) {
	return -c/2 * (cos(PI*t/d) - 1) + b;
}
