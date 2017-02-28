#ifndef _XTIMERS_H_
#define _XTIMERS_H_

#include <stdint.h>
void utc_timer_init(void);
void utc_timer_start(void);
void utc_timer_stop(void);
uint32_t get_utc(void);
void set_utc(uint32_t sec);
#endif //_XTIMERS_H_
