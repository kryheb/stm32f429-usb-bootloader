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

#define FLASH_BEGIN         0x08000000
#define SECTOR_NUM          12

void unlock_flash();
uint32_t get_sector_addr(uint8_t _sector);
BootloaderStatus_t erase_sector(uint8_t _sector);
BootloaderStatus_t program_data(uint32_t _addr, uint8_t* _pdata, uint16_t _size);


#endif /* FLASH_H_ */
