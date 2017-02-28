#ifndef _BSP_MPU_H_
#define _BSP_MPU_H_
#include <stdint.h>//include  uint_8
typedef void (*mpu_evt_handler_t) (uint32_t evt);
typedef enum
{
    BSP_MPU_EVT_MOT = 0,

}bsp_mpu_event_t;

void bsp_mpu_init(void);
void bsp_mpu_uninit(void);
void bsp_mpu_standby(void);
void bsp_mpu_wakeup(void);
uint8_t get_mpu_int(void);
void clear_mpu_int(void);
void bsp_mpu_evt_handler_set(mpu_evt_handler_t mpu_evt_handler);
#endif //_BSP_MPU_H_ 

