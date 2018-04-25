
#include "init.h"
#include "usb_device.h"
#include "usbd_customhid.h"

int main(void)
{
	init();

	while (1)
	{
		uint8_t data[2] = {0x01, 0xDE};
		HAL_Delay(100);
		USBD_CUSTOM_HID_SendReport(&hUsbDeviceHS, (uint8_t*)&data, 2);
	}

}
