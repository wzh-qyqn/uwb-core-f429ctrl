#ifndef HW_CPU_TEMP_H__
#define HW_CPU_TEMP_H__

#include "stm32f4xx_sys.h"

#ifdef __cplusplus
 extern "C" {
#endif


void HW_CPU_TEMP_Init(void);
double Get_Cpu_Temprate(void);

#ifdef __cplusplus
}
#endif

#endif

