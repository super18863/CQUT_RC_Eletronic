/**
 * @file bsp_adc.c
 * @author s.b.y (CQUT)
 * @brief 
 * @version 0.1
 * @date 2024-09-23
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include"bsp_adc.h"

//ADC食用方法
//HAL_StatusTypeDef HAL_ADC_Start_DMA(ADC_HandleTypeDef* hadc, uint32_t* pData, uint32_t Length)
//填adc句柄，接收数组，接收个数开了几个adc通道填几个就完事了
//用ADC用DMA太简单了，以至于我不知道该怎么封装了
//其实是我懒得写了，相信后人的智慧，相信小登
