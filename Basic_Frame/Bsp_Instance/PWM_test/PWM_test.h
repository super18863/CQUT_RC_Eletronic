#ifndef __PWM_TEST_H__
#define __PWM_TEST_H__

#include "bsp_PWM.h"

extern TIM_HandleTypeDef htim3;


void PWM3_Init(void);

void PWM3_Start(void);

void PWM3_SetDutyRatio(float dutyratio);

#endif /* __PWM_TEST_H__ */
