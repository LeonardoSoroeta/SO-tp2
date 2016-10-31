#ifndef __SYS_LOGGING_H__
#define __SYS_LOGGING_H__

#include <arch/types.h>

void logf(char * fmt, ...);
void logi(int64_t);
void loghex(int64_t);
void logd(double);
void logc(char);
void logs(char*);
void lognl();

#endif // __SYS_LOGGING_H__