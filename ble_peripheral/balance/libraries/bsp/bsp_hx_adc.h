#ifndef _BSP_HX_ADX_H_
#define _BSP_HX_ADX_H_
#include <stdint.h>

typedef enum{
    A128=1,
    B32=2,
    A64=3,
}channel_t;

typedef struct {
    uint8_t  sck; 
    uint8_t  adc;
    int32_t offset;
    uint8_t gain;

} hx_adc_cfg_t;

void hx_adc_balance_init();
void hx_adc_init(hx_adc_cfg_t * hx_adc);
int32_t read_hx_adc(hx_adc_cfg_t * hx_adc);
channel_t get_channel(hx_adc_cfg_t * hx_adc);
void set_channel(hx_adc_cfg_t * hx_adc, channel_t gain);
void power_on(void);

int32_t get_average(hx_adc_cfg_t * hx_adc, uint8_t times);
int32_t get_value(hx_adc_cfg_t * hx_adc, uint8_t times);
void set_offset(hx_adc_cfg_t * hx_adc, int32_t  offset);
int32_t get_offset(hx_adc_cfg_t * hx_adc);

#endif // _BSP_HX_ADX_H_
