#include "main.h"

#ifdef TEST_SPIFLASH_ENABLE
#define sFLASH_SPI_PAGESIZE       256
uint8_t sFlash_Test(void)		//Flash��д����
{
	uint16_t i = 0;
	uint8_t* sFlash_WriteBuffer=OS_Malloc(sFLASH_SPI_PAGESIZE);
	uint8_t* sFlash_ReadBuffer=OS_Malloc(sFLASH_SPI_PAGESIZE);
	for(i=0;i<sFLASH_SPI_PAGESIZE;i++)  //��Ҫд��Flash������д������
	{
		sFlash_WriteBuffer[i] = i;
	}
	sFLASH_EraseSector(0x000000);   // ����������FLASHд��ǰҪ�Ȳ���  	 
	sFLASH_WriteBuffer(sFlash_WriteBuffer,0x000000, sFLASH_SPI_PAGESIZE);  // �ӵ�ַ0x000000д������
	sFLASH_ReadBuffer(sFlash_ReadBuffer,0x000000, sFLASH_SPI_PAGESIZE);    // �ӵ�ַ0x000000��ȡ����

	for(i=0;i<sFLASH_SPI_PAGESIZE;i++)	//��֤�����������Ƿ����д�������
	{
		if( sFlash_WriteBuffer[i] != sFlash_ReadBuffer[i] )	//������ݲ���ȣ��򷵻�0
		{
			printf("SPI_FLASH ����\n");
			return 0;
		}
	}		
	printf("SPI_FLASH ���Գɹ���\n");
	OS_Free(sFlash_WriteBuffer);
	OS_Free(sFlash_ReadBuffer);
	return 1; 	//������ȷ����д����ͨ��������1
}


//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С	
#define START_STK_SIZE 		1024  
//������
TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);

//�������ȼ�
#define LED0_TASK_PRIO		2
//�����ջ��С	
#define LED0_STK_SIZE 		50  
//������
TaskHandle_t LED0Task_Handler;
//������
void led0_task(void *pvParameters);

//�������ȼ�
#define LED1_TASK_PRIO		3
//�����ջ��С	
#define LED1_STK_SIZE 		50  
//������
TaskHandle_t LED1Task_Handler;
//������
void led1_task(void *pvParameters);

//�������ȼ�
#define FLOAT_TASK_PRIO		4
//�����ջ��С	
#define FLOAT_STK_SIZE 		128
//������
TaskHandle_t FLOATTask_Handler;
//������
void float_task(void *pvParameters);


extern void APP_Task_Start(void* p_rag);
int main(void){
	HAL_Init();		//�����˱�Ҫ����ĳ�ʼ��
	HW_LED_Init();
	HW_KEY_Init();
	HW_SPIFlash_Init();
	//������ʼ����
    xTaskCreate((TaskFunction_t )start_task,            //������
                (const char*    )"start_task",          //��������
                (uint16_t       )START_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������              
    vTaskStartScheduler();          //�����������
	while(1);
}
void FAT_Init(void){
	FF_Disk_t *pxDisk;
	while((pxDisk=FF_SDDiskInit("/"))==NULL){
		printf("SD����ʼ������!\n");
		delay_ms(1000);
	}
}
//��ʼ����������
void start_task(void *pvParameters)
{
	FAT_Init();
    //����LED0����
    xTaskCreate((TaskFunction_t )led0_task,     	
                (const char*    )"led0_task",   	
                (uint16_t       )LED0_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )LED0_TASK_PRIO,	
                (TaskHandle_t*  )&LED0Task_Handler);   
    //����LED1����
    xTaskCreate((TaskFunction_t )led1_task,     
                (const char*    )"led1_task",   
                (uint16_t       )LED1_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )LED1_TASK_PRIO,
                (TaskHandle_t*  )&LED1Task_Handler);        
    //�����������
    xTaskCreate((TaskFunction_t )float_task,     
                (const char*    )"float_task",   
                (uint16_t       )FLOAT_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )FLOAT_TASK_PRIO,
                (TaskHandle_t*  )&FLOATTask_Handler);  
	vTaskDelete(StartTask_Handler); //ɾ����ʼ����
}

//LED0������ 
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

//LED1������
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

//�����������
void float_task(void *pvParameters)
{
	while(1)
	{
		LED0_Toggle();
        vTaskDelay(1000);
	}
}











#endif