#ifndef _MPU_PORT_H_
#define _MPU_PORT_H_



void ns_delay_ms(uint32_t dly);
void ns_get_clock_ms(unsigned long *count);
uint8_t ns_i2c_write(uint8_t dev_addr, uint8_t reg_addr, uint16_t i2c_len, uint8_t *i2c_data_buf);
uint8_t ns_i2c_read(uint8_t dev_addr, uint8_t reg_addr, uint16_t i2c_len, uint8_t *i2c_data_buf);

#endif //_MPU_PORT_H_
