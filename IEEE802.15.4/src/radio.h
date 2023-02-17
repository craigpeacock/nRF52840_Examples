/*
 * Copyright (c) 2023 Craig Peacock
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/**@brief Radio transmit and address pattern. */
enum transmit_pattern {
	/** Random pattern. */
	TRANSMIT_PATTERN_RANDOM,

	/** Pattern 11110000(F0). */
	TRANSMIT_PATTERN_11110000,

	/** Pattern 11001100(CC). */
	TRANSMIT_PATTERN_11001100,
};

void radio_rx(void);
void radio_tx(void);
void radio_disable(void);
void radio_config(nrf_radio_mode_t mode, enum transmit_pattern pattern);
void radio_set_channel(uint8_t channel);
void radio_enable_interrupt(void);
void radio_handler(const void *context);
void radio_power_set(int8_t power);

/** Maximum radio RX or TX payload. */
#define RADIO_MAX_PAYLOAD_LEN	256

/** IEEE 802.15.4 maximum payload length. */
#define IEEE_MAX_PAYLOAD_LEN	127

/** IEEE 802.15.4 minimum channel. */
#define IEEE_MIN_CHANNEL	11

/** IEEE 802.15.4 maximum channel. */
#define IEEE_MAX_CHANNEL	26

/* Length on air of the LENGTH field. */
#define RADIO_LENGTH_LENGTH_FIELD (8UL)

/* IEEE 802.15.4 default frequency. */
#define IEEE_DEFAULT_FREQ         (5)

/* 
 * Frequency calculation for a given channel in the IEEE 802.15.4 radio
 * mode.
 */
#define IEEE_FREQ_CALC(_channel) (IEEE_DEFAULT_FREQ + \
				 (IEEE_DEFAULT_FREQ * \
				 ((_channel) - IEEE_MIN_CHANNEL)))

/* Frequency calculation for a given channel. */
#define CHAN_TO_FREQ(_channel) (2400 + _channel)

/* Buffer for the radio TX packet */
extern uint8_t tx_packet[];

/* Buffer for the radio RX packet. */
extern uint8_t rx_packet[];





