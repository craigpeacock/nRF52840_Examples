
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <device.h>
#include <zephyr.h>
#include <drivers/i2c.h>

#include "ina226.h"
#include "shtc3.h"

void main(void)
{
	printk("I2C INA226 Current and Power Monitor\n");

	const struct device *dev;

  	printk("Opening I2C\n");
  	dev = device_get_binding("I2C_0");
  	if (!dev) {
		printk("I2C: Device driver not found.\n");
		return;
  	} else {
        i2c_configure(dev, I2C_SPEED_SET(I2C_SPEED_STANDARD));
  	}

	uint16_t temperature;
	uint16_t humidity;

	shtc3_readid(dev);

	while (1) {
		shtc3_wakeup(dev);
		k_msleep(1);
		shtc3_GetTempAndHumidity(dev, &temperature, &humidity);
		shtc3_sleep(dev);
		printk("Temp %.02f RH %.01f\r\n", shtc3_convert_temp(temperature), shtc3_convert_humd(humidity)); 
		k_msleep(1000);
	}

	ina226_init(dev);

	while (1) {

		printf("Voltage = %.03f V, ", ina226_voltage(dev));
		printf("Current = %.03f A, ", ina226_current(dev));
		printf("Power = %.02f W\r\n", ina226_power(dev));

		k_msleep(1000);
	}
}
