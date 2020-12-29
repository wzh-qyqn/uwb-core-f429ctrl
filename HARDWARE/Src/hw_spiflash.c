#include "hw_spiflash.h"

#define sFLASH_CMD_WRITE          0x02  /* Write to Memory instruction */
#define sFLASH_CMD_WRSR           0x01  /* Write Status Register instruction */
#define sFLASH_CMD_WREN           0x06  /* Write enable instruction */
#define sFLASH_CMD_READ           0x03  /* Read from Memory instruction */
#define sFLASH_CMD_RDSR           0x05  /* Read Status Register instruction  */
#define sFLASH_CMD_RDID           0x9F  /* Read identification */
#define sFLASH_CMD_SE             0x20  /* Sector Erase instruction */
#define sFLASH_CMD_BE             0xC7  /* Bulk Erase instruction */
#define sFLASH_WIP_FLAG           0x01  /* Write In Progress (WIP) flag */
#define sFLASH_DUMMY_BYTE         0xA5

#define sFLASH_SPI_PAGESIZE       256	

#define  sFLASH_ID                  0XEF4017     //W25Q64
//#define  sFLASH_ID                0XEF4018     //W25Q128

/*-------------------------------------------- SPI1配置宏 ---------------------------------------*/


#define  SPI1_SCK_PIN								GPIO_PIN_3								// SPI1_SCK 引脚
#define	SPI1_SCK_PORT								GPIOB										// SPI1_SCK 引脚端口
#define 	GPIO_SPI1_SCK_CLK_ENABLE        		__HAL_RCC_GPIOB_CLK_ENABLE()		// SPI1_SCK	引脚时钟

#define  SPI1_MISO_PIN								GPIO_PIN_4								// SPI1_MISO 引脚
#define	SPI1_MISO_PORT								GPIOB										// SPI1_MISO 引脚端口
#define 	GPIO_SPI1_MISO_CLK_ENABLE        	__HAL_RCC_GPIOB_CLK_ENABLE()		// SPI1_MISO 引脚时钟

#define  SPI1_MOSI_PIN								GPIO_PIN_5								// SPI1_MOSI 引脚
#define	SPI1_MOSI_PORT								GPIOB										// SPI1_MOSI 引脚端口
#define 	GPIO_SPI1_MOSI_CLK_ENABLE        	__HAL_RCC_GPIOB_CLK_ENABLE()		// SPI1_MOSI 引脚时钟

#define  SPI1_CS_PIN									GPIO_PIN_3								// SPI1_CS 引脚
#define	SPI1_CS_PORT								GPIOG										// SPI1_CS 引脚端口
#define 	GPIO_SPI1_CS_CLK_ENABLE        		__HAL_RCC_GPIOG_CLK_ENABLE()		// SPI1_CS 引脚时钟


#define sFLASH_CS_LOW()       HAL_GPIO_WritePin(SPI1_CS_PORT, SPI1_CS_PIN, GPIO_PIN_RESET)	// CS输出低电平
#define sFLASH_CS_HIGH()      HAL_GPIO_WritePin(SPI1_CS_PORT, SPI1_CS_PIN, GPIO_PIN_SET) 		// CS输出高电平



SPI_HandleTypeDef hspi1;	 // SPI_HandleTypeDef 结构体变量

void 		sFLASH_Init(void);	

void 		sFLASH_EraseSector(uint32_t SectorAddr);	
void 		sFLASH_EraseBulk(void);							
void 		sFLASH_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void 		sFLASH_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);

static void sFLASH_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
static uint32_t sFLASH_ReadID(void);
static void 		sFLASH_StartReadSequence(uint32_t ReadAddr);
static uint8_t 		sFLASH_ReadByte(void);
static uint8_t 		sFLASH_SendByte(uint8_t byte);
static uint16_t 	sFLASH_SendHalfWord(uint16_t HalfWord);
static void 		sFLASH_WriteEnable(void);
static void 		sFLASH_WaitForWriteEnd(void);

/***************************************************************************************************
*	函 数 名: sFLASH_Init
*	入口参数: 无
*	返 回 值: 无
*	函数功能: 初始化SPI1
*	说    明: 无
****************************************************************************************************/

void sFLASH_Init(void)
{
  sFLASH_CS_HIGH();
	
  hspi1.Instance 						= SPI1;						
  hspi1.Init.Mode				 		= SPI_MODE_MASTER;               //	主机模式
  hspi1.Init.Direction 				= SPI_DIRECTION_2LINES;          //	双线全双工
  hspi1.Init.DataSize 				= SPI_DATASIZE_8BIT;             //	8为数据宽度
  hspi1.Init.CLKPolarity 			= SPI_POLARITY_HIGH;             //	CLK空闲状态为高电平
  hspi1.Init.CLKPhase 				= SPI_PHASE_2EDGE;               //	CLK第二个跳变沿数据开始采样	
  hspi1.Init.NSS 						= SPI_NSS_SOFT;                  //	NSS即片选信号CS由软件控制
  hspi1.Init.BaudRatePrescaler 	= SPI_BAUDRATEPRESCALER_2;       //	时钟分频系数
  hspi1.Init.FirstBit 				= SPI_FIRSTBIT_MSB;              //	高位在先
  hspi1.Init.TIMode 					= SPI_TIMODE_DISABLE;            //	禁止TI模式
  hspi1.Init.CRCCalculation 		= SPI_CRCCALCULATION_DISABLE;    //	禁止CRC
  hspi1.Init.CRCPolynomial 		= 10;                              
	
  HAL_SPI_Init(&hspi1) ;		// 初始化 SPI1
  __HAL_SPI_ENABLE(&hspi1); 	// 使能   SPI1

}

/*************************************************************************************************
*	函 数 名:	HAL_SPI_MspInit
*	入口参数:	hspi - SPI_HandleTypeDef定义的变量，即表示定义的SPI设备
*	返 回 值:无
*	函数功能:	初始化SPI1引脚
*	说    明:无		
*************************************************************************************************/

void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(hspi->Instance==SPI1)
	{
		__HAL_RCC_SPI1_CLK_ENABLE();	// 开启SPI1 时钟

		GPIO_SPI1_SCK_CLK_ENABLE;		// 开启 SPI1_SCK  GPIO时钟
		GPIO_SPI1_MISO_CLK_ENABLE;		// 开启 SPI1_MISO GPIO时钟
		GPIO_SPI1_MOSI_CLK_ENABLE;		// 开启 SPI1_MOSI GPIO时钟
		GPIO_SPI1_CS_CLK_ENABLE;		// 开启 SPI1_CS	GPIO时钟
		
		
		GPIO_InitStruct.Pin 			= SPI1_SCK_PIN;					// SPI1_SCK引脚 --> PB3
		GPIO_InitStruct.Mode 		= GPIO_MODE_AF_PP;				// 复用推挽输出
		GPIO_InitStruct.Pull 		= GPIO_NOPULL;						// 不上下拉
		GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_VERY_HIGH;	// 高速模式
		GPIO_InitStruct.Alternate 	= GPIO_AF5_SPI1;					// 复用为SPI1
		HAL_GPIO_Init(SPI1_SCK_PORT, &GPIO_InitStruct);	
		
		GPIO_InitStruct.Pin 			= SPI1_MISO_PIN;					// SPI1_MISO引脚 --> PB4
		HAL_GPIO_Init(SPI1_MISO_PORT, &GPIO_InitStruct);		

		GPIO_InitStruct.Pin 			= SPI1_MOSI_PIN;					// SPI1_MOSI引脚 --> PB5
		HAL_GPIO_Init(SPI1_MOSI_PORT, &GPIO_InitStruct);	
		
		GPIO_InitStruct.Pin 			= SPI1_CS_PIN;						// SPI1_CS引脚 --> PG3
		GPIO_InitStruct.Mode 		= GPIO_MODE_OUTPUT_PP;			// 推挽输出
		GPIO_InitStruct.Pull 		= GPIO_PULLUP;						// 上拉
		HAL_GPIO_Init(SPI1_CS_PORT, &GPIO_InitStruct);	

	}

}
/**
  * @brief  Erases the specified FLASH sector.
  * @param  SectorAddr: address of the sector to erase.
  * @retval None
  */
void sFLASH_EraseSector(uint32_t SectorAddr)
{
  /*!< Send write enable instruction */
  sFLASH_WriteEnable();

  /*!< Sector Erase */
  /*!< Select the FLASH: Chip Select low */
  sFLASH_CS_LOW();
  /*!< Send Sector Erase instruction */
  sFLASH_SendByte(sFLASH_CMD_SE);
  /*!< Send SectorAddr high nibble address byte */
  sFLASH_SendByte((SectorAddr & 0xFF0000) >> 16);
  /*!< Send SectorAddr medium nibble address byte */
  sFLASH_SendByte((SectorAddr & 0xFF00) >> 8);
  /*!< Send SectorAddr low nibble address byte */
  sFLASH_SendByte(SectorAddr & 0xFF);
  /*!< Deselect the FLASH: Chip Select high */
  sFLASH_CS_HIGH();

  /*!< Wait the end of Flash writing */
  sFLASH_WaitForWriteEnd();
}

/**
  * @brief  Erases the entire FLASH.
  * @param  None
  * @retval None
  */
void sFLASH_EraseBulk(void)
{
  /*!< Send write enable instruction */
  sFLASH_WriteEnable();

  /*!< Bulk Erase */
  /*!< Select the FLASH: Chip Select low */
  sFLASH_CS_LOW();
  /*!< Send Bulk Erase instruction  */
  sFLASH_SendByte(sFLASH_CMD_BE);
  /*!< Deselect the FLASH: Chip Select high */
  sFLASH_CS_HIGH();

  /*!< Wait the end of Flash writing */
  sFLASH_WaitForWriteEnd();
}


/**
  * @brief  Writes more than one byte to the FLASH with a single WRITE cycle 
  *         (Page WRITE sequence).
  * @note   The number of byte can't exceed the FLASH page size.
  * @param  pBuffer: pointer to the buffer  containing the data to be written
  *         to the FLASH.
  * @param  WriteAddr: FLASH's internal address to write to.
  * @param  NumByteToWrite: number of bytes to write to the FLASH, must be equal
  *         or less than "sFLASH_PAGESIZE" value.
  * @retval None
  */
void sFLASH_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  /*!< Enable the write access to the FLASH */
  sFLASH_WriteEnable();

  /*!< Select the FLASH: Chip Select low */
  sFLASH_CS_LOW();
  /*!< Send "Write to Memory " instruction */
  sFLASH_SendByte(sFLASH_CMD_WRITE);
  /*!< Send WriteAddr high nibble address byte to write to */
  sFLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
  /*!< Send WriteAddr medium nibble address byte to write to */
  sFLASH_SendByte((WriteAddr & 0xFF00) >> 8);
  /*!< Send WriteAddr low nibble address byte to write to */
  sFLASH_SendByte(WriteAddr & 0xFF);

  /*!< while there is data to be written on the FLASH */
  while (NumByteToWrite--)
  {
    /*!< Send the current byte */
    sFLASH_SendByte(*pBuffer);
    /*!< Point on the next byte to be written */
    pBuffer++;
  }

  /*!< Deselect the FLASH: Chip Select high */
  sFLASH_CS_HIGH();

  /*!< Wait the end of Flash writing */
  sFLASH_WaitForWriteEnd();
}

/**
  * @brief  Writes block of data to the FLASH. In this function, the number of
  *         WRITE cycles are reduced, using Page WRITE sequence.
  * @param  pBuffer: pointer to the buffer  containing the data to be written
  *         to the FLASH.
  * @param  WriteAddr: FLASH's internal address to write to.
  * @param  NumByteToWrite: number of bytes to write to the FLASH.
  * @retval None
  */
void sFLASH_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  Addr = WriteAddr % sFLASH_SPI_PAGESIZE;
  count = sFLASH_SPI_PAGESIZE - Addr;
  NumOfPage =  NumByteToWrite / sFLASH_SPI_PAGESIZE;
  NumOfSingle = NumByteToWrite % sFLASH_SPI_PAGESIZE;

  if (Addr == 0) /*!< WriteAddr is sFLASH_PAGESIZE aligned  */
  {
    if (NumOfPage == 0) /*!< NumByteToWrite < sFLASH_PAGESIZE */
    {
      sFLASH_WritePage(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /*!< NumByteToWrite > sFLASH_PAGESIZE */
    {
      while (NumOfPage--)
      {
        sFLASH_WritePage(pBuffer, WriteAddr, sFLASH_SPI_PAGESIZE);
        WriteAddr +=  sFLASH_SPI_PAGESIZE;
        pBuffer += sFLASH_SPI_PAGESIZE;
      }

      sFLASH_WritePage(pBuffer, WriteAddr, NumOfSingle);
    }
  }
  else /*!< WriteAddr is not sFLASH_PAGESIZE aligned  */
  {
    if (NumOfPage == 0) /*!< NumByteToWrite < sFLASH_PAGESIZE */
    {
      if (NumOfSingle > count) /*!< (NumByteToWrite + WriteAddr) > sFLASH_PAGESIZE */
      {
        temp = NumOfSingle - count;

        sFLASH_WritePage(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;

        sFLASH_WritePage(pBuffer, WriteAddr, temp);
      }
      else
      {
        sFLASH_WritePage(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /*!< NumByteToWrite > sFLASH_PAGESIZE */
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / sFLASH_SPI_PAGESIZE;
      NumOfSingle = NumByteToWrite % sFLASH_SPI_PAGESIZE;

      sFLASH_WritePage(pBuffer, WriteAddr, count);
      WriteAddr +=  count;
      pBuffer += count;

      while (NumOfPage--)
      {
        sFLASH_WritePage(pBuffer, WriteAddr, sFLASH_SPI_PAGESIZE);
        WriteAddr +=  sFLASH_SPI_PAGESIZE;
        pBuffer += sFLASH_SPI_PAGESIZE;
      }

      if (NumOfSingle != 0)
      {
        sFLASH_WritePage(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}

/**
  * @brief  Reads a block of data from the FLASH.
  * @param  pBuffer: pointer to the buffer that receives the data read from the FLASH.
  * @param  ReadAddr: FLASH's internal address to read from.
  * @param  NumByteToRead: number of bytes to read from the FLASH.
  * @retval None
  */
void sFLASH_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
  /*!< Select the FLASH: Chip Select low */
  sFLASH_CS_LOW();

  /*!< Send "Read from Memory " instruction */
  sFLASH_SendByte(sFLASH_CMD_READ);

  /*!< Send ReadAddr high nibble address byte to read from */
  sFLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  /*!< Send ReadAddr medium nibble address byte to read from */
  sFLASH_SendByte((ReadAddr& 0xFF00) >> 8);
  /*!< Send ReadAddr low nibble address byte to read from */
  sFLASH_SendByte(ReadAddr & 0xFF);

  while (NumByteToRead--) /*!< while there is data to be read */
  {
    /*!< Read a byte from the FLASH */
    *pBuffer = sFLASH_SendByte(sFLASH_DUMMY_BYTE);
    /*!< Point to the next location where the byte read will be saved */
    pBuffer++;
  }

  /*!< Deselect the FLASH: Chip Select high */
  sFLASH_CS_HIGH();
}


/**
  * @brief  Initiates a read data byte (READ) sequence from the Flash.
  *   This is done by driving the /CS line low to select the device, then the READ
  *   instruction is transmitted followed by 3 bytes address. This function exit
  *   and keep the /CS line low, so the Flash still being selected. With this
  *   technique the whole content of the Flash is read with a single READ instruction.
  * @param  ReadAddr: FLASH's internal address to read from.
  * @retval None
  */
void sFLASH_StartReadSequence(uint32_t ReadAddr)
{
  /*!< Select the FLASH: Chip Select low */
  sFLASH_CS_LOW();

  /*!< Send "Read from Memory " instruction */
  sFLASH_SendByte(sFLASH_CMD_READ);

  /*!< Send the 24-bit address of the address to read from -------------------*/
  /*!< Send ReadAddr high nibble address byte */
  sFLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  /*!< Send ReadAddr medium nibble address byte */
  sFLASH_SendByte((ReadAddr& 0xFF00) >> 8);
  /*!< Send ReadAddr low nibble address byte */
  sFLASH_SendByte(ReadAddr & 0xFF);
}

/**
  * @brief  Reads a byte from the SPI Flash.
  * @note   This function must be used only if the Start_Read_Sequence function
  *         has been previously called.
  * @param  None
  * @retval Byte Read from the SPI Flash.
  */
uint8_t sFLASH_ReadByte(void)
{
  return (sFLASH_SendByte(sFLASH_DUMMY_BYTE));
}


/**
  * @brief  Sends a byte through the SPI interface and return the byte received
  *         from the SPI bus.
  * @param  byte: byte to send.
  * @retval The value of the received byte.
  */
uint8_t sFLASH_SendByte(uint8_t byte)
{
  /*!< Loop while DR register in not empty */
  while (__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_TXE) == RESET);

  /*!< Send byte through the SPI1 peripheral */
  WRITE_REG(hspi1.Instance->DR, byte);

  /*!< Wait to receive a byte */
  while (__HAL_SPI_GET_FLAG( &hspi1, SPI_FLAG_RXNE ) == RESET);

  /*!< Return the byte read from the SPI bus */
  return READ_REG(hspi1.Instance->DR);
}

/**
  * @brief  Sends a Half Word through the SPI interface and return the Half Word
  *         received from the SPI bus.
  * @param  HalfWord: Half Word to send.
  * @retval The value of the received Half Word.
  */
uint16_t sFLASH_SendHalfWord(uint16_t HalfWord)
{
  /*!< Loop while DR register in not empty */
  while (__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_TXE) == RESET);
  /*!< Send Half Word through the sFLASH peripheral */
  WRITE_REG(hspi1.Instance->DR, HalfWord);
  /*!< Wait to receive a Half Word */
  while (__HAL_SPI_GET_FLAG( &hspi1, SPI_FLAG_RXNE ) == RESET);
  /*!< Return the Half Word read from the SPI bus */
  return READ_REG(hspi1.Instance->DR);
}

/**
  * @brief  Enables the write access to the FLASH.
  * @param  None
  * @retval None
  */
void sFLASH_WriteEnable(void)
{
  /*!< Select the FLASH: Chip Select low */
  sFLASH_CS_LOW();

  /*!< Send "Write Enable" instruction */
  sFLASH_SendByte(sFLASH_CMD_WREN);

  /*!< Deselect the FLASH: Chip Select high */
  sFLASH_CS_HIGH();
}

/**
  * @brief  Polls the status of the Write In Progress (WIP) flag in the FLASH's
  *         status register and loop until write operation has completed.
  * @param  None
  * @retval None
  */
void sFLASH_WaitForWriteEnd(void)
{
  uint8_t flashstatus = 0;

  /*!< Select the FLASH: Chip Select low */
  sFLASH_CS_LOW();

  /*!< Send "Read Status Register" instruction */
  sFLASH_SendByte(sFLASH_CMD_RDSR);

  /*!< Loop as long as the memory is busy with a write cycle */
  do
  {
    /*!< Send a dummy byte to generate the clock needed by the FLASH
    and put the value of the status register in FLASH_Status variable */
    flashstatus = sFLASH_SendByte(sFLASH_DUMMY_BYTE);

  }
  while ((flashstatus & sFLASH_WIP_FLAG) == SET); /* Write in progress */

  /*!< Deselect the FLASH: Chip Select high */
  sFLASH_CS_HIGH();
}

/**
  * @brief  Reads FLASH identification.
  * @param  None
  * @retval FLASH identification
  */
uint32_t sFLASH_ReadID(void)
{
  uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

  /*!< Select the FLASH: Chip Select low */
  sFLASH_CS_LOW();

  /*!< Send "RDID " instruction */
  sFLASH_SendByte(0x9F);

  /*!< Read a byte from the FLASH */
  Temp0 = sFLASH_SendByte(sFLASH_DUMMY_BYTE);

  /*!< Read a byte from the FLASH */
  Temp1 = sFLASH_SendByte(sFLASH_DUMMY_BYTE);

  /*!< Read a byte from the FLASH */
  Temp2 = sFLASH_SendByte(sFLASH_DUMMY_BYTE);

  /*!< Deselect the FLASH: Chip Select high */
  sFLASH_CS_HIGH();

  Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

  return Temp;
}

void HW_SPIFlash_Init(void){
	
	sFLASH_Init();				// 初始化SPI Flash
	if ( sFLASH_ReadID() == sFLASH_ID ) 	//验证ID
	{	
		printf("W25Q64 初始化成功\r\n");	
	}
	else
	{
		printf("未检测到W25Q64\r\n");
		while(1);
	}
}

