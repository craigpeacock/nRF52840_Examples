
#include <zephyr.h>
#include <device.h>
#include <stdint.h>
#include <device.h>
#include <sys/printk.h>
#include <device.h>
#include <drivers/i2c.h>
#include <sys/byteorder.h>

int8_t i2c_write_short(const struct device *i2c_dev, uint8_t address, uint8_t command, uint16_t byte)
{
    int ret;
    uint8_t data[3];

    data[0] = command;
    data[1] = (byte & 0xFF00) >> 8;
    data[2] = byte & 0xFF;

    ret = i2c_write(i2c_dev, (uint8_t *)&data, 3, address);
    if (ret) {
        printk("Error writing to I2C error code (%d)\n", ret);
        return (-1);
    }
    return(0);
}

int16_t i2c_read_short(const struct device *i2c_dev, uint8_t address, uint8_t command)
{
    int ret;
    uint16_t data;
   
    ret = i2c_write_read(i2c_dev, address, &command, 1, &data, 2);
    if (ret) {
        printk("Error reading from I2C error code (%d)\n", ret);
        return (-1);
    }

    return(__bswap_16(data));
}


