
#include "init.h"
#include "led.h"
#include "usbcomm.h"


int main(void)
{
  init();
  led_init();
  initialize_usbcomm();
  usbCommHandle.state == USB_STATE_READY ? led_green_on() : led_red_on();

  while (1)
  {
    if (usbCommHandle.data_in_pending) {
      send_data();
    }
  }

}
