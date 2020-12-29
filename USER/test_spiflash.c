#include "main.h"

#ifdef TEST_SPIFLASH_ENABLE
#define sFLASH_SPI_PAGESIZE       256
uint8_t sFlash_Test(void)		//Flash读写测试
{
	uint16_t i = 0;
	uint8_t* sFlash_WriteBuffer=OS_Malloc(sFLASH_SPI_PAGESIZE);
	uint8_t* sFlash_ReadBuffer=OS_Malloc(sFLASH_SPI_PAGESIZE);
	for(i=0;i<sFLASH_SPI_PAGESIZE;i++)  //将要写入Flash的数据写入数组
	{
		sFlash_WriteBuffer[i] = i;
	}
	sFLASH_EraseSector(0x000000);   // 擦除扇区，FLASH写入前要先擦除  	 
	sFLASH_WriteBuffer(sFlash_WriteBuffer,0x000000, sFLASH_SPI_PAGESIZE);  // 从地址0x000000写入数据
	sFLASH_ReadBuffer(sFlash_ReadBuffer,0x000000, sFLASH_SPI_PAGESIZE);    // 从地址0x000000读取数据

	for(i=0;i<sFLASH_SPI_PAGESIZE;i++)	//验证读出的数据是否等于写入的数据
	{
		if( sFlash_WriteBuffer[i] != sFlash_ReadBuffer[i] )	//如果数据不相等，则返回0
		{
			printf("SPI_FLASH 出错！\n");
			return 0;
		}
	}		
	printf("SPI_FLASH 测试成功！\n");
	OS_Free(sFlash_WriteBuffer);
	OS_Free(sFlash_ReadBuffer);
	return 1; 	//数据正确，读写测试通过，返回1
}


//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		1024  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define LED0_TASK_PRIO		2
//任务堆栈大小	
#define LED0_STK_SIZE 		50  
//任务句柄
TaskHandle_t LED0Task_Handler;
//任务函数
void led0_task(void *pvParameters);

//任务优先级
#define LED1_TASK_PRIO		3
//任务堆栈大小	
#define LED1_STK_SIZE 		50  
//任务句柄
TaskHandle_t LED1Task_Handler;
//任务函数
void led1_task(void *pvParameters);

//任务优先级
#define FLOAT_TASK_PRIO		4
//任务堆栈大小	
#define FLOAT_STK_SIZE 		128
//任务句柄
TaskHandle_t FLOATTask_Handler;
//任务函数
void float_task(void *pvParameters);


extern void APP_Task_Start(void* p_rag);
int main(void){
	HAL_Init();		//进行了必要组件的初始化
	HW_LED_Init();
	HW_KEY_Init();
	HW_SPIFlash_Init();
	//创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();          //开启任务调度
	while(1);
}
void FAT_Init(void){
	FF_Disk_t *pxDisk;
	while((pxDisk=FF_SDDiskInit("/"))==NULL){
		printf("SD卡初始化错误!\n");
		delay_ms(1000);
	}
}
//开始任务任务函数
void start_task(void *pvParameters)
{
	FAT_Init();
    //创建LED0任务
    xTaskCreate((TaskFunction_t )led0_task,     	
                (const char*    )"led0_task",   	
                (uint16_t       )LED0_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )LED0_TASK_PRIO,	
                (TaskHandle_t*  )&LED0Task_Handler);   
    //创建LED1任务
    xTaskCreate((TaskFunction_t )led1_task,     
                (const char*    )"led1_task",   
                (uint16_t       )LED1_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )LED1_TASK_PRIO,
                (TaskHandle_t*  )&LED1Task_Handler);        
    //浮点测试任务
    xTaskCreate((TaskFunction_t )float_task,     
                (const char*    )"float_task",   
                (uint16_t       )FLOAT_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )FLOAT_TASK_PRIO,
                (TaskHandle_t*  )&FLOATTask_Handler);  
	vTaskDelete(StartTask_Handler); //删除开始任务
}

//LED0任务函数 
void led0_task(void *pvParameters)
{
    while(1)
    {
        switch(key_scan()){
            case Once_Press:
				sFlash_Test();
                break;
            default:
				delay_ms(20);
                break;
        }
    }
}   

//LED1任务函数
void led1_task(void *pvParameters)
{
    while(1)
    {
        LED1(0);
        delay_ms(200);
        LED1(1);
        delay_ms(800);
    }
}

//浮点测试任务
void float_task(void *pvParameters)
{
	while(1)
	{
		LED0_Toggle();
        vTaskDelay(1000);
	}
}











#endif