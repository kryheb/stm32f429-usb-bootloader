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

USBCommHandle_t usbCommHandle;


void initialize_usbcomm()
{
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

  USBState_t state = USB_STATE_INITIALIZATION_FAILED;

  if (usbCommHandle.usb_device_handle->dev_state != USBD_FAIL) {
    state = USB_STATE_READY;
  }

  usbCommHandle.state = state;
}

void send_data()
{
  USBD_CUSTOM_HID_SendReport(&hUsbDeviceHS, usbCommHandle.buffer, usbCommHandle.buffer_len);
  usbCommHandle.state = USB_STATE_READY;

}


void data_received()
{
  USBOutputCommand_t command = usbCommHandle.buffer[1];
  if (command == OUTPUT_COMMAND_INIT) {
    usbCommHandle.buffer[0] = 0x01;
    usbCommHandle.buffer[1] = (usbCommHandle.state == USB_STATE_READY) ? INPUT_COMMAND_INIT_OK : INPUT_COMMAND_INIT_NOK;
    usbCommHandle.buffer_len = 2;
    usbCommHandle.data_in_pending = true;
  }

  usbCommHandle.data_out_pending = false;
}




