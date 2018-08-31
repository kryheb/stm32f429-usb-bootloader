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
  BOOTLOADER_STATE_CONFIGURING,
  BOOTLOADER_STATE_CONFIGURED,
  BOOTLOADER_STATE_CONFIGURATION_FAILED,
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
  BootloaderState_t state;
  USBCommHandle_t *usb;
  // flash controller
  // ihex parser
} Bootloader_t;

void initialize(Bootloader_t* _bootloader);

#endif /* BOOTLOADER_H_ */
