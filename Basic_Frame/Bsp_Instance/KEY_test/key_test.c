#include "key_test.h"


static GPIO_Configs key;

static void key_module(void)
{
		b1_toogle();
}

void key_Init(void)
{
	key.GPIOx = GPIOA;
	key.GPIO_Pin = GPIO_PIN_0;
	key.module_callback = key_module;
	GPIO_Register(&key);
}
