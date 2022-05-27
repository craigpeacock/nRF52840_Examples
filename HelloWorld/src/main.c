
#include <stdio.h>
#include <string.h>
#include <device.h>
#include <zephyr.h>

void main(void)
{
	while (1) {
		printk("Hello World\n");
		k_msleep(1000);
	}
}
