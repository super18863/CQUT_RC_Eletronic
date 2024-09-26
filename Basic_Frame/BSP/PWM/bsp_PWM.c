/**
 * @file bsp_PWM.c
 * @author 
 * @brief PWM底层驱动实现文件
 * @version 0.1
 * @date 2024-09-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "bsp_PWM.h"
#include <stdlib.h>
#include <string.h>

// 记录已注册的PWM实例数量
static uint8_t idx = 0;
// 保存所有已注册的PWM实例，数组大小由PWM_Max_DEVICE宏定义
static PWM_Instance *pwm_instance[PWM_Max_DEVICE] = {NULL};

/**
 * @brief 注册一个PWM实例，并初始化定时器的中断功能
 * 
 * @param pwm_cfg 指向PWM配置的结构体，包含定时器句柄、通道、周期和占空比等参数
 * @return PWM_Instance* 返回注册好的PWM实例指针
 */
PWM_Instance *PWM_Register(PWM_Configs *pwm_cfg)
{
    // 检查是否超过最大允许的PWM实例数
    if (idx >= PWM_Max_DEVICE)
        while (1); // 错误处理：超过实例数，进入死循环

    // 检查是否重复注册同一个定时器实例
    for (uint8_t i = 0; i < idx; i++)
        if (pwm_instance[i]->htimx == pwm_cfg->htimx)
            while (1); // 错误处理：定时器已经注册，进入死循环

    // 动态分配内存给新的PWM实例，并初始化其内容
    PWM_Instance *ins = (PWM_Instance *)malloc(sizeof(PWM_Instance));
    memset(ins, 0, sizeof(PWM_Instance));

    // 将传入的配置参数保存到新创建的PWM实例
    ins->channel = pwm_cfg->channel;       // PWM通道
    ins->dutyratio = pwm_cfg->dutyratio;   // PWM占空比
    ins->htimx = pwm_cfg->htimx;           // 定时器句柄
//    ins->period = pwm_cfg->period;         // PWM周期

    // 将新创建的PWM实例保存到实例数组中，并增加实例计数
    pwm_instance[idx++] = ins;


    return ins; // 返回新创建的PWM实例指针
}

/**
 * @brief 开始PWM输出
 * 
 * @param ins PWM实例指针
 */
void PWM_Start(PWM_Instance *ins)
{
    // 启动PWM信号输出，指定通道
    HAL_TIM_PWM_Start(ins->htimx, ins->channel);
}

/**
 * @brief 停止PWM输出
 * 
 * @param ins PWM实例指针
 */
void PWM_Stop(PWM_Instance *ins)
{
    // 停止PWM信号输出，指定通道
    HAL_TIM_PWM_Stop(ins->htimx, ins->channel);
}

/** 
 * @brief 设置PWM占空比
 * 
 * @param ins PWM实例指针
 * @param dutyratio 占空比，范围为0~1
 */
void PWM_SetDutyRatio(PWM_Instance *ins, float dutyratio)
{
    // 根据传入的占空比调整PWM信号的输出，修改定时器的比较寄存器值
    // __HAL_TIM_SetCompare宏用于修改定时器比较寄存器
    __HAL_TIM_SetCompare(ins->htimx, ins->channel, dutyratio * (ins->htimx->Instance->ARR));
}
