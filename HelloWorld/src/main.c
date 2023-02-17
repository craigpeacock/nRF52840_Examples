
#include <stdio.h>
#include <string.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>

#include "usbcdc.h"

void main(void)
{
	usbconsole_init();	// Initialse USB-CDC Console

	while (1) {
		printk("Hello World\n");
		k_msleep(1000);
	}
}
