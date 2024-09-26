#include "uart_test.h"

static Uart_Configs uart1;
static Uart_Instance *uart1_ins;

void Uart1_Transmit(uint8_t* message,uint16_t Size)
{
  
    Uart_Transmit(uart1_ins, message, Size);
	
}

int fputc(int ch, FILE *f)
{

	Uart1_Transmit((uint8_t *)&ch,sizeof(ch));

	return (ch);
}

void uart1_rx(void)
{
	b1_toogle();
	printf("%c",*(uart1_ins->recv_buff));

		
}

void uart1_Init(void)
{
    uart1.huartx = &huart1;
    uart1.tx_mode = Transmit_BLOCK;
    uart1.recv_buff_size = 1;
    uart1.module_callback = uart1_rx;
    uart1_ins=Uart_Register(&uart1);
}

