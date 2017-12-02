#ifndef FLASH_SPI_MASTER_H__
#define FLASH_SPI_MASTER_H__

#include <stdint.h>

void flash_spi_master_init(void);
void flash_spi_master_uninit(void);
void flash_spi_send_recv(uint8_t * const p_tx_data, uint8_t * const p_rx_data, const uint16_t len);

#endif	/* FLASH_SPI_MASTER_H__ */
