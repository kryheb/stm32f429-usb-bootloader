
#include "init.h"
#include "bootloader.h"

#include "led.h"

int main(void)
{
  init();

  Bootloader_t bootloader;
  led_init();
  OperationResult_t res = initialize(&bootloader);
  (res != OK) ? led_red_on() : led_green_on();

  while (1)
  {
    if (bootloader.usb->data_in_pending) {
      send_data();
    }
  }

}

