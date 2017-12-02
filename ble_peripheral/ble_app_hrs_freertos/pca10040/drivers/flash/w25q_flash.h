#ifndef BSP_FLASH_H__
#define BSP_FLASH_H__

#include <stdbool.h>
#include <stdint.h>

#define  FLASH_ERASE_SECTOR        12
#define  FLASH_ERASE_BLOCK_32      15
#define  FLASH_ERASE_BLOCK_64      16

#define  FLASH_ERROR_NONE          0x00
#define  FLASH_ERROR_BUS           0x01
#define  FLASH_ERROR_BUSY          0x02
#define  FLASH_ERROR_WEL           0x04
#define  FLASH_ERROR_MEM           0x08
#define  FLASH_ERROR_PARA          0x10

// === W25Q MASKS ===

#define W25Q_MASK_BSY      (0x01 << 0)
#define W25Q_MASK_WEL      (0x01 << 1)

#define W25Q_PAGE_SIZE     0x100
#define W25Q_MEM_SIZE      0x100000

uint8_t  flash_init(void);

void flash_uninit(void);

uint8_t  flash_read(uint32_t addr, uint8_t * buffer, uint16_t len);

uint8_t  flash_program(uint32_t addr, uint8_t * buffer, uint16_t len);

uint8_t  flash_write_enable(bool enable);

uint8_t  flash_erase(uint8_t flag, uint32_t addr);

uint8_t  flash_chip_erase(void);

uint8_t  flash_get_status(uint8_t *ptr_status, bool low);

uint8_t  flash_check_idle(void);

uint8_t  flash_power_down(void);

uint8_t  flash_wake_up(void);

#endif
