/**
 * @file bsp_gpio.c
 * @author s.b.y (CQUT)
 * @brief GPIO底层
 * @version 0.1
 * @date 2024-09-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "bsp_gpio.h"
#include <stdlib.h>
#include <string.h>

static uint8_t idx = 0;
static GPIO_Instance *gpio_instance[GPIO_Max_DEVICE] = {NULL};


/**
 * @brief 外部中断注册函数
 * 
 * @param GPIO_cfg 
 */
GPIO_Instance *GPIO_Register(GPIO_Configs *GPIO_cfg)
{
    GPIO_Instance *ins = (GPIO_Instance *)malloc(sizeof(GPIO_Instance));
    memset(ins, 0, sizeof(GPIO_Instance));

    ins->GPIOx = GPIO_cfg->GPIOx;
    ins->GPIO_Pin = GPIO_cfg->GPIO_Pin;
    ins->module_callback = GPIO_cfg->module_callback;
    gpio_instance[idx++] = ins;
    return ins;
}


/**
* @brief 外部中断回调函数，编辑中断触发后的操作
* 
* @param GPIO_Pin 
*/
 void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    for (uint8_t i = 0; i < idx; ++i)
    { 
        if ( GPIO_Pin == gpio_instance[i]->GPIO_Pin&&gpio_instance[i]->module_callback != NULL )
        { 
            gpio_instance[i]->module_callback();
            return; // break the loop
        }
    }
}


/**
 * @brief IO端口为高电平
 * 
 * @param GPIOx 
 * @param GPIO_Pin 
 */
void Pin_H(GPIO_Instance *GPIO_ins)
{
    HAL_GPIO_WritePin( GPIO_ins->GPIOx,GPIO_ins->GPIO_Pin,GPIO_PIN_SET );
}


/**
 * @brief IO端口为低电平
 * 
 * @param GPIOx 
 * @param GPIO_Pin 
 */
void Pin_L(GPIO_Instance *GPIO_ins)
{
    HAL_GPIO_WritePin( GPIO_ins->GPIOx,GPIO_ins->GPIO_Pin,GPIO_PIN_RESET );
}


/**
 * @brief IO端口电平翻转
 * 
 * @param GPIOx 
 * @param GPIO_Pin 
 */
void Pin_Toogle(GPIO_Instance *GPIO_ins)
{
    HAL_GPIO_TogglePin( GPIO_ins->GPIOx,GPIO_ins->GPIO_Pin );
}


/**
 * @brief 锁住IO端口电平
 * 
 * @param GPIOx 
 * @param GPIO_Pin 
 */
void Pin_LockPin(GPIO_Instance *GPIO_ins)
{
    HAL_GPIO_LockPin( GPIO_ins->GPIOx,GPIO_ins->GPIO_Pin );
}


/**
 * @brief 读取IO电平
 * 
 * @param GPIOx 
 * @param GPIO_Pin 
 * @return PinState 
 */
PinState Pin_Read(GPIO_Instance *GPIO_ins)
{
    GPIO_ins->pinState = (PinState)HAL_GPIO_ReadPin( GPIO_ins->GPIOx,GPIO_ins->GPIO_Pin );
    return GPIO_ins->pinState;
}

