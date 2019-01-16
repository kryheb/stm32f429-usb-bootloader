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
#include "flash.h"
#include "ihexparser.h"

typedef enum {
  BOOTLOADER_STATE_IDLE,
  BOOTLOADER_STATE_INITIALIZATION,
  BOOTLOADER_STATE_INITIALIZED,
  BOOTLOADER_STATE_INITIALIZATION_FAILED,
  BOOTLOADER_STATE_USB_INITIALIZATION_FAILED,
  BOOTLOADER_STATE_ADDRESS_BASE_SET,
  BOOTLOADER_STATE_SET_ADDRESS_BASE_FAILED,
  BOOTLOADER_STATE_APPLICATION_ADDRESS_SET,
  BOOTLOADER_STATE_SET_APPLICATION_ADDRESS_FAILED,
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
  BootloaderState_t   state;
  USBCommHandle_t*    usb;
  FlashController_t*  flashController;
} Bootloader_t;

typedef void (*Application_t)(void);

OperationResult_t initialize(Bootloader_t* _bootloader);
void launch_application(Bootloader_t* _bootloader);


#endif /* BOOTLOADER_H_ */
