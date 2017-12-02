#ifndef APP_CONFIG_H
#define APP_CONFIG_H
// <h> Application 

#ifdef BOARD_PCA10040
//==========================================================
// <h> SPI_CONFIGURATION - Spi configuration

//==========================================================
// <o> SPI_SCK_PIN - Pin number  <0-31> 
#ifndef SPI_SCK_PIN
#define SPI_SCK_PIN 28
#endif

// <o> SPI_MISO_PIN - Pin number  <0-31> 
#ifndef SPI_MISO_PIN
#define SPI_MISO_PIN 4
#endif

// <o> SPI_MOSI_PIN - Pin number  <0-31> 
#ifndef SPI_MOSI_PIN
#define SPI_MOSI_PIN 3
#endif

// <o> SPI_SS_PIN - Pin number  <0-31> 
#ifndef SPI_SS_PIN
#define SPI_SS_PIN 29
#endif

// <o> SPI_IRQ_PRIORITY  - Interrupt priority


// <i> Priorities 0,2 (nRF51) and 0,1,4,5 (nRF52) are reserved for SoftDevice
// <0=> 0 (highest) 
// <1=> 1 
// <2=> 2 
// <3=> 3 
// <4=> 4 
// <5=> 5 
// <6=> 6 
// <7=> 7 

#ifndef SPI_IRQ_PRIORITY
#define SPI_IRQ_PRIORITY 7
#endif

#ifndef TWI1_SCL_PIN 
#define TWI1_SCL_PIN 27
//#define TWI1_SCL_PIN 15
#endif

#ifndef TWI1_SDA_PIN 
#define TWI1_SDA_PIN 26
//#define TWI1_SDA_PIN 14
#endif

#define MPU_INT_PIN 2

#define  FLASH_NCS_PIN     29
#define  FLASH_WP_PIN      31
#define  FLASH_HOLD_PIN    30

#else
#error "no flash gpio defined"
#endif// BOARD_PCA10040


#ifndef APP_FIFO_ENABLED
#define  APP_FIFO_ENABLED 1
#endif

#ifndef NRF_LOG_ENABLED
#define NRF_LOG_ENABLED 1 
#endif

#ifndef APP_SCHEDULER_ENABLED
#define APP_SCHEDULER_ENABLED 1
#endif

#ifndef CRC32_ENABLED
#define CRC32_ENABLED 1
#endif

#endif//APP_CONFIG_H
