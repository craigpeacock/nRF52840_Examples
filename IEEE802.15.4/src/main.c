/*
 * Copyright (c) 2023 Craig Peacock
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <hal/nrf_radio.h>

#include "usbcdc.h"
#include "clocks.h"
#include "radio.h"

#define TRANSMIT_MESSAGES

void transmit_msg(void)
{
	uint8_t payload_size = 10;

	// First byte of packet contains length
	tx_packet[0] = payload_size;

	// Fill payload with sequential numbers
	for (int i = 1; i <= payload_size; i++) {
		tx_packet[i] = i;
	}
	
	// Tell radio to initiate transmit
	radio_tx();
}

void main(void)
{
	usbconsole_init();	// Initialse USB-CDC Console

	printk("IEEE802.15.4 Radio Example\nBoard: %s\n", CONFIG_BOARD);

	clock_init();		// Set-up HF clock

	radio_config(NRF_RADIO_MODE_IEEE802154_250KBIT, TRANSMIT_PATTERN_RANDOM);
	/* 
	 * Valid options for Mode:
	 *
	 * NRF_RADIO_MODE_NRF_1MBIT 			1Mbit/s Nordic proprietary radio mode.
	 * NRF_RADIO_MODE_NRF_2MBIT 			2Mbit/s Nordic proprietary radio mode.
	 * NRF_RADIO_MODE_NRF_250KBIT 			250Kbit/s Nordic proprietary radio mode.
	 * NRF_RADIO_MODE_BLE_1MBIT 			1 Mbit/s Bluetooth Low Energy.
	 * NRF_RADIO_MODE_BLE_2MBIT 			2 Mbit/s Bluetooth Low Energy.
	 * NRF_RADIO_MODE_BLE_LR125KBIT 		Bluetooth Low Energy Long range 125 kbit/s TX, 125 kbit/s and 500 kbit/s RX
	 * NRF_RADIO_MODE_BLE_LR500KBIT 		Bluetooth Low Energy Long range 500 kbit/s TX, 125 kbit/s and 500 kbit/s RX
	 * NRF_RADIO_MODE_IEEE802154_250KBIT 	IEEE 802.15.4-2006 250 kbit/s. 
	 * 
	 * */
	
	radio_set_channel(11);
	/*
	 * The IEEE 802.15.4 Standard defines 16 channels:
	 *
	 * Channel 11 - 2405 MHz
	 * Channel 12 - 2410 MHz
	 * Channel 13 - 2415 MHz
	 * Channel 14 - 2420 MHz
	 * Channel 15 - 2425 MHz
	 * Channel 16 - 2430 MHz
	 * Channel 17 - 2435 MHz
	 * Channel 18 - 2440 MHz 
	 * Channel 19 - 2445 MHz
	 * Channel 20 - 2450 MHz
	 * Channel 21 - 2455 MHz
	 * Channel 22 - 2460 MHz
	 * Channel 23 - 2465 MHz
	 * Channel 24 - 2470 MHz
	 * Channel 25 - 2475 MHz
	 * Channel 26 - 2480 MHz 
	 */

	radio_enable_interrupt();	// Enables interrupts and callback handler
	
	radio_rx();		// Start radio in receive mode

	while (1) {
#ifdef TRANSMIT_MESSAGES
		transmit_msg();
#endif
		k_msleep(5000);
	}
}
