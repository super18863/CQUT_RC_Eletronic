#ifndef __UART_TEST_H__
#define __UART_TEST_H__

#include "bsp_uart.h"
#include "gpio_test.h"
#include "stdio.h"

extern UART_HandleTypeDef huart1;


void uart1_Init(void);

void Uart1_Transmit(uint8_t* message,uint16_t Size);

int fputc(int ch, FILE *f);


#endif /* __UART_TEST_H__ */
