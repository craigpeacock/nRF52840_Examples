
#include <stdio.h>
#include <string.h>
#include <device.h>
#include <zephyr.h>
#include <sys/byteorder.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

#include <settings/settings.h>

#include <drivers/i2c.h>
#include "shtc3.h"


#define BT_UUID_BINARY_SENSOR			BT_UUID_DECLARE_16(0x183B)

#define BT_UUID_VOLTAGE 				BT_UUID_DECLARE_16(0x2B18)
#define BT_UUID_VOLTAGE_SPECIFICATION 	BT_UUID_DECLARE_16(0x2B19)

#define BT_UUID_ELECTIC_CURRENT			BT_UUID_DECLARE_16(0x2AEE)
#define BT_UUID_ELECTIC_CURRENT_RANGE
#define BT_UUID_ELECTIC_CURRENT_SPECIFICATION

#define BT_UUID_POWER					BT_UUID_DECLARE_16(0x2B05)
#define BT_UUID_POWER_SPECIFICATION		BT_UUID_DECLARE_16(0x2B06)


static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
};

#define SENSOR_NAME_TEMPERATURE	"SHTC3 Temperature Sensor"
#define SENSOR_NAME_HUMIDITY	"SHTC3 Humidity Sensor"
#define SENSOR_NAME_VOLTAGE		"INA226 Voltage Sensor"
#define SENSOR_NAME_CURRENT		"INA226 Current Sensor"

int16_t temperature;
int16_t humidity;

int16_t voltage;
int16_t current;

static ssize_t read_u16(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			void *buf, uint16_t len, uint16_t offset)
{
	const uint16_t *u16 = attr->user_data;
	uint16_t value = sys_cpu_to_le16(*u16);

	return bt_gatt_attr_read(conn, attr, buf, len, offset, &value, sizeof(value));
}

BT_GATT_SERVICE_DEFINE(ess_svc,
	BT_GATT_PRIMARY_SERVICE(BT_UUID_ESS),

	/* Temperature Sensor */
	BT_GATT_CHARACTERISTIC(BT_UUID_TEMPERATURE,
			       BT_GATT_CHRC_READ, 
			       BT_GATT_PERM_READ,
			       read_u16, NULL, &temperature),
	BT_GATT_CUD(SENSOR_NAME_TEMPERATURE, BT_GATT_PERM_READ),

	/* Humidity Sensor */
	BT_GATT_CHARACTERISTIC(BT_UUID_HUMIDITY, 
				   BT_GATT_CHRC_READ,
			       BT_GATT_PERM_READ,
			       read_u16, NULL, &humidity),
	BT_GATT_CUD(SENSOR_NAME_HUMIDITY, BT_GATT_PERM_READ),

	/* Voltage Sensor */
	BT_GATT_CHARACTERISTIC(BT_UUID_VOLTAGE,
					BT_GATT_CHRC_READ,
					BT_GATT_PERM_READ,
					read_u16, NULL, &voltage),
	BT_GATT_CUD(SENSOR_NAME_VOLTAGE, BT_GATT_PERM_READ),
	
	/* Current Sensor */
	BT_GATT_CHARACTERISTIC(BT_UUID_ELECTIC_CURRENT,
					BT_GATT_CHRC_READ,
					BT_GATT_PERM_READ,
					read_u16, NULL, &current),
	BT_GATT_CUD(SENSOR_NAME_CURRENT, BT_GATT_PERM_READ),

);

void main(void)
{
	int err;
	uint16_t raw_temp;
	uint16_t raw_humd;

	// Voltage has a resolution of 1/64V	
	voltage = 3.3 * 64;
	// Current has a resolution of 0.01. 100 = 1.00A
	current = 100;

	const struct device *dev;

	printk("Beyondlogic Bluetooth LE Peripheral Example\n");

  	printk("Opening I2C\n");
  	dev = device_get_binding("I2C_0");
  	if (!dev) {
		printk("I2C: Device driver not found.\n");
		return;
  	} else {
        i2c_configure(dev, I2C_SPEED_SET(I2C_SPEED_STANDARD));
  	}

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

	shtc3_readid(dev);

	while (1) {
		k_msleep(1000);
		
		shtc3_wakeup(dev);
		k_msleep(2);
		shtc3_GetTempAndHumidity(dev, &raw_temp, &raw_humd);
		shtc3_sleep(dev);
		
		temperature = shtc3_convert_temp_int16(raw_temp);
		humidity = shtc3_convert_humd_int16(raw_humd);
		printk("Temp %d.%02d, Humd %d.%02d \r\n", temperature/100, temperature%100, humidity/100, humidity%100); 

		bt_gatt_notify(NULL, &ess_svc.attrs[2], &temperature, sizeof(temperature));
		bt_gatt_notify(NULL, &ess_svc.attrs[4], &humidity, sizeof(humidity));
		bt_gatt_notify(NULL, &ess_svc.attrs[6], &voltage, sizeof(voltage));
		bt_gatt_notify(NULL, &ess_svc.attrs[8], &current, sizeof(current));
	}
}
