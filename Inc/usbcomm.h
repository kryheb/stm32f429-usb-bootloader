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


typedef enum {
  INPUT_COMMAND_INIT_OK = 0x11,
  INPUT_COMMAND_INIT_NOK
} USBInputCommand_t;

typedef enum {
  OUTPUT_COMMAND_INIT = 0x10,

} USBOutputCommand_t;


typedef enum {
  USB_STATE_INITIALIZATION,
  USB_STATE_INITIALIZATION_FAILED,
  USB_STATE_READY
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
void send_data();

#endif /* USBCOMM_H_ */
