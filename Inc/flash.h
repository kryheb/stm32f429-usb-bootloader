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
#include <stddef.h>

#define BL_FLASH_BEGIN         0x08000000
#define BL_FLASH_END           0x080FFFFF
#define SECTOR_NUM          12

#define SECTOR_SIZE_16KB    0x4000
#define SECTOR_SIZE_64KB    0x10000
#define SECTOR_SIZE_128KB   0x20000

#define RESET_VECTOR_OFFSET 4


typedef enum {
  FLASH_CONTROLLER_OK,
  FLASH_CONTROLLER_BUSY,
  FLASH_CONTROLLER_UNCONFIGURED,
  FLASH_CONTROLLER_INVALID_BASE_ADDRESS,
  FLASH_CONTROLLER_INVALID_SECTOR,
  FLASH_SECTOR_OUT_OF_RANGE,
  FLASH_PROG_SEQ_ERROR,
  FLASH_PROG_PARALLELISM_ERROR,
  FLASH_PROG_ALIGNMENT_ERROR,
  FLASH_WRITE_PROTECTION_ERROR,
  FLASH_PROG_ERROR,
  BUSY_CHECK_TIMEOUT
} FlashState_t;

typedef struct {
  uint32_t app_section_address;
  uint32_t base_address;
  uint32_t application_address;
  uint32_t application_start_address;
  FlashState_t state;
} FlashController_t;


FlashController_t* create_flash_controller();
OperationResult_t initialize_flash(FlashController_t* _flash_controller);
OperationResult_t set_base_address(FlashController_t* _flash_controller, uint32_t _addr);
OperationResult_t set_application_address(FlashController_t* _flash_controller, uint32_t _addr);
OperationResult_t flash_data(FlashController_t* _flash_controller, uint32_t _addr_offset, uint8_t* _datap, size_t _size);



#endif /* FLASH_H_ */
