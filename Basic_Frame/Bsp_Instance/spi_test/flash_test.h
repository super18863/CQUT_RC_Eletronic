#ifndef __FLASH_TEST_H__
#define __FLASH_TEST_H__

#include "main.h"

//W25Q64指令集1
#define W25Q64_Write_Enable						          0x06
#define W25Q64_Write_Disable                              0x04
#define W25Q64_Read_Status_register_1				      0x05
#define W25Q64_Read_Status_register_2				      0x35
#define W25Q64_Write_Status_register				      0x01
#define W25Q64_Page_Program							      0x02
#define W25Q64_Quad_Page_Program				          0x32
#define W25Q64_Block_Erase_64KB						      0xD8
#define W25Q64_Block_Erase_32KB						      0x52
#define W25Q64_Sector_Erase_4KB						   	  0x20
#define W25Q64_Chip_Erase						 		  0xC7
#define W25Q64_Erase_Suspend			          		  0x75
#define W25Q64_Erase_Resume				      			  0x7A
#define W25Q64_Power_down							      0xB9
#define W25Q64_High_Performance_Mode			     	  0xA3
#define W25Q64_Continuous_Read_Mode_Reset			      0xFF
#define W25Q64_Release_Power_Down_HPM_Device_ID			  0xAB
#define W25Q64_Manufacturer_Device_ID				      0x90
#define W25Q64_Read_Uuique_ID						      0x4B
#define W25Q64_JEDEC_ID								      0x9F

//W25Q64指令集表2(读指令)
#define W25Q64_Read_Data						          0x03
#define W25Q64_Fast_Read						          0x0B
#define W25Q64_Fast_Read_Dual_Output				      0x3B
#define W25Q64_Fast_Read_Dual_IO					      0xBB
#define W25Q64_Fast_Read_Quad_Output				      0x6B
#define W25Q64_Fast_Read_Quad_IO					      0xEB
#define W25Q64_Octal_Word_Read_Quad_IO				      0xE3

#define W25Qx_Enable() 			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET)
#define W25Qx_Disable() 			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET)

extern SPI_HandleTypeDef hspi1;

void SPI1_Init(void);

void BSP_W25Qx_Read_ID(void);

#endif /* __FLASH_TEST_H__ */
