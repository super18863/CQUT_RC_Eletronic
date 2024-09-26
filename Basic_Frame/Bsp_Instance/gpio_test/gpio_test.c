#include "gpio_test.h"

static GPIO_Configs gpio1;
static GPIO_Instance *gpio1_ins;

void gpio1_Init(void)
{
	gpio1.GPIOx = GPIOB;
	gpio1.GPIO_Pin = GPIO_PIN_0;
	gpio1.module_callback = NULL;
	gpio1_ins = GPIO_Register(&gpio1);
}

void b1_H(void)
{
	Pin_H(gpio1_ins);
}

void b1_L(void)
{
	Pin_L(gpio1_ins);
}

void b1_toogle(void)
{
	Pin_Toogle(gpio1_ins);
}
