#include "hw_sdram.h"

#define SDRAM_Size 0x02000000  //32M字节
#define SDRAM_BANK_ADDR     ((uint32_t)0xD0000000) 				// FMC SDRAM 数据基地址
#define FMC_COMMAND_TARGET_BANK   FMC_SDRAM_CMD_TARGET_BANK2	//	SDRAM 的bank选择
#define SDRAM_TIMEOUT     ((uint32_t)0x1000) 						// 超时判断时间

#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000) 
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200) 


static SDRAM_HandleTypeDef hsdram2;			// SDRAM_HandleTypeDef 结构体变量
static FMC_SDRAM_CommandTypeDef command;	// 控制指令

/*************************0*****************************************************************************
*	函 数 名: SDRAM_delay
*	入口参数: 无
*	返 回 值: 无
*	函数功能: 简易延时函数，单位约为5ms
*	说    明: 无
*******************************************************************************************************/

static void SDRAM_delay(__IO uint32_t nCount)
{
  __IO uint32_t index = 0; 
	
  for(index = (100000 * nCount); index != 0; index--);

}

/*************************************************************************************************
*	函 数 名:	HAL_FMC_MspInit
*	入口参数:	无
*	返 回 值:无
*	函数功能:	初始化sdram引脚
*	说    明:该函数的代码由 CubeMX （版本5.3.0）生成
*************************************************************************************************/

void HAL_FMC_MspInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct ={0};

	__HAL_RCC_FMC_CLK_ENABLE();		// 开启FMC时钟

	__HAL_RCC_GPIOF_CLK_ENABLE();		// 开启GPIO时钟
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

  /** FMC GPIO Configuration  
	
  PF0   ------> FMC_A0			  PC0   ------> FMC_SDNWE			PD14  ------> FMC_D0
  PF1   ------> FMC_A1          PF11  ------> FMC_SDNRAS       PD15  ------> FMC_D1
  PF2   ------> FMC_A2          PH6   ------> FMC_SDNE1        PD0   ------> FMC_D2
  PF3   ------> FMC_A3          PH7   ------> FMC_SDCKE1       PD1   ------> FMC_D3  
  PF4   ------> FMC_A4          PG4   ------> FMC_BA0          PE7   ------> FMC_D4
  PF5   ------> FMC_A5          PG5   ------> FMC_BA1          PE8   ------> FMC_D5
  PF12  ------> FMC_A6          PG8   ------> FMC_SDCLK        PE9   ------> FMC_D6
  PF13  ------> FMC_A7          PG15  ------> FMC_SDNCAS       PE10  ------> FMC_D7
  PF14  ------> FMC_A8          PE0    ------> FMC_NBL0        PE11  ------> FMC_D8
  PF15  ------> FMC_A9          PE1    ------> FMC_NBL1        PE12  ------> FMC_D9
  PG0   ------> FMC_A10                                        PE13  ------> FMC_D10
  PG1   ------> FMC_A11	                                       PE14  ------> FMC_D11
  PG2   ------> FMC_A12	                                       PE15  ------> FMC_D12
	                                                            PD8   ------> FMC_D13
                                                               PD9   ------> FMC_D14
                                                               PD10  ------> FMC_D15
  */  
  
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3 
								  |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_11|GPIO_PIN_12 
								  |GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_4 
								  |GPIO_PIN_5|GPIO_PIN_8|GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10 
								  |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14 
								  |GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
	HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_14 
								  |GPIO_PIN_15|GPIO_PIN_0|GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	
}

/*************************************************************************************************
*	函 数 名:	HAL_SDRAM_MspInit
*	入口参数:	hsdram - SDRAM_HandleTypeDef定义的变量，即表示定义的sdram
*	返 回 值:无
*	函数功能:	初始化sdram引脚，在函数 HAL_SDRAM_Init 中被调用
*	说    明:无		
*************************************************************************************************/

void HAL_SDRAM_MspInit(SDRAM_HandleTypeDef* hsdram)
{
	HAL_FMC_MspInit();
}

/******************************************************************************************************
*	函 数 名: SDRAM_Initialization_Sequence
*	入口参数: hsdram - SDRAM_HandleTypeDef定义的变量，即表示定义的sdram
*				 Command	- 控制指令
*	返 回 值: 无
*	函数功能: SDRAM 参数配置
*	说    明: 配置SDRAM相关时序和控制方式
*******************************************************************************************************/

static void SDRAM_Initialization_Sequence(SDRAM_HandleTypeDef *hsdram, FMC_SDRAM_CommandTypeDef *Command)
{
  __IO uint32_t tmpmrd = 0;
  
  /* Configure a clock configuration enable command */
  Command->CommandMode 					= FMC_SDRAM_CMD_CLK_ENABLE;	// 开启SDRAM时钟 
  Command->CommandTarget 				= FMC_COMMAND_TARGET_BANK; 	// 选择要控制的区域
  Command->AutoRefreshNumber 			= 1;
  Command->ModeRegisterDefinition 		= 0;
  
  HAL_SDRAM_SendCommand(hsdram, Command, SDRAM_TIMEOUT);	// 发送控制指令
  SDRAM_delay(1);		// 延时等待
  
  /* Configure a PALL (precharge all) command */ 
  Command->CommandMode 					= FMC_SDRAM_CMD_PALL;		// 预充电命令
  Command->CommandTarget 				= FMC_COMMAND_TARGET_BANK;	// 选择要控制的区域
  Command->AutoRefreshNumber 			= 1;
  Command->ModeRegisterDefinition 		= 0;
  
  HAL_SDRAM_SendCommand(hsdram, Command, SDRAM_TIMEOUT);  // 发送控制指令
  
  /* Configure a Auto-Refresh command */ 
  Command->CommandMode 					= FMC_SDRAM_CMD_AUTOREFRESH_MODE;	// 使用自动刷新
  Command->CommandTarget 				= FMC_COMMAND_TARGET_BANK;          // 选择要控制的区域
  Command->AutoRefreshNumber			= 8;                                // 自动刷新次数
  Command->ModeRegisterDefinition 		= 0;
  
  HAL_SDRAM_SendCommand(hsdram, Command, SDRAM_TIMEOUT);	// 发送控制指令
  
  /* Program the external memory mode register */
  tmpmrd = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_2          |
                     SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |
                     SDRAM_MODEREG_CAS_LATENCY_2           |
                     SDRAM_MODEREG_OPERATING_MODE_STANDARD |
                     SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;
  
  Command->CommandMode					= FMC_SDRAM_CMD_LOAD_MODE;	// 加载模式寄存器命令
  Command->CommandTarget 				= FMC_COMMAND_TARGET_BANK;	// 选择要控制的区域
  Command->AutoRefreshNumber 			= 1;
  Command->ModeRegisterDefinition 	= tmpmrd;
  
  HAL_SDRAM_SendCommand(hsdram, Command, SDRAM_TIMEOUT);	// 发送控制指令
  
  hsdram->Instance->SDRTR |= ((uint32_t)((683)<< 1));	// 设置刷新计数器 
}

/******************************************************************************************************
*	函 数 名: SDRAM_Init
*	入口参数: 无
*	返 回 值: 无
*	函数功能: SDRAM初始化
*	说    明: 初始化FMC和SDRAM配置
*******************************************************************************************************/

void HW_SDRAM_Init(void)
{

	FMC_SDRAM_TimingTypeDef SdramTiming = {0};

	hsdram2.Instance = FMC_SDRAM_DEVICE;

	hsdram2.Init.SDBank 				= FMC_SDRAM_BANK2;					   // 选择BANK区
	hsdram2.Init.ColumnBitsNumber 		= FMC_SDRAM_COLUMN_BITS_NUM_9;         // 行地址宽度
	hsdram2.Init.RowBitsNumber 			= FMC_SDRAM_ROW_BITS_NUM_13;           // 列地址线宽度
	hsdram2.Init.MemoryDataWidth 		= FMC_SDRAM_MEM_BUS_WIDTH_16;          // 数据宽度  
	hsdram2.Init.InternalBankNumber 	= FMC_SDRAM_INTERN_BANKS_NUM_4;        // bank数量
	hsdram2.Init.CASLatency 			= FMC_SDRAM_CAS_LATENCY_2;             // CAS 
	hsdram2.Init.WriteProtection 		= FMC_SDRAM_WRITE_PROTECTION_DISABLE;  // 禁止写保护
	hsdram2.Init.SDClockPeriod 			= FMC_SDRAM_CLOCK_PERIOD_2;            // 分频
	hsdram2.Init.ReadBurst 				= FMC_SDRAM_RBURST_ENABLE;             // 突发模式  
	hsdram2.Init.ReadPipeDelay 			= FMC_SDRAM_RPIPE_DELAY_1;             // 读延迟

	SdramTiming.LoadToActiveDelay 		= 2;	 // TMRD: min=12ns (2x11.11ns)  
	SdramTiming.ExitSelfRefreshDelay 	= 7;     // TXSR: min=72ns (7x11.11ns)  
	SdramTiming.SelfRefreshTime			= 4;     // TRAS: min=42ns (4x11.11ns)    
	SdramTiming.RowCycleDelay 			= 7;     // TRC:  min=60ns (7x11.11ns)   
	SdramTiming.WriteRecoveryTime 		= 2;     // TWR:  2 Tck 
	SdramTiming.RPDelay 				= 2;     // TRP:  18ns => 2x11.11ns       
	SdramTiming.RCDDelay 				= 2;     // TRCD: 12ns => 2x11.11ns 

	HAL_SDRAM_Init(&hsdram2, &SdramTiming);		// 初始化FMC接口
															
	SDRAM_Initialization_Sequence(&hsdram2,&command);//配置SDRAM
}

/******************************************************************************************************
*	函 数 名: SDRAM_Test
*	入口参数: 无
*	返 回 值: SUCCESS - 成功，ERROR - 失败
*	函数功能: SDRAM测试
*	说    明: 先以16位的数据宽度写入数据，再读取出来一一进行比较，随后以8位的数据宽度写入，
*				 用以验证NBL0和NBL1两个引脚的连接是否正常。          
*******************************************************************************************************/

uint8_t SDRAM_Test(void)
{
	uint32_t i = 0;		// 计数变量
	uint16_t ReadData = 0; 	// 读取到的数据
	uint8_t  ReadData_8b;
	
	printf("STM32F429 SDRAM测试\r\n");
	printf("测试开始，以16位数据宽度写入数据...\r\n");	
	
	for (i = 0; i < SDRAM_Size/2; i++)
	{
 		*(__IO uint16_t*) (SDRAM_BANK_ADDR + 2*i) = (uint16_t)i;		// 写入数据
	}
	
	printf("写入完毕，读取数据并比较...\r\n");
	for(i = 0; i < SDRAM_Size/2;i++ )
	{
		ReadData = *(__IO uint16_t*)(SDRAM_BANK_ADDR + 2 * i );  // 从SDRAM读出数据	
		if( ReadData != (uint16_t)i )      //检测数据，若不相等，跳出函数,返回检测失败结果。
		{
			printf("SDRAM测试失败！！\r\n");
			return ERROR;	 // 返回失败标志
		}
	}
	
	printf("16位数据宽度读写通过，以8位数据宽度写入数据\r\n");
	for (i = 0; i < 255; i++)
	{
 		*(__IO uint8_t*) (SDRAM_BANK_ADDR + i) =  (uint8_t)i;
	}	
	printf("写入完毕，读取数据并比较...\r\n");
	for (i = 0; i < 255; i++)
	{
		ReadData_8b = *(__IO uint8_t*) (SDRAM_BANK_ADDR + i);
		if( ReadData_8b != (uint8_t)i )      //检测数据，若不相等，跳出函数,返回检测失败结果。
		{
			printf("8位数据宽度读写测试失败！！\r\n");
			printf("请检查NBL0和NBL1的连接\r\n");	
			return ERROR;	 // 返回失败标志
		}
	}		
	printf("8位数据宽度读写通过\r\n");
	printf("SDRAM读写测试通过，系统正常\r\n");
	return SUCCESS;	 // 返回成功标志
}
