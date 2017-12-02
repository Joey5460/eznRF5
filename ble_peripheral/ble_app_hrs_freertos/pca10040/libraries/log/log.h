#ifndef _LOG_H_
#define _LOG_H_
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
typedef struct 
{
   uint32_t time;
   uint32_t steps;
   uint16_t ax;
   uint16_t ay;
   uint16_t az;
   uint16_t reserve;
} log_imu_t;

typedef struct 
{
   uint32_t time;
   uint32_t steps;
   uint16_t ax;
   uint16_t ay;
   uint16_t az;
   uint16_t reserve;
   uint32_t q0;
   uint32_t q1;
   uint32_t q2;
   uint32_t q3;
} log_imu_ex_t;

typedef void vu8_t(uint8_t *data);
typedef struct {
    vu8_t * sync_tx;
 
}log_handlers_t;

uint32_t log_init(void);
void log_uninit(void);
void log_set_callbacks(log_handlers_t cb);
void log_save(log_imu_t *data);
void log_save_ex(log_imu_ex_t *data);

int8_t log_sync_start(void);
int8_t log_sync_end(void);
uint32_t  log_sync_get_len(void);
int8_t log_sync_read(uint8_t pkg_len, uint8_t * data);

int8_t log_sync(void);
void log_clear(void);
void log_erase(void);

void log_standby(void);
void log_wakeup(void);
#endif//_LOG_H_
