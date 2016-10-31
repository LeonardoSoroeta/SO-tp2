#ifndef __ARCH_CLOCK_H__
#define __ARCH_CLOCK_H__


#define CLOCK_ATTR_PORT			0x70
#define CLOCK_GETSET_PORT		0x71

typedef enum {
	SECONDS 				= 0x00,
	SECONDS_ALARM		= 0x01,
	MINUTES 				= 0X02,
	MINUTES_ALARM 	= 0X03,
	HOURS 					= 0X04,
	HOURS_ALARM 		= 0X05,
	WEEKDAY 				= 0X06,
	DAY_OF_MONTH 		= 0X07,
	MONTH 					= 0X08,
	YEAR 						= 0X09,
	REGA 						= 0x0A,
	REGB						= 0x0B,
	REGC						= 0x0C,
	REGD						= 0x0D,
	DIAGNOSTICS			= 0x0E,
	CLK_SHUTDOWN		= 0x0F
} clock_attr;

#endif