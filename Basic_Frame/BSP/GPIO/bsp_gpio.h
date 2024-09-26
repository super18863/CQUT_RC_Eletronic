#ifndef __BSP_GPIO_H__
#define __BSP_GPIO_H__

#include"main.h"

#define GPIO_Max_DEVICE 10 //最大GPIO实例数，根据需要调整

typedef void (*gpio_module_callback)(); //外部中断回调函数

/**
 * @brief GPIO状态，仍然只是形式上的封装 
 * 
 */
typedef enum {
    PIN_RESET = 0,
    PIN_SET
    
} PinState;

/**
 * @brief GPIO设置，用户只可以设置此结构体内容
 * 
 */
typedef struct 
{
    GPIO_TypeDef* GPIOx;
    uint16_t GPIO_Pin;
    gpio_module_callback module_callback;  // 解析收到的数据的回调函数
    
} GPIO_Configs;

/**
 * @brief 注册GPIO实例，函数运行调用实际调用此结构体内容
 * 
 */
typedef struct 
{
    GPIO_TypeDef* GPIOx;
    uint16_t GPIO_Pin;
    PinState pinState;
    gpio_module_callback module_callback;  
    
} GPIO_Instance;


GPIO_Instance *GPIO_Register(GPIO_Configs *GPIO_ins);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void Pin_H(GPIO_Instance *GPIO_ins);
void Pin_L(GPIO_Instance *GPIO_ins);
void Pin_Toogle(GPIO_Instance *GPIO_ins);
void Pin_LockPin(GPIO_Instance*GPIO_ins);
PinState Pin_Read(GPIO_Instance *GPIO_ins);



#endif /* __BSP_GPIO_H__ */

