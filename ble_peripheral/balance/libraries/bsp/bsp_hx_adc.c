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



void hx_adc_balance_init()
{
    hx_adc_cfg_t  hx_adc[HX_ADC_NUM]={
        {.sck = HX_SCK0_PIN, .adc = HX_ADC0_PIN},
        {.sck = HX_SCK1_PIN, .adc = HX_ADC1_PIN},
        {.sck = HX_SCK2_PIN, .adc = HX_ADC2_PIN},
        {.sck = HX_SCK3_PIN, .adc = HX_ADC3_PIN},
    };
    uint8_t i = 0;
    for (i=0;i<HX_ADC_NUM;i++){
        hx_adc_init(&hx_adc[i]);
    }
}
void hx_adc_init(hx_adc_cfg_t * hx_adc)
{
	//nrf_gpio_cfg_output(HX_SCK0_PIN);
	//nrf_gpio_cfg_input(HX_ADC0_PIN, NRF_GPIO_PIN_PULLUP);
	nrf_gpio_cfg_output(hx_adc->sck);
	nrf_gpio_cfg_input(hx_adc->adc, NRF_GPIO_PIN_PULLUP);

    nrf_gpio_cfg_output(HX_POW_PIN); 
    power_on();
}


void power_on(void)
{
	nrf_gpio_pin_set(HX_POW_PIN);
}


static void digitalwrite(hx_adc_cfg_t * hx_adc, dir_t dir)
{
    if (dir == HIGH){

		//nrf_gpio_pin_set(HX_SCK0_PIN);
		nrf_gpio_pin_set(hx_adc->sck);
        nrf_delay_us(1);

    }else if (dir == LOW){

		//nrf_gpio_pin_clear(HX_SCK0_PIN);
		nrf_gpio_pin_clear(hx_adc->sck);
        nrf_delay_us(1);
    }
}

void set_channel(hx_adc_cfg_t * hx_adc, channel_t gain)
{
    hx_adc->gain = gain;
    digitalwrite(hx_adc, LOW);
    read_hx_adc(hx_adc);
}

channel_t get_channel(hx_adc_cfg_t * hx_adc)
{
	return hx_adc->gain;
}


int32_t read_hx_adc(hx_adc_cfg_t * hx_adc)
{
	uint8_t i = 0;
    int32_t adc_val = 0;
	//nrf_gpio_pin_clear(HX_SCK0_PIN);
	nrf_gpio_pin_clear(hx_adc->sck);

    //if (nrf_gpio_pin_read(HX_ADC0_PIN)) nrf_gpio_pin_clear(30);
	//while (nrf_gpio_pin_read(HX_ADC0_PIN))
	while (nrf_gpio_pin_read(hx_adc->adc))
	{
		;
	}
    //if (!nrf_gpio_pin_read(HX_ADC0_PIN)) nrf_gpio_pin_set(30);

	for (i = 0; i<24; i++){
        //nrf_gpio_pin_set(HX_SCK0_PIN);
        nrf_gpio_pin_set(hx_adc->sck);
        nrf_delay_us(1);
		adc_val=(adc_val<<1);
		//nrf_gpio_pin_clear(HX_SCK0_PIN);
		nrf_gpio_pin_clear(hx_adc->sck);
        nrf_delay_us(1);

		//if(nrf_gpio_pin_read(HX_ADC0_PIN)){
		if(nrf_gpio_pin_read(hx_adc->adc)){
			adc_val++;
		}

		//if(nrf_gpio_pin_read(HX_ADC1_PIN)){
		if(nrf_gpio_pin_read(hx_adc->adc)){
			adc_val++;
		}
	}

	for (unsigned int i = 0; i < hx_adc->gain; i++) {
		digitalwrite(hx_adc, HIGH);
		digitalwrite(hx_adc, LOW);
	}

    uint32_t filter = 0xFF;
	if (adc_val & 0x800000){
        adc_val|= (filter << 24);
    }

	return adc_val;
}

int32_t get_average(hx_adc_cfg_t * hx_adc, uint8_t times)
{
    uint8_t i = 0;
    int32_t adc_val = 0;
    for(i = 0;i < times; i++){
        int32_t adc_tmp=0;
        adc_tmp=read_hx_adc(hx_adc);
        adc_val += adc_tmp;
    }

    return adc_val/times ;
}

int32_t get_value(hx_adc_cfg_t * hx_adc, uint8_t times)
{
    int32_t adc_val = get_average(hx_adc, times);
    return (adc_val - hx_adc->offset);
}

void set_offset(hx_adc_cfg_t * hx_adc, int32_t offset)
{
    hx_adc->offset = offset;
}

int32_t get_offset(hx_adc_cfg_t * hx_adc )
{
    return hx_adc->offset;
}


