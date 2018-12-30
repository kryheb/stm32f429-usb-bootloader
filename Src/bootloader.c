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
  FlashController_t config = { .app_section_address=0x08000000, .base_address=0};
  _bootloader->flashController = malloc(sizeof(FlashController_t));
  memcpy(_bootloader->flashController, &config, sizeof(FlashController_t));
  _bootloader->state =
      (_bootloader->usb->state != USB_STATE_READY) ?
          BOOTLOADER_STATE_INITIALIZATION_FAILED : BOOTLOADER_STATE_INITIALIZED;
  led_init();
  _bootloader->state == BOOTLOADER_STATE_INITIALIZED? led_green_on() : led_red_on();
}


void initialize_flash(Bootloader_t* _bootloader)
{
  _bootloader->state = BOOTLOADER_STATE_FLASH_INITIALIZATION;
  clear_prog_errs();
  unlock_flash();
  uint32_t sector = get_sector_from_addr(_bootloader->flashController->base_address);
  if (sector >= SECTOR_NUM) {
    _bootloader->state = BOOTLOADER_STATE_FLASH_INITIALIZATION_FAILED;
    return;
  }
  FlashControllerStatus_t fc_status = erase_sector(sector);
  _bootloader->state = (fc_status != FLASH_CONTROLLER_OK) ?
                          BOOTLOADER_STATE_FLASH_INITIALIZATION_FAILED :
                            BOOTLOADER_STATE_FLASH_INITIALIZED;
}


void set_base_address(Bootloader_t* _bootloader, uint32_t _addr)
{
  if (_bootloader->state == BOOTLOADER_STATE_SET_ADDRESS_BASE_FAILED) {
    return;
  }

  if (_addr >= _bootloader->flashController->app_section_address) {
    // FIXME: temp addr offset
    _bootloader->flashController->base_address = 0x800C000;
    _bootloader->state = BOOTLOADER_STATE_ADDRESS_BASE_SET;
    return;
  }
  _bootloader->state = BOOTLOADER_STATE_SET_ADDRESS_BASE_FAILED;
}


void flash_data(Bootloader_t* _bootloader, uint32_t _addr, uint8_t* _datap, size_t _size)
{
  if (_bootloader->state == BOOTLOADER_STATE_FLASHING_FAILED) {
    return;
  }

  _bootloader->state = BOOTLOADER_STATE_FLASHING;
  uint32_t addr = _bootloader->flashController->base_address + _addr;
  if(program_data(addr, _datap, _size) != FLASH_CONTROLLER_OK) {
    _bootloader->state = BOOTLOADER_STATE_FLASHING_FAILED;
    return;
  }

  _bootloader->state = BOOTLOADER_STATE_FLASHING_DONE;
}





