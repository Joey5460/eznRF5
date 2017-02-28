#include <stdint.h>
#include "../mpu_twi_master/mpu_twim.h"
#include "nordic_common.h"
#include "app_util_platform.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"

void ns_delay_ms(uint32_t dly)
{
	nrf_delay_ms(dly);
}

void ns_get_clock_ms(unsigned long * count)
{
}


uint8_t ns_i2c_write(uint8_t dev_addr, uint8_t reg_addr, uint16_t i2c_len, uint8_t *i2c_data_buf)
{		
	if (0 == i2c_write(dev_addr, reg_addr, i2c_len, i2c_data_buf)){
		return 0;
	}
	return -1;
}

uint8_t ns_i2c_read(uint8_t dev_addr, uint8_t reg_addr, uint16_t i2c_len, uint8_t *i2c_data_buf)
{
	if (0 == i2c_read(dev_addr, reg_addr, i2c_len, i2c_data_buf)){
		return 0;
	}
	return -1;
}



