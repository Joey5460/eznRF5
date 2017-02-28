#include "bsp_hx_adc.h"
#include "nrf_gpio.h"
//#include "nrf_drv_gpiote.h"
#include "nrf_delay.h"
#include "sdk_config.h"
#define NRF_LOG_MODULE_NAME "BSP_HX"
#include "nrf_log.h"


typedef enum{
    LOW=0,
    HIGH=1,
}dir_t;


static uint8_t _gain=1;
static int32_t _offset[2]={0,0}; 

void hx_adc_init(void)
{
	nrf_gpio_cfg_output(HX_SCK_PIN);
	nrf_gpio_cfg_output(HX_RATE_PIN);
	nrf_gpio_cfg_input(HX_ADC0_PIN, NRF_GPIO_PIN_PULLUP);
	nrf_gpio_cfg_input(HX_ADC1_PIN, NRF_GPIO_PIN_PULLUP);
	nrf_gpio_pin_set(HX_RATE_PIN);
}

void set_rate(uint8_t rate)
{
	if (rate){
		nrf_gpio_pin_set(HX_RATE_PIN);
	}else {
		nrf_gpio_pin_clear(HX_RATE_PIN);
	}
}

static void digitalwrite(dir_t dir)
{
    if (dir == HIGH){

		nrf_gpio_pin_set(HX_SCK_PIN);
        nrf_delay_us(1);

    }else if (dir == LOW){

		nrf_gpio_pin_clear(HX_SCK_PIN);
        nrf_delay_us(1);
    }
}

void set_channel(channel_t gain)
{
    _gain = gain;
    digitalwrite(LOW);
	int32_t adc_cnt[2] = {0,0};
    read_hx_adc(adc_cnt);
}

channel_t get_channel(void)
{
	return _gain;
}

int32_t read_hx_adc(int32_t * adc_cnt)
{
	uint8_t i = 0;

    adc_cnt[0] = 0;
    adc_cnt[1] = 0;
	nrf_gpio_pin_clear(HX_SCK_PIN);

    //if (nrf_gpio_pin_read(HX_ADC0_PIN)) nrf_gpio_pin_clear(30);
	while (nrf_gpio_pin_read(HX_ADC0_PIN)||nrf_gpio_pin_read(HX_ADC1_PIN))
	{
		;
	}
    //if (!nrf_gpio_pin_read(HX_ADC0_PIN)) nrf_gpio_pin_set(30);

	for (i = 0; i<24; i++){
		nrf_gpio_pin_set(HX_SCK_PIN);
        nrf_delay_us(1);
		adc_cnt[0]=(adc_cnt[0]<<1);
		adc_cnt[1]=(adc_cnt[1]<<1);
		nrf_gpio_pin_clear(HX_SCK_PIN);
        nrf_delay_us(1);

		if(nrf_gpio_pin_read(HX_ADC0_PIN)){
			adc_cnt[0] ++;
		}

		if(nrf_gpio_pin_read(HX_ADC1_PIN)){
			adc_cnt[1] ++;
		}
	}

	for (unsigned int i = 0; i < _gain; i++) {
		digitalwrite(HIGH);
		digitalwrite(LOW);
	}

    uint32_t filter = 0xFF;
	if (adc_cnt[0] & 0x800000){
        adc_cnt[0]|= (filter << 24);
    }

	if (adc_cnt[1] & 0x800000){
        adc_cnt[1]|= (filter << 24);
    }
	return 0;
}

void get_average(uint8_t times, int32_t * adc_cnt)
{
    uint8_t i = 0;
    adc_cnt[0] = 0;
    adc_cnt[1] = 0;
    for(i = 0;i < times; i++){
        int32_t adc_tmp[2]={0,0};
        read_hx_adc(adc_tmp);
        adc_cnt[0] += adc_tmp[0];
        adc_cnt[1] += adc_tmp[1];
    }

    adc_cnt[0] = adc_cnt[0]/times ;
    adc_cnt[1] = adc_cnt[1]/times ;

}

void get_value(uint8_t times, int32_t * adc_cnt)
{
    get_average(times, adc_cnt);
    adc_cnt[0] = adc_cnt[0] - _offset[0];
    adc_cnt[1] = adc_cnt[1] - _offset[1];
}

void set_offset(int32_t * offset)
{
    _offset[0] = offset[0];
    _offset[1] = offset[1];
}

void get_offset(int32_t * offset)
{
    offset[0] = _offset[0];
    offset[1] = _offset[1];
}


