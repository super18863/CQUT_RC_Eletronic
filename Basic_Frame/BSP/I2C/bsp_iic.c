/**
 * @file bsp_iic.c
 * @author s.b.y (CQUT)
 * @brief I2C底层
 * @version 0.1
 * @date 2024-09-20
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "bsp_iic.h"
#include <stdlib.h>
#include <string.h>

#include "uart_test.h"

static uint8_t idx;
static IIC_Instance *iic_instance[IIC_Max_DEVICE] = {NULL};


/**
 * @brief IIC设备注册函数
 * 
 * @param iic_cfg 指向IIC配置结构体的指针
 * @return IIC_Instance* 指向新注册的IIC实例的指针
 */
IIC_Instance *IIC_Register(IIC_Configs *iic_cfg)
{
    if (idx >= IIC_Max_DEVICE) // 超过最大实例数
        while (1);//错误处理不会写，相信后人的智慧

    for (uint8_t i = 0; i < idx; i++) // 检查是否已经注册过
        if (iic_instance[i]->handle == iic_cfg->handle )
            while (1);//错误处理不会写，相信后人的智慧

    IIC_Instance *ins = (IIC_Instance *)malloc(sizeof(IIC_Instance));
    memset(ins, 0, sizeof(IIC_Instance));
    
    ins->handle = iic_cfg->handle;
    ins->dev_address = iic_cfg->dev_address;
    ins->module_tx_callback = iic_cfg->module_tx_callback;
    ins->module_rx_callback = iic_cfg->module_rx_callback;
    ins->operation_mode = iic_cfg->operation_mode;
    ins->work_mode = iic_cfg->work_mode;

    iic_instance[idx++]=ins;

    return ins;
}


/**
 * @brief 向设备发送数据
 * 
 * @param ins 指向IIC实例的指针
 * @param data 指向要发送的数据的指针
 * @param size 数据大小
 */
void IIC_Transmit(IIC_Instance *ins, uint8_t *data, uint16_t size)
{
    if(ins->operation_mode != Master_MODE )
        while (1);//错误处理不会写，相信后人的智慧v

    switch (ins->work_mode)
    {
    case IIC_BLOCK_MODE:
        HAL_I2C_Master_Transmit(ins->handle, ins->dev_address, data, size, 100);
        break;
    case IIC_IT_MODE:
        HAL_I2C_Master_Transmit_IT(ins->handle, ins->dev_address, data, size);
        break;
    case IIC_DMA_MODE:
        HAL_I2C_Master_Transmit_DMA(ins->handle, ins->dev_address, data, size);
        break;
    default:
        while (1);
    }
}



/**
 * @brief 向设备特定地址写入数据
 * 
 * @param ins 指向IIC实例的指针
 * @param MemAddress 要写入的内存地址
 * @param mem8bit_flag 地址位数标志（8位或16位）
 * @param Data 指向要发送的数据的指针
 * @param Size 数据大小
 */
void IIC_Mem_Transmit(IIC_Instance *ins, uint16_t MemAddress, uint8_t mem8bit_flag, uint8_t *Data, uint16_t size)
{
    if(ins->operation_mode != MEM_MODE  )
        while (1);

    uint16_t bit_flag = mem8bit_flag ? I2C_MEMADD_SIZE_8BIT : I2C_MEMADD_SIZE_16BIT;

    switch (ins->work_mode)
    {
    case IIC_BLOCK_MODE:
        HAL_I2C_Mem_Write(ins->handle, ins->dev_address, MemAddress, bit_flag, Data, size, 100);				
        break;
    case IIC_IT_MODE:
        HAL_I2C_Mem_Write_IT(ins->handle, ins->dev_address, MemAddress, bit_flag, Data, size);
        break;
    case IIC_DMA_MODE:
        HAL_I2C_Mem_Write_DMA(ins->handle, ins->dev_address, MemAddress, bit_flag, Data, size);
        break;
    default:
        while (1);
    }
}


/**
 * @brief 从设备接收数据
 * 
 * @param ins 指向IIC实例的指针
 * @param data 指向接收数据的缓冲区
 * @param size 数据大小
 */
void IIC_Receive(IIC_Instance *ins, uint8_t *data, uint16_t size)
{
    if(ins->operation_mode != Master_MODE )
        while (1);

    ins->rx_buffer = data;
    ins->rx_len = size;

    switch (ins->work_mode)
    {
    case IIC_BLOCK_MODE:
        HAL_I2C_Master_Receive(ins->handle, ins->dev_address, data, size, 100);
        break;
    case IIC_IT_MODE:
        HAL_I2C_Master_Receive_IT(ins->handle, ins->dev_address, data, size);
        break;
    case IIC_DMA_MODE:
        HAL_I2C_Master_Receive_DMA(ins->handle, ins->dev_address, data, size);
        break;
    default:
        while (1);
    }
}


/**
 * @brief 从设备特定地址读取数据
 * 
 * @param ins 指向IIC实例的指针
 * @param MemAddress 要读取的内存地址
 * @param mem8bit_flag 地址位数标志（8位或16位）
 * @param Data 指向接收数据的缓冲区
 * @param size 数据大小
 */
void IIC_Mem_Receive(IIC_Instance *ins, uint16_t MemAddress, uint8_t mem8bit_flag, uint8_t *Data, uint16_t size)
{
    if(ins->operation_mode != MEM_MODE )
      while (1);

    uint16_t bit_flag = mem8bit_flag ? I2C_MEMADD_SIZE_8BIT : I2C_MEMADD_SIZE_16BIT;
    
    ins->rx_buffer = Data;
    ins->rx_len = size;

    switch (ins->work_mode)
    {
    case IIC_BLOCK_MODE:
        HAL_I2C_Mem_Read(ins->handle, ins->dev_address, MemAddress, I2C_MEMADD_SIZE_8BIT, Data, size, 100);
        break;
    case IIC_IT_MODE:
        HAL_I2C_Mem_Read_IT(ins->handle, ins->dev_address, MemAddress, bit_flag, Data, size);
        break;
    case IIC_DMA_MODE:
        HAL_I2C_Mem_Read_DMA(ins->handle, ins->dev_address, MemAddress, bit_flag, Data, size);
        break;
    default:
        while (1);
    }
}

// I2C传输完成回调
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    for(uint8_t i = 0; i < idx; ++i)
    {
       if (hi2c == iic_instance[i]->handle && iic_instance[i]->module_tx_callback != NULL)
       {
            iic_instance[i]->module_tx_callback();
            return;
            
        }     
    }
}


// I2C接收完成回调
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    for(uint8_t i = 0; i < idx; ++i)
    {
       if (hi2c == iic_instance[i]->handle && iic_instance[i]->module_rx_callback != NULL)
       {
            iic_instance[i]->module_rx_callback();
            return;
            
        }     
    }
}


// 内存写完成回调
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    for(uint8_t i = 0; i < idx; ++i)
    {
       if (hi2c == iic_instance[i]->handle && iic_instance[i]->module_tx_callback != NULL)
       {
            iic_instance[i]->module_tx_callback();
            return;
            
        }     
    }
}


void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    for(uint8_t i = 0; i < idx; ++i)
    {
       if (hi2c == iic_instance[i]->handle && iic_instance[i]->module_rx_callback != NULL)
       {
            iic_instance[i]->module_rx_callback();
            return;
            
        }     
    }
}
