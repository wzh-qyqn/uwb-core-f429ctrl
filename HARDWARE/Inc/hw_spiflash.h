#ifndef HW_SPIFLASH_H_
#define HW_SPIFLASH_H_

#include "stm32f4xx_sys.h"


#ifdef __cplusplus
 extern "C" {
#endif

void 		HW_SPIFlash_Init(void);
void 		sFLASH_EraseSector(uint32_t SectorAddr);	
void 		sFLASH_EraseBulk(void);							
void 		sFLASH_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void 		sFLASH_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);

#ifdef __cplusplus
}
#endif



#endif
