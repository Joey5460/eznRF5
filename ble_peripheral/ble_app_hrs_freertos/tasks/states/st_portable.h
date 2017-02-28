#ifndef _ST_PORT_H_
#define _ST_PORT_H_
#include <stdint.h>
void st_mpu_active(void);
void st_mpu_standy(void);
void st_store_active_data(void);

uint8_t st_sync_get_data(uint8_t * data);
uint8_t st_imu_get_data(uint8_t * data);

uint8_t st_sync_start(void);
void st_sync_end(void);
#endif// _ST_PORT_H_
