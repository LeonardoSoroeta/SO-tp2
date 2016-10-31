#include <arch/types.h>
#include <sys/clock.h>
#include <arch/arch.h>
#include <util/util.h>

/*===================================
*************    GETTERS
===================================*/

uint8_t clock_get(clock_attr attr) {
	_outport(CLOCK_ATTR_PORT, attr);
	return _inport(CLOCK_GETSET_PORT);
}

uint8_t clock_get_seconds() {
	return clock_get(SECONDS);
}

uint8_t clock_get_minutes() {
	return clock_get(MINUTES);
}

uint8_t clock_get_hours() {
	return clock_get(HOURS);
}

uint8_t clock_get_weekday() {
	return clock_get(WEEKDAY);
}

uint8_t clock_get_day_of_month() {
	return clock_get(DAY_OF_MONTH);
}

uint8_t clock_get_month() {
	return clock_get(MONTH);
}

uint8_t clock_get_year() {
	return clock_get(YEAR);
}

/*===================================
*************    SETTERS
===================================*/

void clock_set(clock_attr attr, uint8_t value) {

	_outport(CLOCK_ATTR_PORT, attr);
	_outport(CLOCK_GETSET_PORT, value);

}

void clock_set_seconds(uint8_t value) {
	clock_set(SECONDS, value);
}

void clock_set_minutes(uint8_t value) {
	clock_set(MINUTES, value);
}

void clock_set_hours(uint8_t value) {
	clock_set(HOURS, value);
}

void clock_set_weekday(uint8_t value) {
	clock_set(WEEKDAY, value);
}

void clock_set_day_of_month(uint8_t value) {
	clock_set(DAY_OF_MONTH, value);
}

void clock_set_month(uint8_t value) {
	clock_set(MONTH, value);
}

void clock_set_year(uint8_t value) {
	clock_set(YEAR, value);
}
