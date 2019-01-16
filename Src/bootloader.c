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


void launch_application(Bootloader_t* _bootloader)
{
  Application_t application = (Application_t)_bootloader->flashController->application_start_address;
  uint32_t appStack = (uint32_t) *((__IO uint32_t*) _bootloader->flashController->application_address);
  SCB->VTOR = _bootloader->flashController->application_address;
  __set_MSP(appStack);
  application();
}
