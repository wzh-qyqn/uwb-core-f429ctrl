#ifndef HW_LED_H_
#define HW_LED_H_

#include "stm32f4xx_sys.h"

__STATIC_FORCEINLINE void LED0(bool n){
	n?	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_4,GPIO_PIN_RESET):\
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_4,GPIO_PIN_SET);
}
__STATIC_FORCEINLINE void LED1(bool n){
	n?	HAL_GPIO_WritePin(GPIOI,GPIO_PIN_3,GPIO_PIN_RESET):\
		HAL_GPIO_WritePin(GPIOI,GPIO_PIN_3,GPIO_PIN_SET);
}
__STATIC_FORCEINLINE void LED0_Toggle(void){
	HAL_GPIO_TogglePin(GPIOD,GPIO_PIN_4);
}
__STATIC_FORCEINLINE void LED1_Toggle(void){
	HAL_GPIO_TogglePin(GPIOI,GPIO_PIN_3);
}

#ifdef __cplusplus
 extern "C" {
#endif

void HW_LED_Init(void);

#ifdef __cplusplus
}
#endif
#endif
