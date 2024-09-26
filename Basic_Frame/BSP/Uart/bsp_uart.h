#ifndef __BSP_UART_H__
#define __BSP_UART_H__

#include"main.h"
#include"stdio.h"
#include"string.h"

#define UART_RXBUFF_LIMIT 256 // 如果协议需要更大的buff,请修改这里
#define UART_Max_DEVICE  3   //A板至多分配三个串口

// 模块回调函数,用于解析协议
typedef void (*usart_module_callback)();


//发送模式枚举，没有接收模式枚举统一使用DMA接收
typedef enum {
    Transmit_BLOCK = 0U,
    Transmit_IT,
    Transmit_DMA
} Transmit_Mode;


typedef struct 
{
    uint8_t recv_buff[UART_RXBUFF_LIMIT]; // 预先定义的最大buff大小,如果太小请修改USART_RXBUFF_LIMIT
    uint8_t recv_buff_size;                // 模块接收一包数据的大小
    UART_HandleTypeDef *huartx;      // UART 句柄
    Transmit_Mode tx_mode;           // 传输模式
    usart_module_callback module_callback; // 解析收到的数据的回调函数
    
} Uart_Instance;


typedef struct 
{
    uint8_t recv_buff_size;                // 模块接收一包数据的大小
    UART_HandleTypeDef *huartx;      // UART 句柄
    Transmit_Mode tx_mode;           // 传输模式
    usart_module_callback module_callback; // 解析收到的数据的回调函数
    
} Uart_Configs;



Uart_Instance *Uart_Register(Uart_Configs *uart_cfg);

void Uart_Transmit(Uart_Instance *ins, uint8_t* message,uint16_t Size);

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size);



#endif /* __BSP_UART_H__ */
