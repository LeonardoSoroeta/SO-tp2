#ifndef __CLOCK_H__
#define __CLOCK_H__

#define BCD2BIN(val) (((val)&15) + ((val)>>4)*10)
#define BIN2BCD(val) ((((val)/10)<<4) + (val)%10)

/**
 *
 * Tiene la informacion del clock de la arquitectura
 * correspondiente
 * 
 */
#include <arch/clock.h>
#include <arch/types.h>

typedef struct {
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	uint8_t day;
	uint8_t mon;
	uint8_t year;
} date_t;

uint8_t clock_get(clock_attr attr);
uint8_t clock_get_seconds();
uint8_t clock_get_minutes();
uint8_t clock_get_hours();
uint8_t clock_get_weekday();
uint8_t clock_get_day_of_month();
uint8_t clock_get_month();
uint8_t clock_get_year();

void clock_set(clock_attr attr, uint8_t value);
void clock_set_seconds(uint8_t);
void clock_set_minutes(uint8_t);
void clock_set_hours(uint8_t);
void clock_set_weekday(uint8_t);
void clock_set_day_of_month(uint8_t);
void clock_set_month(uint8_t);
void clock_set_year(uint8_t);

#endif