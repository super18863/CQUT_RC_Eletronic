#ifndef __AT24C02_TEST_H__
#define __AT24C02_TEST_H__

#include "bsp_iic.h"

extern I2C_HandleTypeDef hi2c1;



void I2C1_init(void);

void at24c02_transmit(uint16_t MemAddress ,uint8_t *Data ,uint16_t size);

void at24c02_receive(uint16_t MemAddress ,uint8_t *Data ,uint16_t size);

uint8_t *read_at24c02(void);

void at24c02_test(void);
#endif /* __AT24C02_TEST_H__ */
