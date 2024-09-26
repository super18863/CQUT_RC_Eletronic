#include "timer_test.h"
#include "gpio_test.h"

static Timer_Configs timer2;

static void timer2_callback()
{
	b1_toogle();
}

void Timer2_Init(void)
{
    timer2.htimx = &htim2;
		timer2.module_callback = timer2_callback;
		Timer_Register(&timer2);
	
}
