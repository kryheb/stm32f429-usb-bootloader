/*
 ============================================================================
 Name        : flash.h
 Author      : Krystian Heberlein
 Email       : krystianheberlein@gmail.com
 Description : Flash operations interface
 ============================================================================
 */

#ifndef FLASH_H_
#define FLASH_H_

#include "stm32f429xx.h"
#include "defs.h"

#define BL_FLASH_BEGIN         0x08000000
#define BL_FLASH_END           0x080FFFFF
#define SECTOR_NUM          12

#define SECTOR_SIZE_16KB    0x4000
#define SECTOR_SIZE_64KB    0x10000
#define SECTOR_SIZE_128KB   0x20000

typedef enum {
  FLASH_CONTROLLER_OK,
  FLASH_SECTOR_OUT_OF_RANGE,
  FLASH_PROG_SEQ_ERROR,
  FLASH_PROG_PARALLELISM_ERROR,
  FLASH_PROG_ALIGNMENT_ERROR,
  FLASH_WRITE_PROTECTION_ERROR,
  FLASH_PROG_ERROR,
  BUSY_CHECK_TIMEOUT
} FlashControllerStatus_t;

typedef struct {
  const uint32_t app_section_address;
  uint32_t base_address;
} FlashController_t;

void clear_prog_errs();
void unlock_flash();
uint8_t get_sector_from_addr(uint32_t _addr);
uint32_t get_addr_from_sector(uint8_t _sector);
FlashControllerStatus_t erase_sector(uint8_t _sector);
FlashControllerStatus_t program_data(uint32_t _addr, uint8_t* _pdata, uint16_t _size);


#endif /* FLASH_H_ */
