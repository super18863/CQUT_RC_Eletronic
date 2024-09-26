#ifndef BSP_CAN_H
#define BSP_CAN_H

#include <stdint.h>
#include "can.h"

// 定义CAN设备的最大支持数量，取决于总线的负载能力
#define CAN_MX_REGISTER_CNT 16     // 支持的最大CAN设备数量
#define MX_CAN_FILTER_CNT (2 * 14) // CAN过滤器的最大数量
#define DEVICE_CAN_CNT 2           // 定义板子上支持的CAN数量
// 当只有一个CAN控制器时，需要将 `bsp_can.c` 中所有 `hcan2` 的变量替换为 `hcan1`。这主要用于均衡总线和FIFO负载，不影响功能


/* 
 * CAN实例结构体：用于保存CAN模块的配置信息和运行时状态。
 * 每个实例代表一个CAN模块，包含发送和接收的ID、缓存、回调函数等信息。
 */
typedef struct can_temp
{
    CAN_HandleTypeDef *can_handle; // CAN句柄，指向对应的硬件CAN模块
    CAN_TxHeaderTypeDef txconf;    // CAN报文的发送配置，包括发送ID、报文类型等
    uint32_t tx_id;                // 发送ID，用于识别该模块发出的报文
    uint32_t tx_mailbox;           // 保存CAN发送邮箱号，用于发送数据
    uint8_t tx_buff[8];            // 发送数据缓存，最大长度为8字节
    uint8_t rx_buff[8];            // 接收数据缓存，最大长度为8字节
    uint32_t rx_id;                // 接收ID，用于过滤和接收特定报文
    uint8_t rx_len;                // 接收到的数据长度，最大为8字节
    void (*can_module_callback)(struct can_temp *); // 指向的回调函数，在接收数据后调用
} CAN_Instance;

/* 
 * CAN配置结构体：在初始化时使用，传递给注册函数。
 * 该结构体包含CAN句柄、发送ID、接收ID以及接收到数据时的回调函数指针。
 */
typedef struct
{
    CAN_HandleTypeDef *can_handle;              // CAN句柄，指定是CAN1还是CAN2
//		CAN_TxHeaderTypeDef txconf;    // CAN报文的发送配置，包括发送ID、报文类型等
    uint32_t tx_id;                             // 发送ID，用于识别发送的数据
    uint32_t rx_id;                             // 接收ID，用于过滤和接收特定ID的消息
    void (*can_module_callback)(CAN_Instance *);  // 指向的回调函数，在接收数据后调用
} CAN_Config;

/* 注册一个新的CAN模块实例 */
CAN_Instance *CANRegister(CAN_Config *can_cfg);

/* 发送CAN消息 */
uint8_t CANTransmit(CAN_Instance *ins, float timeout);

/* 设置发送数据的长度（DLC） */
void CANSetDLC(CAN_Instance *ins, uint8_t length);

#endif // BSP_CAN_H
