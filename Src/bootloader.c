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
  BootloaderConfig_t config = { .app_section_address=0x08000000, .base_address=0};
  _bootloader->config = malloc(sizeof(BootloaderConfig_t));
  memcpy(_bootloader->config, &config, sizeof(BootloaderConfig_t));
  _bootloader->state =
      (_bootloader->usb->state != USB_STATE_READY) ?
          BOOTLOADER_STATE_INITIALIZATION_FAILED : BOOTLOADER_STATE_INITIALIZED;
  led_init();
  _bootloader->state == BOOTLOADER_STATE_INITIALIZED? led_green_on() : led_red_on();

}


void set_base_address(Bootloader_t* _bootloader, uint32_t _addr)
{
  if (_addr >= _bootloader->config->app_section_address) {
    _bootloader->config->base_address = _addr;
    _bootloader->state = BOOTLOADER_STATE_ADDRESS_BASE_SET;
    return;
  }
  _bootloader->state = BOOTLOADER_STATE_SET_ADDRESS_BASE_FAILED;
}
