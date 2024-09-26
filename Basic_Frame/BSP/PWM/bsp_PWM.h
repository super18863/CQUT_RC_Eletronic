#ifndef __BSP_PWM_H__
#define __BSP_PWM_H__

#include"main.h"

#define PWM_Max  16
#define PWM_Max_DEVICE 16
//定时器实例
typedef struct 
{
    TIM_HandleTypeDef *htimx;                 // TIM句柄
    uint32_t channel;                        // 通道
    float dutyratio;                      // 占空比
    
} PWM_Instance;

typedef struct 
{
    TIM_HandleTypeDef *htimx;                 // TIM句柄
    uint32_t channel;                        // 通道
    float dutyratio;                      // 占空比
    
} PWM_Configs;

PWM_Instance *PWM_Register(PWM_Configs *pwm_cfg);

void PWM_Start(PWM_Instance *ins);

void PWM_Stop(PWM_Instance *ins);

void PWM_SetDutyRatio(PWM_Instance *ins, float dutyratio);

#endif /* __BSP_PWM_H__ */
