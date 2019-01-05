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
#include "usbd_custom_hid_if.h"
#include "bootloader.h"
#include "ihexparser.h"

USBCommHandle_t usbCommHandle;
Bootloader_t* bootloaderHandle;


void send_data()
{
  USBD_CUSTOM_HID_SendReport(usbCommHandle.usb_device_handle, usbCommHandle.buffer, usbCommHandle.buffer_len);
  usbCommHandle.state = USB_STATE_READY;
  usbCommHandle.data_in_pending = false;

}


static void prepare_resonse(USBInputCommand_t _in_command)
{
  usbCommHandle.buffer[HID_ADDR_INDEX] = CUSTOM_HID_EPOUT_ADDR;
  usbCommHandle.buffer[COMMAND_INDEX] = _in_command;
  usbCommHandle.buffer_len = 2;
  usbCommHandle.data_in_pending = true;
}


static void data_received(USBOutputCommand_t _out_command)
{
  switch (_out_command) {
    case OUTPUT_COMMAND_INIT: {
      USBInputCommand_t rsp = (usbCommHandle.state == USB_STATE_READY) ?
          INPUT_COMMAND_INIT_OK : INPUT_COMMAND_INIT_NOK;
      prepare_resonse(rsp);
    }
    break;
    case OUTPUT_COMMAND_INITIALIZE_FLASH: {
      OperationResult_t res = initialize_flash(bootloaderHandle->flashController);
      USBInputCommand_t rsp = (res != OK) ?
              INPUT_COMMAND_FLASH_INITIALIZATION_NOK : INPUT_COMMAND_FLASH_INITIALIZATION_OK;
      prepare_resonse(rsp);
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
      FlashState_t res = set_base_address(bootloaderHandle->flashController, addr);
      USBInputCommand_t rsp = (res != OK) ?
              INPUT_COMMAND_INVALID_ADDRESS_BASE : INPUT_COMMAND_ADDRESS_BASE_SET;
      prepare_resonse(rsp);
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
      OperationResult_t res = flash_data(bootloaderHandle->flashController,
                                          hex_record.addr, hex_record.data, hex_record.size);
      USBInputCommand_t rsp = (res != OK) ?
              INPUT_COMMAND_FLASHING_NOK : INPUT_COMMAND_FLASHING_OK;
      prepare_resonse(rsp);
    }
    break;

    default:
      // TODO
      break;

  }

  usbCommHandle.data_out_pending = false;
}


static int8_t out_data_handler(uint8_t _ep_addr, uint8_t _cmd)
{
  USBD_CUSTOM_HID_HandleTypeDef *hhid =
      (USBD_CUSTOM_HID_HandleTypeDef*)usbCommHandle.usb_device_handle->pClassData;
  if (_ep_addr == CUSTOM_HID_EPOUT_ADDR && usbCommHandle.state == USB_STATE_READY
      && !usbCommHandle.data_out_pending) {

    usbCommHandle.data_out_pending = true;

    for (int i=0; i<CUSTOM_HID_EPOUT_SIZE; i++) {
      usbCommHandle.buffer[i] = hhid->Report_buf[i];
    }
    data_received(_cmd);
  }

  return USBD_OK;
}


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

  register_out_data_handler(out_data_handler);

  MX_USB_DEVICE_Init();

  while(usbCommHandle.usb_device_handle->dev_state != USBD_BUSY);

  usbCommHandle.state =
      (usbCommHandle.usb_device_handle->dev_state == USBD_FAIL) ?
          USB_STATE_INITIALIZATION_FAILED : USB_STATE_READY;

  if (usbCommHandle.state == USB_STATE_READY) {
    bootloaderHandle = _bootloader;
  }
}



