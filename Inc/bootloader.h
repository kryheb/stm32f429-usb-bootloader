/*
 ============================================================================
 Name        : bootloader.h
 Author      : Krystian Heberlein
 Email       : krystianheberlein@gmail.com
 Description : Bootloader operations
 ============================================================================
 */

#ifndef BOOTLOADER_H_
#define BOOTLOADER_H_

#include "defs.h"
#include "usbcomm.h"

typedef enum {
  BOOTLOADER_STATE_IDLE,
  BOOTLOADER_STATE_INITIALIZATION,
  BOOTLOADER_STATE_INITIALIZED,
  BOOTLOADER_STATE_INITIALIZATION_FAILED,
  BOOTLOADER_STATE_ADDRESS_BASE_SET,
  BOOTLOADER_STATE_SET_ADDRESS_BASE_FAILED,
  BOOTLOADER_STATE_FLASH_INITIALIZATION,
  BOOTLOADER_STATE_FLASH_INITIALIZED,
  BOOTLOADER_STATE_FLASH_INITIALIZATION_FAILED,
  BOOTLOADER_STATE_FLASHING,
  BOOTLOADER_STATE_FLASHING_FAILED,
  BOOTLOADER_STATE_FLASHING_READY,
  BOOTLOADER_STATE_FLASHING_DONE,
  BOOTLOADER_ERROR
} BootloaderState_t;

typedef struct {
  const uint32_t app_section_address;
  uint32_t base_address;
}BootloaderConfig_t;

typedef struct {
  BootloaderState_t state;
  USBCommHandle_t *usb;
  // flash controller
  // ihex parser
  BootloaderConfig_t* config;
} Bootloader_t;

void initialize(Bootloader_t* _bootloader);
void set_base_address(Bootloader_t* _bootloader, uint32_t _addr);

#endif /* BOOTLOADER_H_ */
