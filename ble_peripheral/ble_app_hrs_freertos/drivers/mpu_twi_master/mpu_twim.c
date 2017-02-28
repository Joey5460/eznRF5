/**
  ******************************************************************************
  * @file    i2c.c
  * @author  Thomas R.
  * @version V1.0
  * @date    15/11/15
  * @brief   i2c Driver
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "mpu_twim.h"
#include <string.h>
#include "nrf_delay.h"
#include "nrf_soc.h"
#include "nrf_drv_twi.h"
#include "app_util_platform.h"
#if defined HELENA_DEBUG_RTT
#include "SEGGER_RTT.h"
#endif
#include "nrf_gpio.h"
#include "sdk_config.h"
#include "nrf_log.h"

/* External variables --------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private defines -----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static const nrf_drv_twi_t twi_instance = NRF_DRV_TWI_INSTANCE(1);
static nrf_drv_twi_config_t twi_config = NRF_DRV_TWI_DEFAULT_CONFIG;


/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
volatile static bool twi_tx_done = false;
volatile static bool twi_rx_done = false;
static void twi_handler(nrf_drv_twi_evt_t const * evt, void * p_context)
{
    switch(evt->type)
    {
        case NRF_DRV_TWI_EVT_DONE:
            //nrf_gpio_pin_toggle(24);
            switch(evt->xfer_desc.type)
            {
                case NRF_DRV_TWI_XFER_TX:
                    twi_tx_done = true;
                    break;
                case NRF_DRV_TWI_XFER_TXTX:
                    twi_tx_done = true;
                    break;
                case NRF_DRV_TWI_XFER_RX:
                    twi_rx_done = true;
                    break;
                case NRF_DRV_TWI_XFER_TXRX:
                    twi_rx_done = true;
                    break;
                default:
                    break;
            }
            break;
        case NRF_DRV_TWI_EVT_ADDRESS_NACK:
            break;
        case NRF_DRV_TWI_EVT_DATA_NACK:
            break;
        default:
            break;
	}
}

/* Public functions ----------------------------------------------------------*/
uint32_t i2c_init()
{
    static uint32_t err_code;
    twi_config.scl = TWI1_SCL_PIN; 
    twi_config.sda = TWI1_SDA_PIN; 
    err_code = nrf_drv_twi_init(&twi_instance, &twi_config, twi_handler, NULL);
    if (err_code == NRF_SUCCESS)
    {
        nrf_drv_twi_enable(&twi_instance);
        nrf_delay_ms(400);
    }
    return err_code;
}

void i2c_deinit()
{
    nrf_drv_twi_disable(&twi_instance);
    nrf_drv_twi_uninit(&twi_instance);
    nrf_gpio_cfg_input(TWI1_SCL_PIN, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(TWI1_SDA_PIN, NRF_GPIO_PIN_PULLUP);
}

// The new SDK 11 TWI driver is not able to do two transmits without repeating the ADDRESS + Write bit byte
// Hence we need to merge the MPU register address with the buffer and then transmit all as one transmission
static void buffer_merger(uint8_t * new_buffer, uint8_t reg, uint8_t * p_data, uint32_t length)
{
    new_buffer[0] = reg;
    memcpy((new_buffer + 1), p_data, length);
}

#define MPU_TWI_TIMEOUT 1000000
static uint32_t mpu_write_burst(uint8_t device_address, uint8_t reg, uint8_t * p_data, uint32_t length)
{
    // This burst write function is not optimal and needs improvement.
    // The new SDK 11 TWI driver is not able to do two transmits without repeating the ADDRESS + Write bit byte
    uint32_t err_code;
    uint32_t timeout = MPU_TWI_TIMEOUT;

    // Merging MPU register address and p_data into one buffer.
    uint8_t buffer[20];
    buffer_merger(buffer, reg, p_data, length);

    // Setting up transfer
    nrf_drv_twi_xfer_desc_t xfer_desc;
    xfer_desc.address = device_address;
    xfer_desc.type = NRF_DRV_TWI_XFER_TX;
    xfer_desc.primary_length = length + 1;
    xfer_desc.p_primary_buf = buffer;

    // Transfering
    err_code = nrf_drv_twi_xfer(&twi_instance, &xfer_desc, 0);

    while((!twi_tx_done) && --timeout);
    if(!timeout) {
        
        return NRF_ERROR_TIMEOUT;
    }
    twi_tx_done = false;

    return err_code;
}

uint32_t mpu_write_register(uint8_t device_address, uint8_t reg, uint8_t data)
{
    uint32_t err_code;
    uint32_t timeout = MPU_TWI_TIMEOUT;

    uint8_t packet[2] = {reg, data};

    err_code = nrf_drv_twi_tx(&twi_instance, device_address, packet, 2, false);
    if(err_code != NRF_SUCCESS) return err_code;

    while((!twi_tx_done) && --timeout);
    if(!timeout) return NRF_ERROR_TIMEOUT;

    twi_tx_done = false;

    return err_code;
}

uint32_t i2c_read(uint8_t device_address, uint8_t reg, uint16_t length, uint8_t *data)
{
	uint32_t err_code;
    uint32_t timeout = MPU_TWI_TIMEOUT;

    err_code = nrf_drv_twi_tx(&twi_instance, device_address, &reg, 1, false);
    if(err_code != NRF_SUCCESS) return err_code;

    while((!twi_tx_done) && --timeout);
    if(!timeout)     {
    
        NRF_LOG_INFO("[I2C]: read time out\r\n");
        return NRF_ERROR_TIMEOUT;

    }
    twi_tx_done = false;

    err_code = nrf_drv_twi_rx(&twi_instance, device_address, data, length);
    if(err_code != NRF_SUCCESS) return err_code;

    timeout = MPU_TWI_TIMEOUT;
    while((!twi_rx_done) && --timeout);
    if(!timeout) {
        NRF_LOG_INFO("[I2C]: read 1 time out\r\n");

        return NRF_ERROR_TIMEOUT;
    }
    twi_rx_done = false;

	return err_code;
}

uint32_t i2c_write(uint8_t device_address, uint8_t reg, uint16_t length, uint8_t *data)
{
	uint32_t err_code = 0;
	if (length < 2){

		err_code = mpu_write_register(device_address, reg, *data);
	}else{
		err_code = mpu_write_burst(device_address, reg, data, length);

	}
	return err_code;
}

/**END OF FILE*****************************************************************/



