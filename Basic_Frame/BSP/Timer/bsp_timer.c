/**
 * @file bsp_timer.c
 * @author 
 * @brief 定时器底层驱动实现文件
 * @version 0.1
 * @date 2024-09-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "bsp_timer.h"
#include <stdlib.h>
#include <string.h>

static uint8_t idx = 0; // 用于记录已经注册的定时器实例数
static Timer_Instance *timer_instance[Timer_Max_DEVICE] = {NULL}; // 保存所有定时器实例的数组

/**
 * @brief 注册定时器实例，并开始定时器中断
 * 
 * @param timer_cfg 指向定时器配置的结构体，该结构体包含定时器句柄和回调函数
 * @return Timer_Instance* 返回注册的定时器实例指针
 */
Timer_Instance *Timer_Register(Timer_Configs *timer_cfg)
{
    // 检查是否超过允许的最大定时器实例数
    if (idx >= Timer_Max_DEVICE)
        while (1); // 错误处理：定时器实例数量超出上限，使用死循环作为错误处理

    // 检查定时器是否已经注册过，避免重复注册
    for (uint8_t i = 0; i < idx; i++)
        if (timer_instance[i]->htimx == timer_cfg->htimx)
            while (1); // 错误处理：定时器已经注册，使用死循环作为错误处理

    // 分配内存给新的定时器实例并清空其内容
    Timer_Instance *ins = (Timer_Instance *)malloc(sizeof(Timer_Instance));
    memset(ins, 0, sizeof(Timer_Instance));
    
    // 初始化定时器实例的句柄和回调函数
    ins->htimx = timer_cfg->htimx;
    ins->module_callback = timer_cfg->module_callback;

    // 将新创建的定时器实例保存到实例数组中，并增加实例计数
    timer_instance[idx++] = ins;
    
    // 启动定时器的中断功能，允许定时器周期性触发中断
    HAL_TIM_Base_Start_IT(timer_cfg->htimx);

    return ins; // 返回新创建的定时器实例指针
}

/**
 * @brief 定时器中断回调函数，每次定时器溢出时会调用此函数
 * 
 * @param htim 定时器句柄，用于标识哪个定时器触发了中断
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    // 遍历所有已注册的定时器实例，找到触发中断的那个定时器实例
    for(uint8_t i = 0; i < idx; ++i)
    {
        // 比较传入的定时器句柄和已注册定时器的句柄，找到匹配的定时器实例
        if (htim == timer_instance[i]->htimx && timer_instance[i]->module_callback != NULL)
        {
            // 调用定时器实例对应的回调函数
            timer_instance[i]->module_callback();
            return; // 找到并执行回调函数后，结束函数
        }     
    }
}
