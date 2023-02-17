/*
 * Copyright (c) 2023 Craig Peacock
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/drivers/uart.h>

void usbconsole_init(void) {

#ifdef CONFIG_USB_DEVICE_STACK
	/* 
	 * If the app is built with USB device support, redirect console to
	 * USB CDC. This is useful on the nRF52840 Dongle
	 * https://www.nordicsemi.com/Products/Development-hardware/nrf52840-dongle
	 */

	const struct device *const dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
	uint32_t dtr = 0;

	if (usb_enable(NULL)) {
		return;
	}

	/* Poll if the DTR flag was set */
	while (!dtr) {
		uart_line_ctrl_get(dev, UART_LINE_CTRL_DTR, &dtr);
		/* Give CPU resources to low priority threads. */
		k_sleep(K_MSEC(100));
	}
    
#endif

}