#include "w25q_flash.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "../flash_spi_master/flash_spi_master.h"
#define NRF_LOG_MODULE_NAME "FLASH"
#include "nrf_log.h"
#include "sdk_config.h"


static uint8_t flash_access(uint32_t addr, uint8_t * buffer, uint16_t len, bool read);
static uint8_t flash_get_id(uint8_t *manufacturer_id, uint16_t *device_id);

// === W25Q INSTRUCTION SET ===

#define W25Q_WEN           0x06
#define W25Q_WDI           0x04

#define W25Q_READ          0x03

#define W25Q_PROG_PAGE     0x02

#define W25Q_ERASE_SECTOR  0x20  // 4 KByte
#define W25Q_ERASE_BLOCK1  0x52  // 32 KByte
#define W25Q_ERASE_BLOCK2  0xD8  // 64 KByte
#define W25Q_ERASE_CHIP    0xC7  // 1024 KByte

#define W25Q_SR_WEN        0x50
#define W25Q_SR_READ1      0x05
#define W25Q_SR_READ2      0x35
#define W25Q_SR_WRITE      0x01 

#define W25Q_JEDEC_ID      0x9F

#define W25Q_POWER_DOWN    0xB9
#define W25Q_WAKE_UP       0xAB

#define ACCESS_FLASH_DATA(aTxBuffer, aRxBuffer, size)      \
do{                                                        \
  nrf_gpio_pin_clear(FLASH_NCS_PIN);                       \
    flash_spi_send_recv(aTxBuffer, aRxBuffer, size);       \
       nrf_gpio_pin_set(FLASH_NCS_PIN);                    \
}while(0)         
                                       
uint8_t  flash_init(void)
{
   
  uint8_t  manufacturer_id = 00;
  uint16_t device_id = 00;
  
  flash_spi_master_init();
  nrf_gpio_pin_set(FLASH_NCS_PIN);
  nrf_gpio_cfg_output(FLASH_NCS_PIN);
  
  nrf_gpio_pin_set(FLASH_WP_PIN);
  nrf_gpio_cfg_output(FLASH_WP_PIN);
  
  //nrf_gpio_pin_set(FLASH_HOLD_PIN);
  //nrf_gpio_cfg_output(FLASH_HOLD_PIN);
  
  flash_wake_up();
  nrf_delay_ms(5);
  
  flash_get_id(&manufacturer_id, &device_id);
  NRF_LOG_INFO("[FLASH]%x\r\n", manufacturer_id);
  NRF_LOG_INFO("[FLASH]%x\r\n", device_id);
  
  if((0xEF == manufacturer_id) && (0x4014 == device_id))
  {
      NRF_LOG_INFO("[FLASH] Init OK\r\n");
      return FLASH_ERROR_NONE;
  }
  else
  {
      NRF_LOG_INFO("[FLASH] Could not initialize flash.\r\n");
    
      return FLASH_ERROR_BUS;
  }
}

void flash_uninit(void)
{
  flash_power_down();
  flash_spi_master_uninit();
  nrf_gpio_cfg_input(FLASH_NCS_PIN, NRF_GPIO_PIN_PULLUP);
  nrf_gpio_cfg_input(FLASH_WP_PIN, NRF_GPIO_PIN_PULLUP);
  //nrf_gpio_cfg_input(FLASH_HOLD_PIN, NRF_GPIO_PIN_PULLUP);
}

static uint8_t flash_get_id(uint8_t *manufacturer_id, uint16_t *device_id)
{
  uint8_t tx_buf[4], rx_buf[4];
  
  tx_buf[0] = W25Q_JEDEC_ID;
  ACCESS_FLASH_DATA(tx_buf, rx_buf, 4);
  NRF_LOG_INFO("[FLASH]%x %x %x %x\r\n", rx_buf[0], rx_buf[1], rx_buf[2],rx_buf[3]);
  *manufacturer_id = rx_buf[1];
  *device_id = ((uint16_t)rx_buf[2] << 8) | rx_buf[3];
  
  return FLASH_ERROR_NONE;
}

static uint8_t flash_access(uint32_t addr, uint8_t * buffer, uint16_t len, bool read)
{
  static uint8_t TxBuffer[W25Q_PAGE_SIZE + 4];
  static uint8_t RxBuffer[W25Q_PAGE_SIZE + 4];
  
  if((len > W25Q_PAGE_SIZE-5)
     || (0 == len)
       || (NULL == buffer)
         || ((addr + len) > W25Q_MEM_SIZE-5))
  {
    NRF_LOG_INFO("FLASH ACCESS ERROR" );
    return FLASH_ERROR_PARA;
  }
  
  TxBuffer[0] = read ? W25Q_READ : W25Q_PROG_PAGE;
  TxBuffer[1] = 0xFF & (addr >> 16);
  TxBuffer[2] = 0xFF & (addr >> 8);
  TxBuffer[3] = 0xFF & addr;
  
  if(!read)
  {
    memcpy(&TxBuffer[4], buffer, len);
  }

  //app_trace_log("access len %u \r\n", len);
  //ACCESS_FLASH_DATA(TxBuffer, RxBuffer, 5);
  //if (len>1) ACCESS_FLASH_DATA(&TxBuffer[5], &RxBuffer[5], len-1);
  ACCESS_FLASH_DATA(TxBuffer, RxBuffer, len+4);
  if(read)
  {
    memcpy(buffer, &RxBuffer[4], len);
  }
  
  return FLASH_ERROR_NONE;
}

uint8_t flash_read(uint32_t addr, uint8_t * buffer, uint16_t len)
{
  uint16_t idx, this_write;
  uint8_t ret;
  
  
  for(idx = 0; idx < len; idx += this_write)
  {
    this_write = MIN(len - idx, W25Q_PAGE_SIZE - 5);
    //this_write = MIN(len - idx, W25Q_PAGE_SIZE );
	//app_trace_log("read addr %x\r\n", addr+idx);
    while(flash_check_idle()){
        ;
    }
    ret = flash_access(addr + idx, &buffer[idx], this_write, true);
    if(ret){
        NRF_LOG_INFO("FLASH ACCESS\r\n");
        return ret;
    }
  }
  
  return FLASH_ERROR_NONE;
}

uint8_t flash_program(uint32_t addr, uint8_t * buffer, uint16_t len)
{
  uint16_t idx, this_write;
  uint8_t ret;
  
  for(idx = 0; idx < len; idx += this_write){

    this_write = (W25Q_PAGE_SIZE ) - ((addr + idx) & (W25Q_PAGE_SIZE - 1));
    this_write = MIN(len - idx, this_write);
    this_write = MIN(this_write, W25Q_PAGE_SIZE-5);
	NRF_LOG_INFO("write addr %x\r\n", addr+idx);

    while(flash_check_idle()){
        ;
    }
    flash_write_enable(true);

    ret = flash_access(addr + idx, &buffer[idx], this_write, false);
    if(ret){
        NRF_LOG_INFO("FLASH ACCESS\r\n");
        return ret;
    }
  }
  
  return FLASH_ERROR_NONE;
}

uint8_t flash_write_enable(bool enable)
{
  uint8_t tx_buf[1], rx_buf[1];
  
  tx_buf[0] = enable ? W25Q_WEN : W25Q_WDI;
  ACCESS_FLASH_DATA(tx_buf, rx_buf, 1);
  return FLASH_ERROR_NONE;
}

uint8_t flash_get_status(uint8_t *ptr_status, bool low)
{
  uint8_t tx_buf[2]={0, 0}, rx_buf[2]={0, 0};
  
  tx_buf[0] = low ? W25Q_SR_READ1 : W25Q_SR_READ2;
  ACCESS_FLASH_DATA(tx_buf, rx_buf, 2);
  *ptr_status = rx_buf[1];
  return FLASH_ERROR_NONE;
}

uint8_t flash_erase(uint8_t flag, uint32_t addr)
{
  uint8_t tx_buf[4], rx_buf[4];
  
  switch(flag)
  {
  case FLASH_ERASE_SECTOR:
    tx_buf[0] = W25Q_ERASE_SECTOR;
    break;
  case FLASH_ERASE_BLOCK_32:
    tx_buf[0] = W25Q_ERASE_BLOCK1;
    break;
  case FLASH_ERASE_BLOCK_64:
    tx_buf[0] = W25Q_ERASE_BLOCK2;
    break;
  default:
    return FLASH_ERROR_PARA;
  }
  
  addr &= ~((1UL << flag) - 1);
  NRF_LOG_INFO("erase addr %x\r\n", addr);
  tx_buf[1] = 0xFF & (addr >> 16);
  tx_buf[2] = 0xFF & (addr >> 8);
  tx_buf[3] = 0xFF & addr;
  while(flash_check_idle()) {
      ;
  } 
  flash_write_enable(true);
  ACCESS_FLASH_DATA(tx_buf, rx_buf, 4);
  
  return FLASH_ERROR_NONE;
}

uint8_t flash_chip_erase(void)
{
  uint8_t tx_buf[1] = {W25Q_ERASE_CHIP}, rx_buf[1];

  while(flash_check_idle()){
      ;
  }  
  flash_write_enable(true);
  
  ACCESS_FLASH_DATA(tx_buf, rx_buf, 1);
  
  return FLASH_ERROR_NONE;
}

uint8_t flash_check_idle(void)
{
  uint8_t status;
  
  flash_get_status(&status, true);
  
  return (status & W25Q_MASK_BSY);
}

uint8_t flash_power_down(void)
{
  uint8_t tx_buf[1] = {W25Q_POWER_DOWN}, rx_buf[1];
  
  ACCESS_FLASH_DATA(tx_buf, rx_buf, 1);
  
  return FLASH_ERROR_NONE;
}

uint8_t flash_wake_up(void)
{
  uint8_t tx_buf[1] = {W25Q_WAKE_UP}, rx_buf[1];
  
  ACCESS_FLASH_DATA(tx_buf, rx_buf, 1);
  
  return FLASH_ERROR_NONE;
}

