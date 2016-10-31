#ifndef __TWEENS_H__
#define __TWEENS_H__

/**
 * For a given time t, gives the position in the tween
 * @param  t time
 * @param  b initial value
 * @param  c delta (ending position - initial value)
 * @param  d duration
 * @return   position for the tween
 */
double ease_linear(double t, double b, double c, double d);
double ease_in_quad(double t, double b, double c, double d);
double ease_out_quad(double t, double b, double c, double d);
double ease_inout_quad(double t, double b, double c, double d);
double ease_in_cubic(double t, double b, double c, double d);
double ease_out_cubic(double t, double b, double c, double d);
double ease_inout_cubic(double t, double b, double c, double d);
double ease_in_sine(double t, double b, double c, double d);
double ease_out_sine(double t, double b, double c, double d);
double ease_inout_sine(double t, double b, double c, double d);

#endif