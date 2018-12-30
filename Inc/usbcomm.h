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

#define HID_ADDR_INDEX  0
#define COMMAND_INDEX   1


typedef enum {
  INPUT_COMMAND_INIT_OK = 0x11,
  INPUT_COMMAND_INIT_NOK,
  INPUT_COMMAND_ADDRESS_BASE_SET = 0x21,
  INPUT_COMMAND_INVALID_ADDRESS_BASE,
  INPUT_COMMAND_FLASH_INITIALIZATION_OK = 0x31,
  INPUT_COMMAND_FLASH_INITIALIZATION_NOK,
  INPUT_COMMAND_FLASHING_OK = 0x41,
  INPUT_COMMAND_FLASHING_NOK
} USBInputCommand_t;

typedef enum {
  OUTPUT_COMMAND_INIT = 0x10,
  OUTPUT_COMMAND_SET_ADDRESS_BASE = 0x20,
  OUTPUT_COMMAND_INITIALIZE_FLASH = 0x30,
  OUTPUT_COMMAND_FLASH_DATA = 0x40

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

extern USBCommHandle_t usbCommHandle;

void initialize_usbcomm();
void data_received();
void send_data(USBCommHandle_t* _usbCommHandle);

#endif /* USBCOMM_H_ */
