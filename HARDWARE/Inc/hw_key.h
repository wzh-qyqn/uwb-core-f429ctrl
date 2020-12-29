#ifndef HW_KEY_H_
#define HW_KEY_H_

#include "stm32f4xx_sys.h"

typedef enum 
{
	No_Press=0,
	Once_Press,
	Double_Press,
	Third_Press,
	Mid_Press,
	Long_Press
}KEY_Press;

#ifdef __cplusplus
 extern "C" {
#endif
void HW_KEY_Init(void);
KEY_Press key_scan(void);

void Key_Timer_Create(void);
bool Key_IS_Press(void);
bool Key_IS_LongPress(void);
#ifdef __cplusplus
}
#endif

#endif

