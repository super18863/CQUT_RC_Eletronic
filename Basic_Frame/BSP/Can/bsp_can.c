/**
 * @file bsp_can.c
 * @author s.b.y (CQUT)
 * @brief Can通讯底层
 * @version 0.1
 * @date 2024-09-23
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "bsp_can.h"
#include <stdlib.h>
#include <string.h>

static CAN_Instance *can_instance[CAN_MX_REGISTER_CNT] = {NULL}; // 保存CAN实例的数组
static uint8_t idx; // 全局CAN实例索引,每次有新的模块注册时自增

/**
 * @brief 配置 CAN 过滤器
 * 
 * 这个函数将 CAN 实例的接收ID添加到过滤器中，分配给相应的 FIFO。
 * CAN1 和 CAN2 各有不同的过滤器组（0-13给 CAN1 用, 14-27给 CAN2 用）。
 * 
 * @param instance CAN 实例指针
 */
static void CAN_AddFilter(CAN_Instance *instance)
{
    CAN_FilterTypeDef can_filter_cfg;
    static uint8_t can1_filter_idx = 0, can2_filter_idx = 14; // CAN1用过滤器0-13，CAN2用过滤器14-27

    // 过滤器配置：ID列表模式，只接收特定ID的消息
    can_filter_cfg.FilterMode = CAN_FILTERMODE_IDLIST; // 使用ID list模式，其他报文会被过滤
    can_filter_cfg.FilterScale = CAN_FILTERSCALE_16BIT; // 使用16位ID模式
    // 根据发送ID的奇偶性，分配到FIFO0或FIFO1
    can_filter_cfg.FilterFIFOAssignment = (instance->tx_id & 1) ? CAN_RX_FIFO0 : CAN_RX_FIFO1; 
    can_filter_cfg.SlaveStartFilterBank = 14; // CAN2从第14个过滤器开始
    // 设置过滤器寄存器的低16位
    can_filter_cfg.FilterIdLow = instance->rx_id << 5; // 标准ID低11位有效，高5位填0
    // 判断CAN是CAN1还是CAN2，然后分配相应的过滤器索引
    can_filter_cfg.FilterBank = instance->can_handle == &hcan1 ? (can1_filter_idx++) : (can2_filter_idx++);
    can_filter_cfg.FilterActivation = CAN_FILTER_ENABLE; // 启用过滤器

    // 应用配置到过滤器
    HAL_CAN_ConfigFilter(instance->can_handle, &can_filter_cfg);
}

/**
 * @brief 初始化 CAN 服务
 * 
 * 这个函数在第一个 CAN 实例初始化时被自动调用，启动 CAN 服务。
 * 它启动 CAN1 和 CAN2，并启用 FIFO0 和 FIFO1 的消息挂起中断通知。
 */
static void CANServiceInit()
{
    // 启动 CAN1 并启用 FIFO0 和 FIFO1 的接收中断
    HAL_CAN_Start(&hcan1);
    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO1_MSG_PENDING);
    // 启动 CAN2 并启用 FIFO0 和 FIFO1 的接收中断
    HAL_CAN_Start(&hcan2);
    HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING);
    HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO1_MSG_PENDING);
}

/**
 * @brief 注册一个新的 CAN 模块实例
 * 
 * 这个函数将 CAN 模块的配置信息注册为一个 CAN_Instance，并分配必要的过滤器和配置。
 * 第一次调用时会启动 CAN 服务，并将该实例保存到全局的 can_instance 数组中。
 * 
 * @param can_cfg CAN 配置信息
 * @return CAN_Instance* 返回 CAN 实例的指针
 */
CAN_Instance *CANRegister(CAN_Config *can_cfg)
{
    // 第一次注册时，进行硬件初始化
    if (!idx)
    {
        CANServiceInit();
    }
    // 超过最大注册实例数时，进入死循环
    if (idx >= CAN_MX_REGISTER_CNT)
    {
        while (1);
    }
    // 遍历检查是否有重复注册的实例（重复ID或句柄）
    for (size_t i = 0; i < idx; i++)
    {
        if (can_instance[i]->rx_id == can_cfg->rx_id && can_instance[i]->can_handle == can_cfg->can_handle)
        {
            while (1); // 出现重复注册的情况，进入死循环
        }
    }
    
    // 为新实例分配内存并清空
    CAN_Instance *ins = (CAN_Instance *)malloc(sizeof(CAN_Instance));
    memset(ins, 0, sizeof(CAN_Instance));

    // 配置发送报文信息
    ins->txconf.StdId = can_cfg->tx_id; // 发送 ID
    ins->txconf.IDE = CAN_ID_STD;      // 使用标准ID
    ins->txconf.RTR = CAN_RTR_DATA;    // 发送数据帧
    ins->txconf.DLC = 0x08;            // 默认数据长度为8

    // 设置 CAN 实例的句柄和回调函数
    ins->can_handle = can_cfg->can_handle;
    ins->rx_id = can_cfg->rx_id;
    ins->can_module_callback = can_cfg->can_module_callback;

    // 添加过滤器配置
    CAN_AddFilter(ins);
    // 将实例保存到全局数组中
    can_instance[idx++] = ins;

    return ins;
}

/**
 * @brief 发送 CAN 消息
 * 
 * 这个函数检查邮箱是否空闲，然后将消息添加到发送队列中进行发送。
 * 
 * @param ins CAN 实例指针
 * @param timeout 超时时间，当前未使用
 * @return uint8_t 返回发送状态，1表示成功，0表示失败
 */
uint8_t CANTransmit(CAN_Instance *ins, float timeout)
{
    // 等待邮箱空闲
    while (HAL_CAN_GetTxMailboxesFreeLevel(ins->can_handle) == 0);

    // 发送消息到 CAN 控制器
    if (HAL_CAN_AddTxMessage(ins->can_handle, &ins->txconf, ins->tx_buff, &ins->tx_mailbox))
    {
        return 0; // 发送失败
    }

    return 1; // 发送成功
}

/**
 * @brief 设置 CAN 消息的数据长度（DLC）
 * 
 * @param ins CAN 实例指针
 * @param length 数据长度，最大值为8字节
 */
void CANSetDLC(CAN_Instance *ins, uint8_t length)
{
    // 数据长度错误检查，长度必须在 1-8 之间
    if (length > 8 || length == 0)
        while (1); // 出错时进入死循环
    ins->txconf.DLC = length;
}

/**
 * @brief 处理 FIFO 中的消息
 * 
 * 这个函数用于从指定的 FIFO 中读取 CAN 消息，并查找相应的实例进行处理。
 * 如果找到对应的实例，会调用注册的回调函数来处理接收到的数据。
 * 
 * @param hcan CAN 句柄
 * @param fifox FIFO 索引（FIFO0 或 FIFO1）
 */
static void CANFIFOxCallback(CAN_HandleTypeDef *hcan, uint32_t fifox)
{
    static CAN_RxHeaderTypeDef rxconf; // 保存接收到的 CAN 消息的头部信息
    uint8_t can_rx_buff[8];            // 接收缓存，最大8字节

    // 检查 FIFO 是否非空
    while (HAL_CAN_GetRxFifoFillLevel(hcan, fifox))
    {
        // 从 FIFO 中读取消息
        HAL_CAN_GetRxMessage(hcan, fifox, &rxconf, can_rx_buff);

        // 遍历所有实例，寻找匹配的实例
        for (size_t i = 0; i < idx; ++i)
        {
            // 判断消息是否属于某个已注册的实例
            if (hcan == can_instance[i]->can_handle && rxconf.StdId == can_instance[i]->rx_id)
            {
                // 如果实例注册了回调函数，则调用回调函数处理消息
                if (can_instance[i]->can_module_callback != NULL)
                {
                    can_instance[i]->rx_len = rxconf.DLC; // 保存接收数据长度
                    memcpy(can_instance[i]->rx_buff, can_rx_buff, rxconf.DLC); // 复制消息数据到实例
                    can_instance[i]->can_module_callback(can_instance[i]); // 调用回调处理数据
                }
                return;
            }
        }
    }
}

/**
 * @brief FIFO0 消息挂起中断回调
 * 
 * @param hcan CAN 句柄
 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CANFIFOxCallback(hcan, CAN_RX_FIFO0); // 处理 FIFO0 的消息
}

/**
 * @brief FIFO1 消息挂起中断回调
 * 
 * @param hcan CAN 句柄
 */
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CANFIFOxCallback(hcan, CAN_RX_FIFO1); // 处理 FIFO1 的消息
}
