
int8_t i2c_write_short(const struct device *i2c_dev, uint8_t address, uint8_t command, uint16_t data);
int16_t i2c_read_short(const struct device *i2c_dev, uint8_t address, uint8_t command);