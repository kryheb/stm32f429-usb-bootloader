/*
 ============================================================================
 Name        : usbcomm.c
 Author      : Krystian Heberlein
 Email       : krystianheberlein@gmail.com
 Description : USB Communication layer
 ============================================================================
 */

#include "usbcomm.h"
#include "usbd_customhid.h"
#include "bootloader.h"
#include "ihexparser.h"

USBCommHandle_t usbCommHandle;
Bootloader_t* bootloaderHandle;


void initialize_usbcomm(Bootloader_t* _bootloader)
{
  _bootloader->usb = &usbCommHandle;

  usbCommHandle = (USBCommHandle_t){
      .usb_device_handle = &hUsbDeviceHS,
      .state = USB_STATE_INITIALIZATION,
      .buffer_len = 0,
      .data_in_pending = false,
      .data_out_pending = false
  };

  usbCommHandle.buffer = calloc(sizeof(uint8_t), CUSTOM_HID_EPOUT_SIZE);

  if (!usbCommHandle.buffer) {
    usbCommHandle.state = USB_STATE_INITIALIZATION_FAILED;
    return;
  }

  MX_USB_DEVICE_Init();


  while(usbCommHandle.usb_device_handle->dev_state != USBD_BUSY);

  usbCommHandle.state =
      (usbCommHandle.usb_device_handle->dev_state == USBD_FAIL) ?
          USB_STATE_INITIALIZATION_FAILED : USB_STATE_READY;

  if (usbCommHandle.state == USB_STATE_READY) {
	  bootloaderHandle = _bootloader;
  }
}

void send_data(USBCommHandle_t* _usbCommHandle)
{
  USBD_CUSTOM_HID_SendReport(&hUsbDeviceHS, _usbCommHandle->buffer, _usbCommHandle->buffer_len);
  _usbCommHandle->state = USB_STATE_READY;
  _usbCommHandle->data_in_pending = false;

}


void data_received()
{
  int hid_addr = usbCommHandle.buffer[HID_ADDR_INDEX];
  if (hid_addr != CUSTOM_HID_EPOUT_ADDR) {
    return;
  }
  USBOutputCommand_t command = usbCommHandle.buffer[COMMAND_INDEX];
  switch (command) {
    case OUTPUT_COMMAND_INIT: {
      // TODO: create command wrapper for usbCommHandle
      usbCommHandle.buffer[HID_ADDR_INDEX] = CUSTOM_HID_EPOUT_ADDR;
      usbCommHandle.buffer[COMMAND_INDEX] = (usbCommHandle.state == USB_STATE_READY) ?
          INPUT_COMMAND_INIT_OK : INPUT_COMMAND_INIT_NOK;
      usbCommHandle.buffer_len = 2;
      usbCommHandle.data_in_pending = true;
    }
    break;
    case OUTPUT_COMMAND_INITIALIZE_FLASH: {
      initialize_flash(bootloaderHandle);

      usbCommHandle.buffer[HID_ADDR_INDEX] = CUSTOM_HID_EPOUT_ADDR;
      usbCommHandle.buffer[COMMAND_INDEX] =
          (bootloaderHandle->state == BOOTLOADER_STATE_FLASH_INITIALIZATION_FAILED) ?
              INPUT_COMMAND_FLASH_INITIALIZATION_NOK : INPUT_COMMAND_FLASH_INITIALIZATION_OK;
      usbCommHandle.buffer_len = 2;
      usbCommHandle.data_in_pending = true;
    }
    break;
    case OUTPUT_COMMAND_SET_ADDRESS_BASE: {
      HexRec_t hex_record;
      if (parse_record(&hex_record, usbCommHandle.buffer + 2, CUSTOM_HID_EPOUT_SIZE) != HEX_PARSER_OK) {
        bootloaderHandle->state = BOOTLOADER_STATE_SET_ADDRESS_BASE_FAILED;
      }
      if (hex_record.rtype != RTYPE_EXT_LINEAR_ADDR) {
        bootloaderHandle->state = BOOTLOADER_STATE_SET_ADDRESS_BASE_FAILED;
      }
      uint32_t addr = (hex_record.data[0] << 24) | (hex_record.data[1] << 16);
      set_base_address(bootloaderHandle, addr);

      usbCommHandle.buffer[HID_ADDR_INDEX] = CUSTOM_HID_EPOUT_ADDR;
      usbCommHandle.buffer[COMMAND_INDEX] =
          (bootloaderHandle->state == BOOTLOADER_STATE_SET_ADDRESS_BASE_FAILED) ?
              INPUT_COMMAND_INVALID_ADDRESS_BASE : INPUT_COMMAND_ADDRESS_BASE_SET;
      usbCommHandle.buffer_len = 2;
      usbCommHandle.data_in_pending = true;
    }
    break;
    case OUTPUT_COMMAND_FLASH_DATA: {
      HexRec_t hex_record;
      if (parse_record(&hex_record, usbCommHandle.buffer + 2, CUSTOM_HID_EPOUT_SIZE) != HEX_PARSER_OK) {
        bootloaderHandle->state = BOOTLOADER_STATE_FLASHING_FAILED;
      }
      if (hex_record.rtype != RTYPE_DATA) {
        bootloaderHandle->state = BOOTLOADER_STATE_FLASHING_FAILED;
      }
      flash_data(bootloaderHandle, hex_record.addr, hex_record.data, hex_record.size);

      usbCommHandle.buffer[HID_ADDR_INDEX] = CUSTOM_HID_EPOUT_ADDR;
      usbCommHandle.buffer[COMMAND_INDEX] =
          (bootloaderHandle->state == BOOTLOADER_STATE_FLASHING_FAILED) ?
              INPUT_COMMAND_FLASHING_NOK : INPUT_COMMAND_FLASHING_OK;
      usbCommHandle.buffer_len = 2;
      usbCommHandle.data_in_pending = true;
    }
    break;

    default:
      // TODO
      break;

  }

  usbCommHandle.data_out_pending = false;
}




