#ifndef __TYPES_H__
#define __TYPES_H__

#include <stdint.h>

#define TRUE 1
#define FALSE 0

#define true TRUE
#define false FALSE

#define YES true
#define NO false

#define yes YES
#define no NO

typedef unsigned char boolean;
typedef boolean bool;

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int dword;
typedef unsigned long ddword;

// typedef signed char int8_t;
// typedef short int16_t;
// typedef int int32_t;
// typedef long long int64_t;

// typedef char uint8_t;
// typedef unsigned short uint16_t;
// typedef unsigned int uint32_t;
// typedef unsigned long long uint64_t;

typedef char* string;

typedef int8_t error_t;

typedef uint64_t time_t;

#define nil ((void*)0)

#endif