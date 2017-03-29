#ifndef APP_CONFIG_H
#define APP_CONFIG_H
// <h> Application 

#ifdef EZSMART
//==========================================================
// <h> SPI_CONFIGURATION - Spi configuration

//==========================================================
// <o> SPI_SCK_PIN - Pin number  <0-31> 
#ifndef SPI_SCK_PIN
#define SPI_SCK_PIN 17
#endif

// <o> SPI_MISO_PIN - Pin number  <0-31> 
#ifndef SPI_MISO_PIN
#define SPI_MISO_PIN 19
#endif

// <o> SPI_MOSI_PIN - Pin number  <0-31> 
#ifndef SPI_MOSI_PIN
#define SPI_MOSI_PIN 16
#endif

// <o> SPI_SS_PIN - Pin number  <0-31> 
#ifndef SPI_SS_PIN
//#define SPI_SS_PIN 14
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
#define TWI1_SCL_PIN 15
#endif

#ifndef TWI1_SDA_PIN 
#define TWI1_SDA_PIN 14
#endif

#define MPU_INT_PIN 13

#define HX_ADC0_PIN 7
#define HX_ADC1_PIN 2
#define HX_ADC2_PIN 9
#define HX_ADC3_PIN 5

#define HX_SCK0_PIN 8
#define HX_SCK1_PIN 3
#define HX_SCK2_PIN 10
#define HX_SCK3_PIN 6

#define HX_POW_PIN 12
#define HX_ADC_NUM  4

#define  FLASH_NCS_PIN     18
#define  FLASH_WP_PIN      20
//#define  FLASH_HOLD_PIN    18

// <e> NRF_LOG_BACKEND_SERIAL_USES_UART - If enabled data is printed over UART

//==========================================================

#ifndef NRF_LOG_BACKEND_SERIAL_USES_UART
#define NRF_LOG_BACKEND_SERIAL_USES_UART 1
#endif

#if  NRF_LOG_BACKEND_SERIAL_USES_UART
// <o> NRF_LOG_BACKEND_SERIAL_UART_BAUDRATE  - Default Baudrate

// <323584=> 1200 baud 
// <643072=> 2400 baud 
// <1290240=> 4800 baud 
// <2576384=> 9600 baud 
// <3862528=> 14400 baud 
// <5152768=> 19200 baud 
// <7716864=> 28800 baud 
// <10289152=> 38400 baud 
// <15400960=> 57600 baud 
// <20615168=> 76800 baud 
// <30801920=> 115200 baud 
// <61865984=> 230400 baud 
// <67108864=> 250000 baud 
// <121634816=> 460800 baud 
// <251658240=> 921600 baud 
// <268435456=> 57600 baud 
#ifndef NRF_LOG_BACKEND_SERIAL_UART_BAUDRATE
#define NRF_LOG_BACKEND_SERIAL_UART_BAUDRATE 30801920
#endif

// <o> NRF_LOG_BACKEND_SERIAL_UART_TX_PIN - UART TX pin 
#ifndef NRF_LOG_BACKEND_SERIAL_UART_TX_PIN
#define NRF_LOG_BACKEND_SERIAL_UART_TX_PIN 28//27
//#define NRF_LOG_BACKEND_SERIAL_UART_TX_PIN 6
#endif

// <o> NRF_LOG_BACKEND_SERIAL_UART_RX_PIN - UART RX pin 
#ifndef NRF_LOG_BACKEND_SERIAL_UART_RX_PIN
#define NRF_LOG_BACKEND_SERIAL_UART_RX_PIN 28//26
//#define NRF_LOG_BACKEND_SERIAL_UART_RX_PIN 8
#endif

// <o> NRF_LOG_BACKEND_SERIAL_UART_RTS_PIN - UART RTS pin 
#ifndef NRF_LOG_BACKEND_SERIAL_UART_RTS_PIN
#define NRF_LOG_BACKEND_SERIAL_UART_RTS_PIN 5
#endif

// <o> NRF_LOG_BACKEND_SERIAL_UART_CTS_PIN - UART CTS pin 
#ifndef NRF_LOG_BACKEND_SERIAL_UART_CTS_PIN
#define NRF_LOG_BACKEND_SERIAL_UART_CTS_PIN 7
#endif

// <o> NRF_LOG_BACKEND_SERIAL_UART_FLOW_CONTROL  - Hardware Flow Control
 
// <0=> Disabled 
// <1=> Enabled 

#ifndef NRF_LOG_BACKEND_SERIAL_UART_FLOW_CONTROL
#define NRF_LOG_BACKEND_SERIAL_UART_FLOW_CONTROL 0
#endif

// <o> NRF_LOG_BACKEND_UART_INSTANCE  - UART instance used
 
// <0=> 0 

#ifndef NRF_LOG_BACKEND_UART_INSTANCE
#define NRF_LOG_BACKEND_UART_INSTANCE 0
#endif

#endif //NRF_LOG_BACKEND_SERIAL_USES_UART

#define BOARD_CUSTOM
#else//EZSMART
#error "no flash gpio defined"
#endif// EZSMART


#ifndef APP_FIFO_ENABLED
#define  APP_FIFO_ENABLED 1
#endif

#ifndef NRF_LOG_ENABLED
#define NRF_LOG_ENABLED 1 
#endif

#ifndef LOW_POWER_PWM_ENABLED
#define LOW_POWER_PWM_ENABLED 1
#endif

#endif//APP_CONFIG_H
