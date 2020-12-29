#include "hw_led.h"



void HW_LED_Init(void){
	GPIO_InitTypeDef Gpio_Initure;
	
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOI_CLK_ENABLE();

	Gpio_Initure.Mode=GPIO_MODE_OUTPUT_PP;
	Gpio_Initure.Pull=GPIO_PULLUP;
	Gpio_Initure.Speed=GPIO_SPEED_FAST;
	Gpio_Initure.Pin=GPIO_PIN_4;
	HAL_GPIO_Init(GPIOD,&Gpio_Initure);
	
	Gpio_Initure.Pin=GPIO_PIN_3;
	HAL_GPIO_Init(GPIOI,&Gpio_Initure);
	
	LED0(0);
	LED1(0);
}

