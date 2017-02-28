#ifndef _ST_PORT_H_
#define _ST_PORT_H_
#include <stdint.h>
void st_mpu_active(void);
void st_mpu_standy(void);
void st_store_active_data(void);
void st_read_hx_adc(uint8_t channel, int32_t *adc_val);
uint8_t st_sync_get_data(uint8_t * data);
uint8_t st_sync_start(void);
void st_sync_end(void);
void st_get_offset(int32_t * adc_val);
#endif// _ST_PORT_H_
