#ifndef HW_SDRAM_H_
#define HW_SDRAM_H_

#include "stm32f4xx_sys.h"

#ifdef __cplusplus
 extern "C" {
#endif
void HW_SDRAM_Init(void);
uint8_t SDRAM_Test(void);
#ifdef __cplusplus
}
#endif

#endif
