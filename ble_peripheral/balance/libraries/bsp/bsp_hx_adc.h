#ifndef _BSP_HX_ADX_H_
#define _BSP_HX_ADX_H_
#include <stdint.h>

typedef enum{
    A128=1,
    B32=2,
    A64=3,
}channel_t;

void hx_adc_init(void);
int32_t read_hx_adc(int32_t *adc_cnt);
channel_t get_channel(void);
void set_channel(channel_t gain);
void set_rate(uint8_t rate);
void power_on(void);

void get_average(uint8_t times, int32_t * adc_cnt);
void get_value(uint8_t times, int32_t * adc_cnt);
void set_offset(int32_t * offset);
void get_offset(int32_t * offset);

#endif // _BSP_HX_ADX_H_
