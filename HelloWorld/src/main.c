
#include <stdio.h>
#include <string.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>

void main(void)
{
	while (1) {
		printk("Hello World\n");
		k_msleep(1000);
	}
}
