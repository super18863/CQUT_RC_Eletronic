#ifndef __BSP_TIMER_H__
#define __BSP_TIMER_H__

#include"main.h"

#define Timer_Max_DEVICE    14 //最大定时器个数，根据需要修改

// 模块回调函数,用于解析协议
typedef void (*timer_module_callback)();


//定时器实例
typedef struct 
{
    TIM_HandleTypeDef *htimx;                //定时器句柄
    timer_module_callback module_callback;  // 解析收到的数据的回调函数
    
} Timer_Instance;


//定时器配置
typedef struct 
{
    TIM_HandleTypeDef *htimx;                //定时器句柄
    timer_module_callback module_callback;  // 解析收到的数据的回调函数
    
} Timer_Configs;


Timer_Instance *Timer_Register(Timer_Configs *timer_cfg);


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);


#endif /* __BSP_TIMER_H__ */
