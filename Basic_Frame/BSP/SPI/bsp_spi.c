/**
 * @file bsp_spi.c
 * @author s.b.y (CQUT)
 * @brief 底层
 * @version 0.1
 * @date 2024-09-22
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "bsp_spi.h"
#include <stdlib.h>
#include <string.h>
#include "uart_test.h"

extern SPI_HandleTypeDef hspi1;

/* 所有的spi instance保存于此,用于callback时判断中断来源*/
static SPI_Instance *spi_instance[SPI_DEVICE_CNT] = {NULL};
static uint8_t idx = 0;                         // 配合中断以及初始化
uint8_t SPIDeviceOnGoing[SPI_DEVICE_CNT] = {1}; // 用于判断当前spi是否正在传输,防止多个模块同时使用一个spi总线 (0: 正在传输, 1: 未传输)


/**
 * @brief SPI设备注册函数
 * 
 * @param cfg 指向SPI配置结构体的指针
 * @return SPI_Instance* 指向新注册的SPI实例的指针
 */
SPI_Instance *SPI_Register(SPI_Config *cfg)
{
    if (idx >= MX_SPI_BUS_SLAVE_CNT) // 超过最大实例数
            while (1);
    
    SPI_Instance *ins = (SPI_Instance *)malloc(sizeof(SPI_Instance));
    memset(ins, 0, sizeof(SPI_Instance));

    ins->spi_handle = cfg->spi_handle;
    ins->GPIOx = cfg->GPIOx;
    ins->cs_pin = cfg->cs_pin;
    ins->spi_work_mode = cfg->spi_work_mode;
    ins->rx_callback = cfg->rx_callback;
    ins->tx_callback = cfg->tx_callback;

    if (ins->spi_handle->Instance == SPI1)
    {
        ins->cs_pin_state = &SPIDeviceOnGoing[0];
    }
    else if (ins->spi_handle->Instance == SPI2)
    {
        ins->cs_pin_state = &SPIDeviceOnGoing[1];
    }
    else
    {
        while (1);
    }
    spi_instance[idx++] = ins;
    return ins;
}


/**
 * @brief 发送数据
 * 
 * @param spi_ins 指向SPI实例的指针
 * @param ptr_data 指向要发送的数据的指针
 * @param len 数据长度
 */
void SPI_Transmit(SPI_Instance  *spi_ins, uint8_t *ptr_data, uint8_t len)
{
    // 拉低片选,开始传输(选中从机)
    HAL_GPIO_WritePin(spi_ins->GPIOx, spi_ins->cs_pin, GPIO_PIN_RESET);
    switch (spi_ins->spi_work_mode)
    {
    case SPI_DMA_MODE:
        HAL_SPI_Transmit_DMA(spi_ins->spi_handle, ptr_data, len);
        break;
    case SPI_IT_MODE:
        HAL_SPI_Transmit_IT(spi_ins->spi_handle, ptr_data, len);
        break;
    case SPI_BLOCK_MODE:
        HAL_SPI_Transmit(spi_ins->spi_handle, ptr_data, len, 1000); // 默认50ms超时
        // 阻塞模式不会调用回调函数,传输完成后直接拉高片选结束
        HAL_GPIO_WritePin(spi_ins->GPIOx, spi_ins->cs_pin, GPIO_PIN_SET);
        break;
    default:
        while (1)
            ; //错误处理不会写，相信后人的智慧
    }
}


/**
 * @brief 接收数据
 * 
 * @param spi_ins 指向SPI实例的指针
 * @param ptr_data 指向接收数据的缓冲区
 * @param len 数据长度
 */
void SPI_Recv(SPI_Instance *spi_ins, uint8_t *ptr_data, uint8_t len)
{
    // 用于稍后回调使用
    spi_ins->rx_size = len;
    spi_ins->rx_buffer = ptr_data;
    // 拉低片选,开始传输
    HAL_GPIO_WritePin(spi_ins->GPIOx, spi_ins->cs_pin, GPIO_PIN_RESET);
    switch (spi_ins->spi_work_mode)
    {
    case SPI_DMA_MODE:
        HAL_SPI_Receive_DMA(spi_ins->spi_handle, ptr_data, len);
        break;
    case SPI_IT_MODE:
        HAL_SPI_Receive_IT(spi_ins->spi_handle, ptr_data, len);
        break;
    case SPI_BLOCK_MODE:
        HAL_SPI_Receive(spi_ins->spi_handle, ptr_data, len, 1000);
        // 阻塞模式不会调用回调函数,传输完成后直接拉高片选结束
        HAL_GPIO_WritePin(spi_ins->GPIOx, spi_ins->cs_pin, GPIO_PIN_SET);
        break;
    default:
        while (1); //错误处理不会写，相信后人的智慧

    }
}


/**
 * @brief 同时发送和接收数据
 * 
 * @param spi_ins 指向SPI实例的指针
 * @param ptr_data_rx 指向接收数据的缓冲区
 * @param ptr_data_tx 指向要发送的数据的缓冲区
 * @param len 数据长度
 */
void SPI_TransRecv(SPI_Instance *spi_ins, uint8_t *ptr_data_rx, uint8_t *ptr_data_tx, uint8_t len)
{

    // 用于稍后回调使用,请保证ptr_data_rx在回调函数被调用之前仍然在作用域内,否则析构之后的行为是未定义的!!!
    spi_ins->rx_size = len;
    spi_ins->rx_buffer = ptr_data_rx;
    // 等待上一次传输完成
     if (spi_ins->spi_handle->Instance == SPI1)
    {
        while (!SPIDeviceOnGoing[0])
        {
        };
    }
    else if (spi_ins->spi_handle->Instance == SPI2)
    {
        while (!SPIDeviceOnGoing[1])
        {
        };
    } 
    // 拉低片选,开始传输
    HAL_GPIO_WritePin(spi_ins->GPIOx, spi_ins->cs_pin, GPIO_PIN_RESET);
    *spi_ins->cs_pin_state =
        spi_ins->CS_State =
            HAL_GPIO_ReadPin(spi_ins->GPIOx, spi_ins->cs_pin);
    switch (spi_ins->spi_work_mode)
    {
    case SPI_DMA_MODE:
        HAL_SPI_TransmitReceive_DMA(spi_ins->spi_handle, ptr_data_tx, ptr_data_rx, len);
        break;
    case SPI_IT_MODE:
        HAL_SPI_TransmitReceive_IT(spi_ins->spi_handle, ptr_data_tx, ptr_data_rx, len);
        break;
    case SPI_BLOCK_MODE:
        HAL_SPI_TransmitReceive(spi_ins->spi_handle, ptr_data_tx, ptr_data_rx, len, 1000); // 默认50ms超时

        // 阻塞模式不会调用回调函数,传输完成后直接拉高片选结束
        HAL_GPIO_WritePin(spi_ins->GPIOx, spi_ins->cs_pin, GPIO_PIN_SET);
        *spi_ins->cs_pin_state =
            spi_ins->CS_State =
                HAL_GPIO_ReadPin(spi_ins->GPIOx, spi_ins->cs_pin);
        break;
    default:
        while (1); //错误处理不会写，相信后人的智慧
    }
}


/**
 * @brief 在轮询阻塞模式下使用，发送指令后立即读取总线数据
 * 
 * @param spi_ins 指向SPI实例的指针
 * @param ptr_data_rx 指向接收数据的缓冲区
 * @param ptr_data_tx 指向要发送的数据的缓冲区
 * @param len_rx 接收数据长度
 * @param len_tx 发送数据长度
 */
void SPI_TransThenRecv(SPI_Instance *spi_ins, uint8_t *ptr_data_rx, uint8_t *ptr_data_tx, uint8_t len_rx , uint8_t len_tx)
{

    spi_ins->rx_buffer = ptr_data_rx;

    HAL_GPIO_WritePin(spi_ins->GPIOx, spi_ins->cs_pin, GPIO_PIN_RESET);

    switch (spi_ins->spi_work_mode)
    {
    case SPI_BLOCK_MODE:
        HAL_SPI_Transmit(spi_ins->spi_handle, ptr_data_tx, len_tx, 1000);
        HAL_SPI_Receive(spi_ins->spi_handle, ptr_data_rx, len_rx, 1000);
        HAL_GPIO_WritePin(spi_ins->GPIOx, spi_ins->cs_pin, GPIO_PIN_SET);
        break;
    default:
        while (1);  
    }
}


/**
 * @brief 当SPI接收完成,将会调用此回调函数,可以进行协议解析或其他必须的数据处理等
 *
 * @param hspi spi handle
 */
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
    for (size_t i = 0; i < idx; i++)
    {
        // 如果是当前spi硬件发出的complete,且cs_pin为低电平(说明正在传输),则尝试调用回调函数
        if (spi_instance[i]->spi_handle == hspi && // 显然同一时间一条总线只能有一个从机在接收数据
            HAL_GPIO_ReadPin(spi_instance[i]->GPIOx, spi_instance[i]->cs_pin) == GPIO_PIN_RESET)
        {
            // 先拉高片选,结束传输,在判断是否有回调函数,如果有则调用回调函数
            HAL_GPIO_WritePin(spi_instance[i]->GPIOx, spi_instance[i]->cs_pin, GPIO_PIN_SET);
            *spi_instance[i]->cs_pin_state =
                spi_instance[i]->CS_State =
                    HAL_GPIO_ReadPin(spi_instance[i]->GPIOx, spi_instance[i]->cs_pin);
            // @todo 后续添加holdon模式,由用户自行决定何时释放片选,允许进行连续传输
            if (spi_instance[i]->rx_callback != NULL) // 回调函数不为空, 则调用回调函数
                spi_instance[i]->rx_callback();
            return;
        }
    }
}


/**
 * @brief 和RxCpltCallback共用解析即可,这里只是形式上封装一下,不用重复写
 *        这是对HAL库的__weak函数的重写,传输使用IT或DMA模式,在传输完成时会调用此函数
 * @param hspi spi handle
 */
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
    HAL_SPI_RxCpltCallback(hspi); // 直接调用接收完成的回调函数
}
