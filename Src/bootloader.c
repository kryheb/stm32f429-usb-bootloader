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


void initialize(Bootloader_t* _bootloader)
{
  _bootloader->state = BOOTLOADER_STATE_INITIALIZATION;
  initialize_usbcomm(_bootloader);
  _bootloader->state =
      (_bootloader->usb->state != USB_STATE_READY) ?
          BOOTLOADER_STATE_INITIALIZATION_FAILED : BOOTLOADER_STATE_INITIALIZED;
  led_init();
  _bootloader->state == BOOTLOADER_STATE_INITIALIZED? led_green_on() : led_red_on();
}
