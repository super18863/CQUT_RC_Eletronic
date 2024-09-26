#include "flash_test.h"
#include "uart_test.h"
#include "bsp_spi.h"
#include "bsp_gpio.h"

static SPI_Instance *spi1_ins;
static SPI_Config spi1;

void SPI1_Init(void)
{
    spi1.cs_pin = GPIO_PIN_0;
    spi1.GPIOx = GPIOC;
    spi1.spi_handle = &hspi1;
    spi1.spi_work_mode = SPI_BLOCK_MODE;
    spi1.rx_callback = NULL;
    spi1.tx_callback = NULL;
    spi1_ins = SPI_Register(&spi1);

}


void BSP_W25Qx_Read_ID(void)
{
	
	uint8_t cmd[4] = {0x90,0x00,0x00,0x00};
	uint8_t id[2] = {0};

	
	SPI_TransThenRecv(spi1_ins, id, cmd, 2 , 4);

	
	printf("%x %x\n",id[0],id[1]);    
		
}



