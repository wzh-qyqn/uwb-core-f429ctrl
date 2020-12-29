#include "main.h"

#define START_TASK_PRIO		6
#define START_STK_SIZE 		1024  
TaskHandle_t StartTask_Handler;
void start_task(void *pvParameters);

#define TOUCH_TASK_PRIO		2
#define TOUCH_STK_SIZE		128
TaskHandle_t TOUCHTask_Handler;
void touch_task(void* pvParameters);

#define EMWIN_TASK_PRIO		1
#define EMWIN_STK_SIZE		1024*8
TaskHandle_t EMWINTask_Handler;
void EMWIN_task(void* pvParameters);

#define LED_TASK_PRIO		3
#define LED_STK_SIZE		128
TaskHandle_t LEDTask_Handler;
void LED_Task(void* pvParameters);

extern void APP_Task_Start(void* p_rag);
int main(void){
	HAL_Init();		//�����˱�Ҫ����ĳ�ʼ��
	HW_LED_Init();
	HW_KEY_Init();
	HW_SPIFlash_Init();
	HW_Touch_Init();
	HW_CPU_TEMP_Init();
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
	xTaskCreate((TaskFunction_t )touch_task,     
                (const char*    )"touch_task",   
                (uint16_t       )TOUCH_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )TOUCH_TASK_PRIO,
                (TaskHandle_t*  )&TOUCHTask_Handler);
	xTaskCreate((TaskFunction_t )EMWIN_task,     
                (const char*    )"EMWIN_task",   
                (uint16_t       )EMWIN_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )EMWIN_TASK_PRIO,
                (TaskHandle_t*  )&EMWINTask_Handler);
	xTaskCreate((TaskFunction_t )LED_Task,     
                (const char*    )"LED_task",   
                (uint16_t       )LED_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )LED_TASK_PRIO,
                (TaskHandle_t*  )&LEDTask_Handler);			
	vTaskDelete(StartTask_Handler); //ɾ����ʼ����
}

void EMWIN_task(void* pvParameters){
	extern void EMWin_MainTask(void);
	__HAL_RCC_CRC_CLK_ENABLE();		
	GUI_Init();  			
//	GUI_CURSOR_Show();
	WM_MULTIBUF_Enable(1); 
	EMWin_MainTask();
	while(1){
		GUI_Delay(10);
	}
}

void touch_task(void* pvParameters){
	while(1){
		Touch_Scan();
		GUI_TOUCH_Exec();
		vTaskDelay(10);
	}
}

void LED_Task(void* pvParameters){
	while(1){
		LED0_Toggle();
		vTaskDelay(500);
	}
}
