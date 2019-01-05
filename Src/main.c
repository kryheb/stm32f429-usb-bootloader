
#include "init.h"
#include "bootloader.h"

#include "led.h"

int main(void)
{
  init();

  Bootloader_t bootloader;
  initialize(&bootloader);

  while (1)
  {
    if (bootloader.usb->data_in_pending) {
      send_data();
    }
  }

}

