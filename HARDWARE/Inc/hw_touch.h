#ifndef HW_TOUCH_H_
#define HW_TOUCH_H_
#include "stm32f4xx_sys.h"

#ifdef __cplusplus
 extern "C" {
#endif
#define TOUCH_MAX   5	//最大触摸点数

typedef struct 
{
	uint8_t  flag;			//	触摸标志位，为1时表示有触摸操作
	uint8_t  num;				//	触摸点数
	uint16_t x[TOUCH_MAX];	//	x坐标
	uint16_t y[TOUCH_MAX];	//	y坐标
}TouchStructure;

extern TouchStructure touchInfo;	// 触摸数据结构体声明	
void Touch_Scan(void);
uint8_t HW_Touch_Init(void);
#ifdef __cplusplus
}
#endif

#endif
