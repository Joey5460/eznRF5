#ifndef _ST_PORT_H_
#define _ST_PORT_H_
#include <stdint.h>
void st_mpu_active(void);
void st_mpu_standy(void);
void st_store_active_data(void);
int32_t st_read_hx_adc(int32_t *hx_adc_val);
uint8_t st_sync_get_data(uint8_t * data);
uint8_t st_sync_start(void);
void st_sync_end(void);
void st_set_hx_offset(void);
#endif// _ST_PORT_H_
