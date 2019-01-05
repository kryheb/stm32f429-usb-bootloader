/*
 ============================================================================
 Name        : bootloader.c
 Author      : Krystian Heberlein
 Email       : krystianheberlein@gmail.com
 Description : Bootloader operations
 ============================================================================
 */


#include "bootloader.h"
#include "flash.h"
#include "ihexparser.h"

#include "led.h"

Bootloader_t* bootloader;


OperationResult_t initialize(Bootloader_t* _bootloader)
{
  _bootloader->state = BOOTLOADER_STATE_INITIALIZATION;
  USBCommHandle_t* usb = create_usb_comm();
  if (!usb) {
     _bootloader->state = BOOTLOADER_STATE_USB_INITIALIZATION_FAILED;
     return FAILED;
   }

  OperationResult_t res = initialize_usbcomm(usb);
  if (res != OK) {
    _bootloader->state = BOOTLOADER_STATE_INITIALIZATION_FAILED;
    return FAILED;
  }
  _bootloader->usb = usb;
   FlashController_t* fc = create_flash_controller();
  if (!fc) {
    _bootloader->state = BOOTLOADER_STATE_INITIALIZATION_FAILED;
    return FAILED;
  }
  _bootloader->flashController = fc;
  bootloader = _bootloader;
  _bootloader->state = BOOTLOADER_STATE_INITIALIZED;
  return OK;
}






