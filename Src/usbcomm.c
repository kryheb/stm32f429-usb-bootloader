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

extern Bootloader_t* bootloader;


void send_data()
{
  USBD_CUSTOM_HID_SendReport(bootloader->usb->usb_device_handle, bootloader->usb->buffer, bootloader->usb->buffer_len);
  bootloader->usb->state = USB_STATE_READY;
  bootloader->usb->data_in_pending = false;

}


static void prepare_resonse(USBInputCommand_t _in_command)
{
  bootloader->usb->buffer[HID_ADDR_INDEX] = CUSTOM_HID_EPOUT_ADDR;
  bootloader->usb->buffer[COMMAND_INDEX] = _in_command;
  bootloader->usb->buffer_len = 2;
  bootloader->usb->data_in_pending = true;
}


static void data_received(USBOutputCommand_t _out_command, uint8_t* _data)
{
  switch (_out_command) {
    case OUTPUT_COMMAND_INIT: {
      USBInputCommand_t rsp = (bootloader->state == BOOTLOADER_STATE_INITIALIZED) ?
          INPUT_COMMAND_INIT_OK : INPUT_COMMAND_INIT_NOK;
      prepare_resonse(rsp);
    }
    break;
    case OUTPUT_COMMAND_INITIALIZE_FLASH: {
      OperationResult_t res = initialize_flash(bootloader->flashController);
      USBInputCommand_t rsp = (res != OK) ?
              INPUT_COMMAND_FLASH_INITIALIZATION_NOK : INPUT_COMMAND_FLASH_INITIALIZATION_OK;
      prepare_resonse(rsp);
    }
    break;
    case OUTPUT_COMMAND_SET_ADDRESS_BASE: {
      HexRec_t hex_record;
      if (parse_record(&hex_record, _data, CUSTOM_HID_EPOUT_SIZE) != HEX_PARSER_OK) {
        bootloader->state = BOOTLOADER_STATE_SET_ADDRESS_BASE_FAILED;
      }
      if (hex_record.rtype != RTYPE_EXT_LINEAR_ADDR) {
        bootloader->state = BOOTLOADER_STATE_SET_ADDRESS_BASE_FAILED;
      }
      uint32_t addr = (hex_record.data[0] << 24) | (hex_record.data[1] << 16);
      OperationResult_t res = set_base_address(bootloader->flashController, addr);
      USBInputCommand_t rsp = (res != OK) ?
              INPUT_COMMAND_INVALID_ADDRESS_BASE : INPUT_COMMAND_ADDRESS_BASE_SET;
      prepare_resonse(rsp);
    }
    break;
    case OUTPUT_COMMAND_FLASH_DATA: {
      HexRec_t hex_record;
      if (parse_record(&hex_record, _data, CUSTOM_HID_EPOUT_SIZE) != HEX_PARSER_OK) {
        bootloader->state = BOOTLOADER_STATE_FLASHING_FAILED;
      }
      if (hex_record.rtype != RTYPE_DATA) {
        bootloader->state = BOOTLOADER_STATE_FLASHING_FAILED;
      }
      OperationResult_t res = flash_data(bootloader->flashController,
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

  bootloader->usb->data_out_pending = false;
}


static int8_t out_data_handler(uint8_t _ep_addr, uint8_t _cmd)
{
  USBD_CUSTOM_HID_HandleTypeDef *hhid =
      (USBD_CUSTOM_HID_HandleTypeDef*)bootloader->usb->usb_device_handle->pClassData;
  if (_ep_addr == CUSTOM_HID_EPOUT_ADDR && bootloader->usb->state == USB_STATE_READY
      && !bootloader->usb->data_out_pending) {

    bootloader->usb->data_out_pending = true;

    for (int i=0; i<CUSTOM_HID_EPOUT_SIZE; i++) {
      bootloader->usb->buffer[i] = hhid->Report_buf[i];
    }
    data_received(_cmd, bootloader->usb->buffer + DATA_OFFSET);
  }

  return USBD_OK;
}


USBCommHandle_t* create_usb_comm()
{
  USBCommHandle_t config = (USBCommHandle_t){
      .usb_device_handle = &hUsbDeviceHS,
      .state = USB_STATE_INITIALIZATION,
      .buffer_len = 0,
      .data_in_pending = false,
      .data_out_pending = false
  };

  USBCommHandle_t* usb_comm = malloc(sizeof(USBCommHandle_t));
  if (!usb_comm) {
    return NULL;
  }
  memcpy(usb_comm, (USBCommHandle_t*)(&config), sizeof(USBCommHandle_t));
  usb_comm->buffer = calloc(sizeof(uint8_t), CUSTOM_HID_EPOUT_SIZE);

  if (!usb_comm->buffer) {
    usb_comm->state = USB_STATE_INITIALIZATION_FAILED;
    return NULL;
  }
  return usb_comm;
}


OperationResult_t initialize_usbcomm(USBCommHandle_t* _usb_comm)
{
  register_out_data_handler(out_data_handler);
  MX_USB_DEVICE_Init();

  while(_usb_comm->usb_device_handle->dev_state != USBD_BUSY);

  _usb_comm->state =
      (_usb_comm->usb_device_handle->dev_state == USBD_FAIL) ?
          USB_STATE_INITIALIZATION_FAILED : USB_STATE_READY;

  if (_usb_comm->state != USB_STATE_READY) {
    return FAILED;
  }
  return OK;
}



