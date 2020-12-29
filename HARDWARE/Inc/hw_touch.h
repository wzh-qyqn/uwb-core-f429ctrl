#ifndef HW_TOUCH_H_
#define HW_TOUCH_H_
#include "stm32f4xx_sys.h"

#ifdef __cplusplus
 extern "C" {
#endif
#define TOUCH_MAX   5	//���������

typedef struct 
{
	uint8_t  flag;			//	������־λ��Ϊ1ʱ��ʾ�д�������
	uint8_t  num;				//	��������
	uint16_t x[TOUCH_MAX];	//	x����
	uint16_t y[TOUCH_MAX];	//	y����
}TouchStructure;

extern TouchStructure touchInfo;	// �������ݽṹ������	
void Touch_Scan(void);
uint8_t HW_Touch_Init(void);
#ifdef __cplusplus
}
#endif

#endif
