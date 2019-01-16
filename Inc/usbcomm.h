/*
 ============================================================================
 Name        : usbcomm.h
 Author      : Krystian Heberlein
 Email       : krystianheberlein@gmail.com
 Description : USB Communication layer interface
 ============================================================================
 */

#ifndef USBCOMM_H_
#define USBCOMM_H_


#include "usb_device.h"
#include <stdbool.h>
#include "defs.h"

#define HID_ADDR_INDEX  0
#define COMMAND_INDEX   1
#define DATA_OFFSET     2


typedef enum {
  INPUT_COMMAND_INIT_OK = 0x11,
  INPUT_COMMAND_INIT_NOK,
  INPUT_COMMAND_ADDRESS_BASE_SET = 0x21,
  INPUT_COMMAND_INVALID_ADDRESS_BASE,
  INPUT_COMMAND_FLASH_INITIALIZATION_OK = 0x31,
  INPUT_COMMAND_FLASH_INITIALIZATION_NOK,
  INPUT_COMMAND_FLASHING_OK = 0x41,
  INPUT_COMMAND_FLASHING_NOK,
  INPUT_COMMAND_SET_START_ADDRESS_OK = 0x51,
  INPUT_COMMAND_SET_START_ADDRESS_NOK,
  INPUT_COMMAND_UNKNOWN_COMMAND,

} USBInputCommand_t;

typedef enum {
  OUTPUT_COMMAND_INIT = 0x10,
  OUTPUT_COMMAND_SET_ADDRESS_BASE = 0x20,
  OUTPUT_COMMAND_INITIALIZE_FLASH = 0x30,
  OUTPUT_COMMAND_FLASH_DATA = 0x40,
  OUTPUT_COMMAND_SET_START_ADDRESS = 0x50,
  OUTPUT_COMMAND_LAUNCH_APPLICATION = 0x60
} USBOutputCommand_t;


typedef enum {
  USB_STATE_INITIALIZATION,
  USB_STATE_INITIALIZATION_FAILED,
  USB_STATE_READY,
  USB_STATE_BUSY
} USBState_t;

typedef struct {
  USBD_HandleTypeDef* usb_device_handle;
  USBState_t state;
  uint8_t* buffer;
  uint8_t buffer_len;
  bool data_out_pending;
  bool data_in_pending;
} USBCommHandle_t;

USBCommHandle_t* create_usb_comm();
OperationResult_t initialize_usbcomm(USBCommHandle_t* _usb_comm);
void send_data();

#endif /* USBCOMM_H_ */
