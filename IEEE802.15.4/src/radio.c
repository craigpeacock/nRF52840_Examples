/*
 * Copyright (c) 2023 Craig Peacock
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/kernel.h>
#include <zephyr/types.h>
#include <hal/nrf_radio.h>
#include <string.h>

#include "radio.h"

/* Buffer for the radio TX packet */
uint8_t tx_packet[RADIO_MAX_PAYLOAD_LEN];

/* Buffer for the radio RX packet. */
uint8_t rx_packet[RADIO_MAX_PAYLOAD_LEN];

/* Number of transmitted packets. */
uint32_t tx_packet_cnt;

/* Number of received packets with valid CRC. */
uint32_t rx_packet_cnt;

/* Total payload size */
static uint16_t total_payload_size;

void radio_rx(void)
{
	radio_disable();

	nrf_radio_shorts_enable(NRF_RADIO,
				NRF_RADIO_SHORT_READY_START_MASK |
				NRF_RADIO_SHORT_END_START_MASK);
	nrf_radio_packetptr_set(NRF_RADIO, rx_packet);

	nrf_radio_int_enable(NRF_RADIO, NRF_RADIO_INT_CRCOK_MASK);
	nrf_radio_task_trigger(NRF_RADIO, NRF_RADIO_TASK_RXEN);
}

void radio_tx(void)
{
	radio_disable();
	
	nrf_radio_shorts_enable(NRF_RADIO,
				NRF_RADIO_SHORT_READY_START_MASK |
				NRF_RADIO_SHORT_END_DISABLE_MASK);
	nrf_radio_packetptr_set(NRF_RADIO, tx_packet);

	tx_packet_cnt++;

	nrf_radio_int_enable(NRF_RADIO, NRF_RADIO_INT_END_MASK);
	nrf_radio_task_trigger(NRF_RADIO, NRF_RADIO_TASK_TXEN);

	printk("TX %03d: ", tx_packet_cnt);
	int len = tx_packet[0];
	for (int i = 0; i <= len; i++) {
		printk("%02X ", tx_packet[i]);
	}
	printk("\n");
}

void radio_disable(void)
{
	nrf_radio_shorts_set(NRF_RADIO, 0);
	nrf_radio_int_disable(NRF_RADIO, ~0);
	nrf_radio_event_clear(NRF_RADIO, NRF_RADIO_EVENT_DISABLED);

	nrf_radio_task_trigger(NRF_RADIO, NRF_RADIO_TASK_DISABLE);
	while (!nrf_radio_event_check(NRF_RADIO, NRF_RADIO_EVENT_DISABLED)) {
		/* Do nothing */
	}
	nrf_radio_event_clear(NRF_RADIO, NRF_RADIO_EVENT_DISABLED);
}

void radio_config(nrf_radio_mode_t mode, enum transmit_pattern pattern)
{
	nrf_radio_packet_conf_t packet_conf;

	rx_packet_cnt = 0;
	tx_packet_cnt = 0;

	nrf_radio_mode_set(NRF_RADIO, mode);

	/* Set fast ramp-up time. */
	nrf_radio_modecnf0_set(NRF_RADIO, true, RADIO_MODECNF0_DTX_Center);

	/* Disable CRC. */
	nrf_radio_crc_configure(NRF_RADIO, RADIO_CRCCNF_LEN_Disabled,
				NRF_RADIO_CRC_ADDR_INCLUDE, 0);

	/* Set the device address 0 to use when transmitting. */
	nrf_radio_txaddress_set(NRF_RADIO, 0);

	/* Enable the device address 0 to use to select which addresses to receive */
	nrf_radio_rxaddresses_set(NRF_RADIO, 1);

	/* Set the address according to the transmission pattern. */
	switch (pattern) {

		case TRANSMIT_PATTERN_RANDOM:
			nrf_radio_prefix0_set(NRF_RADIO, 0xAB);
			nrf_radio_base0_set(NRF_RADIO, 0xABABABAB);
			break;

		case TRANSMIT_PATTERN_11001100:
			nrf_radio_prefix0_set(NRF_RADIO, 0xCC);
			nrf_radio_base0_set(NRF_RADIO, 0xCCCCCCCC);
			break;

		case TRANSMIT_PATTERN_11110000:
			nrf_radio_prefix0_set(NRF_RADIO, 0x6A);
			nrf_radio_base0_set(NRF_RADIO, 0x58FE811B);
			break;

		default:
			return;
	}

	/* Packet configuration:
	 * payload length size = 8 bits,
	 * 0-byte static length, max 255-byte payload,
	 * 4-byte base address length (5-byte full address length),
	 * Bit 24: 1 Big endian,
	 * Bit 25: 1 Whitening enabled.
	 */
	memset(&packet_conf, 0, sizeof(packet_conf));
	packet_conf.lflen = RADIO_LENGTH_LENGTH_FIELD;
	packet_conf.maxlen = (sizeof(tx_packet) - 1);
	packet_conf.statlen = 0;
	packet_conf.balen = 4;
	packet_conf.big_endian = true;
	packet_conf.whiteen = true;

	switch (mode) {
#if CONFIG_HAS_HW_NRF_RADIO_IEEE802154
	case NRF_RADIO_MODE_IEEE802154_250KBIT:
		/* Packet configuration:
		 * S1 size = 0 bits,
		 * S0 size = 0 bytes,
		 * 32-bit preamble.
		 */
		packet_conf.plen = NRF_RADIO_PREAMBLE_LENGTH_32BIT_ZERO;
		packet_conf.maxlen = IEEE_MAX_PAYLOAD_LEN;
		packet_conf.balen = 0;
		packet_conf.big_endian = false;
		packet_conf.whiteen = false;

		/* Preamble, address (BALEN + PREFIX), lflen and payload */
		total_payload_size = 4 + (packet_conf.balen + 1) + 1 + packet_conf.maxlen;
		break;
#endif /* CONFIG_HAS_HW_NRF_RADIO_IEEE802154 */

#if CONFIG_HAS_HW_NRF_RADIO_BLE_CODED
	case NRF_RADIO_MODE_BLE_LR500KBIT:
	case NRF_RADIO_MODE_BLE_LR125KBIT:
		/* Packet configuration:
		 * S1 size = 0 bits,
		 * S0 size = 0 bytes,
		 * 10 bytes preamble.
		 */
		packet_conf.plen = NRF_RADIO_PREAMBLE_LENGTH_LONG_RANGE;
		packet_conf.cilen = 2;
		packet_conf.termlen = 3;
		packet_conf.big_endian = false;
		packet_conf.balen = 3;

		/* Set CRC length; CRC calculation does not include the address
		 * field.
		 */
		nrf_radio_crc_configure(NRF_RADIO, RADIO_CRCCNF_LEN_Three,
					NRF_RADIO_CRC_ADDR_SKIP, 0);

		/* Preamble, address (BALEN + PREFIX), lflen, code indicator, TERM, payload, CRC */
		total_payload_size = 10 + (packet_conf.balen + 1) + 1 + packet_conf.cilen +
				  packet_conf.termlen + packet_conf.maxlen + RADIO_CRCCNF_LEN_Three;
		break;

#endif /* CONFIG_HAS_HW_NRF_RADIO_BLE_CODED */

	case NRF_RADIO_MODE_BLE_2MBIT:
		/* Packet configuration:
		 * S1 size = 0 bits,
		 * S0 size = 0 bytes,
		 * 16-bit preamble.
		 */
		packet_conf.plen = NRF_RADIO_PREAMBLE_LENGTH_16BIT;

		/* preamble, address (BALEN + PREFIX), lflen and payload */
		total_payload_size = 2 + (packet_conf.balen + 1) + 1 + packet_conf.maxlen;
		break;

	default:
		/* Packet configuration:
		 * S1 size = 0 bits,
		 * S0 size = 0 bytes,
		 * 8-bit preamble.
		 */
		packet_conf.plen = NRF_RADIO_PREAMBLE_LENGTH_8BIT;

		/* preamble, address (BALEN + PREFIX), lflen, and payload */
		total_payload_size = 1 + (packet_conf.balen + 1) + 1 + packet_conf.maxlen;
		break;
	}

	nrf_radio_packet_configure(NRF_RADIO, &packet_conf);
}

void radio_set_channel(uint8_t channel)
{
	uint16_t frequency = 2400 + IEEE_FREQ_CALC(channel);
	printk("Channel %d, Frequency = %dMHz\n", channel, frequency);
	nrf_radio_frequency_set(NRF_RADIO, frequency);
}

void radio_enable_interrupt(void)
{
	irq_connect_dynamic(RADIO_IRQn, IRQ_PRIO_LOWEST, radio_handler, NULL, 0);
	irq_enable(RADIO_IRQn);
}

void radio_handler(const void *context)
{
	if (nrf_radio_event_check(NRF_RADIO, NRF_RADIO_EVENT_CRCOK)) {
		nrf_radio_event_clear(NRF_RADIO, NRF_RADIO_EVENT_CRCOK);
		rx_packet_cnt++;

		printk("RX %03d: ",rx_packet_cnt);
		int len = rx_packet[0];
		for (int i = 1; i <= len; i++) {
			printk("%02X ", rx_packet[i]);
		}
		printk("\n");
	}

	if (nrf_radio_event_check(NRF_RADIO, NRF_RADIO_EVENT_CRCERROR)) {
		nrf_radio_event_clear(NRF_RADIO, NRF_RADIO_EVENT_CRCERROR);
		printk("RX CRC Error\n");
	}

	if (nrf_radio_event_check(NRF_RADIO, NRF_RADIO_EVENT_END)) {
		nrf_radio_event_clear(NRF_RADIO, NRF_RADIO_EVENT_END);
		//printk("Radio Event End\n");
		// Return radio back to receive.
		radio_rx();
	}
}

void radio_power_set(int8_t power)
{
	nrf_radio_txpower_set(NRF_RADIO, (nrf_radio_txpower_t)power);
	printk("Tx power set to: %" PRIi8 " dBm\n", power);
}

