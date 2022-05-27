
#include <stdio.h>
#include <string.h>
#include <device.h>
#include <zephyr.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/uuid.h>

#include <settings/settings.h>

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
};

void main(void)
{
	int err;

	printk("Beyondlogic Bluetooth LE Peripheral Example\n");

	err = bt_enable(NULL);
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}

	printk("Bluetooth initialized\n");

	if (IS_ENABLED(CONFIG_SETTINGS)) {
		settings_load();
	}

	err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
	if (err) {
		printk("Advertising failed to start (err %d)\n", err);
		return;
	}

	printk("Advertising successfully started\n");

	while (1) {
		k_msleep(1000);
	}
}
