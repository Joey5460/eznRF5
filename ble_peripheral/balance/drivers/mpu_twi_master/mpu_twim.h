/**
  ******************************************************************************
  * @file    i2c.h
  * @author  Thomas R.
  * @version V1.0
  * @date    15/11/15
  * @brief   header file for i2c driver
  ******************************************************************************
  */

#include <stdint.h>
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _MPU_I2C_H_
#define _MPU_I2C_H_

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
#define I2CERRORCOUNT_TO_RECOVER    5
#define I2CBUFFERSIZE               20

/* Exported macros -----------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
uint32_t i2c_init(void);
void i2c_deinit(void);
uint32_t i2c_read(uint8_t device_address, uint8_t register_address, uint16_t length, uint8_t *data);
uint32_t i2c_write(uint8_t device_address, uint8_t register_address, uint16_t length, uint8_t *data);

#endif /* _MPU_I2C_H_*/

