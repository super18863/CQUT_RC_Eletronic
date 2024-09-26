#ifndef __BSP_IIC_H__
#define __BSP_IIC_H__

#include"main.h"
#include "i2c.h"

#define IIC_Max_DEVICE    2 //最大iic个数，根据需要修改

typedef void (*iic_module_callback)();

typedef enum
{
    Master_MODE = 0, // 主机模式
    MEM_MODE        // 存储模式

} IIC_Operation_Mode;

typedef enum
{
    IIC_BLOCK_MODE = 0, // 阻塞模式
    IIC_IT_MODE,        // 中断模式
    IIC_DMA_MODE,       // DMA模式

} IIC_Work_Mode;


/* i2c实例 */
typedef struct
{
    I2C_HandleTypeDef *handle; // i2c handle
    uint8_t dev_address;       // 暂时只支持7位地址(还有一位是读写位)
    IIC_Work_Mode work_mode;             // 工作模式
    IIC_Operation_Mode operation_mode;
    iic_module_callback module_tx_callback; //回调函数
    iic_module_callback module_rx_callback; //回调函数
    uint8_t *rx_buffer;                    // 接收缓冲区指针
    uint8_t rx_len;                        // 接收长度

    //中断模式下的标志位，想不出判断逻辑了，相信后人的智慧，暂时一条i2c总线只能挂载一个设备
    //uint8_t tx_start_flag;
    //uint8_t rx_start_flag;
    //uint8_t tx_flag;
    //uint8_t rx_flag;

} IIC_Instance;

/* i2c配置 */
typedef struct
{
    I2C_HandleTypeDef *handle; // i2c handle
    uint8_t dev_address;       // 暂时只支持7位地址(还有一位是读写位)
    IIC_Work_Mode work_mode;             // 工作模式
    IIC_Operation_Mode operation_mode;
    iic_module_callback module_tx_callback; //回调函数
    iic_module_callback module_rx_callback; //回调函数

} IIC_Configs;

IIC_Instance *IIC_Register(IIC_Configs *iic_cfg);

void IIC_Transmit(IIC_Instance *ins, uint8_t *data, uint16_t size);

void IIC_Mem_Transmit(IIC_Instance *ins, uint16_t MemAddress, uint8_t mem8bit_flag, uint8_t *Data, uint16_t size);

void IIC_Receive(IIC_Instance *ins, uint8_t *data, uint16_t size);

void IIC_Mem_Receive(IIC_Instance *ins, uint16_t MemAddress, uint8_t mem8bit_flag, uint8_t *Data, uint16_t size);

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c);

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c);

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c);

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c);

#endif /* __BSP_IIC_H__ */

