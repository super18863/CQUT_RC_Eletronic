#ifndef __BSP_SPI_H__
#define __BSP_SPI_H__

#include"main.h"

#include "spi.h"
#include "stdint.h"
#include "gpio.h"

/* 根据开发板引出的spi引脚以及CubeMX中的初始化配置设定 */
#define SPI_DEVICE_CNT 2       // C型开发板引出两路spi,分别连接BMI088/作为扩展IO在8pin牛角座引出
#define MX_SPI_BUS_SLAVE_CNT 4 // 单个spi总线上挂载的从机数目

typedef void (*spi_tx_callback)();
typedef void (*spi_rx_callback)();

/* spi transmit recv mode enumerate*/
typedef enum
{
    SPI_BLOCK_MODE = 0, // 默认使用阻塞模式
    SPI_IT_MODE,
    SPI_DMA_MODE,
} SPI_MODE;

/* SPI实例结构体定义 */
typedef struct spi_ins_temp
{
    SPI_HandleTypeDef *spi_handle; // SPI外设handle
    GPIO_TypeDef *GPIOx;           // 片选信号对应的GPIO,如GPIOA,GPIOB等等
    uint16_t cs_pin;               // 片选信号对应的引脚号,GPIO_PIN_1,GPIO_PIN_2等等

    SPI_MODE spi_work_mode; // 传输工作模式
    uint8_t rx_size;               // 本次接收的数据长度
    uint8_t *rx_buffer;            // 本次接收的数据缓冲区
    uint8_t CS_State;              // 片选信号状态,用于中断模式下的片选控制
    uint8_t * cs_pin_state;        // 片选信号状态,用于中断模式下的片选控制
    spi_tx_callback tx_callback; //模块回调函数
    spi_rx_callback rx_callback;  
    
} SPI_Instance;


// @todo: 这里可以将GPIO_TypeDef *GPIOx; uint16_t cs_pin合并为bsp_gpio以简化代码实现
typedef struct
{
    SPI_HandleTypeDef *spi_handle; // SPI外设handle
    GPIO_TypeDef *GPIOx;           // 片选信号对应的GPIO,如GPIOA,GPIOB等等
    uint16_t cs_pin;               // 片选信号对应的引脚号,GPIO_PIN_1,GPIO_PIN_2等等

    SPI_MODE spi_work_mode; // 传输工作模式

    spi_tx_callback tx_callback; //模块回调函数
    spi_rx_callback rx_callback;
} SPI_Config;


SPI_Instance *SPI_Register(SPI_Config *conf);


void SPI_Transmit(SPI_Instance *spi_ins, uint8_t *ptr_data, uint8_t len);


void SPI_Recv(SPI_Instance *spi_ins, uint8_t *ptr_data, uint8_t len);


void SPI_TransRecv(SPI_Instance *spi_ins, uint8_t *ptr_data_rx, uint8_t *ptr_data_tx, uint8_t len);


void SPI_SetMode(SPI_Instance *spi_ins, SPI_MODE spi_mode);

void SPI_TransThenRecv(SPI_Instance *spi_ins, uint8_t *ptr_data_rx, uint8_t *ptr_data_tx, uint8_t len_rx , uint8_t len_tx);


#endif /* __BSP_SPI_H__ */
