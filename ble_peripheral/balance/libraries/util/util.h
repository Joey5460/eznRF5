#ifndef _UTIL_H_ 
#define _UTIL_H_ 
#include <stdint.h>
#define time_after(unknown, known) ((long)(known) - (long)(unknown) < 0) 
uint8_t mac2symbol(uint8_t *mac, uint8_t *out, uint8_t len);
#endif
