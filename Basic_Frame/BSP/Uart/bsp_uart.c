/**
 * @file bsp_uart.c
 * @author s.b.y (CQUT)
 * @brief Uart底层,uart中断接收暂时只支持DMA模式
 * @version 0.1
 * @date 2024-09-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "bsp_uart.h"
#include <stdlib.h>
#include <string.h>
#include "gpio_test.h"

static uint8_t idx = 0;
static Uart_Instance *uart_instance[UART_Max_DEVICE] = {NULL};


/**
 * @brief 串口注册函数，每个串口赋予唯一ID
 * 
 * @param uart_cfg 
 */
Uart_Instance *Uart_Register(Uart_Configs *uart_cfg)
{
    if (idx >= UART_Max_DEVICE) // 超过最大实例数
        while (1);//错误处理不会写，相信后人的智慧

    for (uint8_t i = 0; i < idx; i++) // 检查是否已经注册过
        if (uart_instance[i]->huartx == uart_cfg->huartx)
            while (1);//错误处理不会写，相信后人的智慧


    Uart_Instance *ins = (Uart_Instance *)malloc(sizeof(Uart_Instance));
    memset(ins, 0, sizeof(Uart_Instance));

    ins->huartx = uart_cfg->huartx;
    ins->recv_buff_size = uart_cfg->recv_buff_size;
    ins->tx_mode = uart_cfg->tx_mode;
    ins->module_callback = uart_cfg->module_callback;
    
    uart_instance[idx++] = ins;

    //开启串口接收中断
				
    HAL_UARTEx_ReceiveToIdle_DMA(ins->huartx, ins->recv_buff, ins->recv_buff_size);
    __HAL_DMA_DISABLE_IT(ins->huartx->hdmarx, DMA_IT_HT);

    return ins;
}



/**
 * @brief 串口发送函数
 * 
 * @note  底层串口发送函数，只是形式上的封装
 * 
 * @param uart_cfg 要发送信息的串口
 * @param message 发送的信息
 * @param Size 发送信息的大小
 */
void Uart_Transmit(Uart_Instance *ins, uint8_t* message,uint16_t Size)
{
    switch (ins->tx_mode) {
        case Transmit_BLOCK:
            HAL_UART_Transmit(ins->huartx, message, Size, 100);
            break;
        case Transmit_IT:
            HAL_UART_Transmit_IT(ins->huartx, message, Size); 
            break;
        case Transmit_DMA:
            HAL_UART_Transmit_DMA(ins->huartx, message, Size); 
    }
}


/**
 * @brief 每次dma/idle中断发生时，都会调用此函数.对于每个串口会调用对应的回调进行进一步的处理
 *
 * @note  通过__HAL_DMA_DISABLE_IT(huart->hdmarx,DMA_IT_HT)关闭dma half transfer中断防止两次进入HAL_UARTEx_RxEventCallback()
 *        这是HAL库的一个设计失误,发生DMA传输完成/半完成以及串口IDLE中断都会触发HAL_UARTEx_RxEventCallback()
 *        我们只希望处理，因此直接关闭DMA半传输中断第一种和第三种情况
 * 
 * @param huart 发生中断的串口
 * @param Size 此次接收到的总数居量,暂时没用
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
						
    for (uint8_t i = 0; i < idx; ++i)
    { 
        if (huart == uart_instance[i]->huartx && uart_instance[i]->module_callback != NULL)
        { 
            uart_instance[i]->module_callback();
            memset(uart_instance[i]->recv_buff, 0, Size); // 接收结束后清空buffer,对于变长数据是必要的

            HAL_UARTEx_ReceiveToIdle_DMA(uart_instance[i]->huartx, uart_instance[i]->recv_buff, uart_instance[i]->recv_buff_size);
            __HAL_DMA_DISABLE_IT(uart_instance[i]->huartx->hdmarx, DMA_IT_HT);
           
            return; // break the loop
        }
    }
}
