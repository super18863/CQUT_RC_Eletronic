#include "at24c02_test.h"
#include "uart_test.h"

IIC_Instance *iic1_ins;
IIC_Configs iic1;

void I2C1_init(void)
{
	iic1.handle = &hi2c1;
	iic1.dev_address = 0xA0;
	iic1.operation_mode = MEM_MODE;
	iic1.work_mode = IIC_BLOCK_MODE;
	iic1.module_rx_callback = NULL;
	iic1.module_tx_callback = NULL;
	
	iic1_ins = IIC_Register(&iic1);
	
}

void at24c02_transmit(uint16_t MemAddress, uint8_t *Data, uint16_t size)
{
	
	IIC_Mem_Transmit(iic1_ins, MemAddress, 1, Data, size);
//	HAL_I2C_Mem_Write(&hi2c1, 0xA0, 0, bit_flag, Data, size, 100)
	
}

void at24c02_receive(uint16_t MemAddress, uint8_t *Data, uint16_t size)
{
	IIC_Mem_Receive(iic1_ins, MemAddress, 1, Data, size);
}

uint8_t *read_at24c02(void)
{
	return iic1_ins->rx_buffer;
}

void at24c02_test(void)
{
	static uint8_t write_buff[10]={0,1};
	static uint8_t read_buff[10]={2};
 
	
  while (1)
  {
    
		at24c02_transmit(0 ,write_buff ,1);
		HAL_Delay(50);//直接读会失败需要延时
		at24c02_receive(0 ,read_buff ,1);

		printf("%d\n",*read_at24c02());

		HAL_Delay(500);
		write_buff[0]++;
	}
}
