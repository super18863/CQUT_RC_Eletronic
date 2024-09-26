#include "PWM_test.h"

static PWM_Instance *pwm3_ins;
static PWM_Configs	pwm3;

void PWM3_Init(void)
{
	pwm3.htimx = &htim3;
	pwm3.channel = TIM_CHANNEL_4;
	pwm3.dutyratio = 0;
	pwm3_ins = PWM_Register(&pwm3);
	
}

void PWM3_Start(void)
{
	PWM_Start(pwm3_ins);
}

void PWM3_SetDutyRatio(float dutyratio)
{
	PWM_SetDutyRatio(pwm3_ins, dutyratio);
}
