#include "main.h"

#ifdef TEST_TOUCH_ENABLE
#include "test_iic_touch.h"

////�������ȼ�
//#define START_TASK_PRIO		6
////�����ջ��С	
//#define START_STK_SIZE 		1024  
////������
//TaskHandle_t StartTask_Handler;
////������
//void start_task(void *pvParameters);

////�������ȼ�
//#define LED0_TASK_PRIO		5
////�����ջ��С	
//#define LED0_STK_SIZE 		512  
////������
//TaskHandle_t LED0Task_Handler;
////������
//void led0_task(void *pvParameters);

////�������ȼ�
//#define LED1_TASK_PRIO		4
////�����ջ��С	
//#define LED1_STK_SIZE 		50  
////������
//TaskHandle_t LED1Task_Handler;
////������
//void led1_task(void *pvParameters);

////�������ȼ�
//#define FLOAT_TASK_PRIO		3
////�����ջ��С	
//#define FLOAT_STK_SIZE 		128
////������
//TaskHandle_t FLOATTask_Handler;
////������
//void float_task(void *pvParameters);

//#define TOUCH_TASK_PRIO		2
//#define TOUCH_STK_SIZE		128
//TaskHandle_t TOUCHTask_Handler;
//void touch_task(void* pvParameters);

extern void APP_Task_Start(void* p_rag);
int main(void){
	HAL_Init();		//�����˱�Ҫ����ĳ�ʼ��
//	HW_LED_Init();
//	HW_KEY_Init();
//	HW_SPIFlash_Init();
	Touch_Init();
//	HW_CPU_TEMP_Init();
	//������ʼ����
//    xTaskCreate((TaskFunction_t )start_task,            //������
//                (const char*    )"start_task",          //��������
//                (uint16_t       )START_STK_SIZE,        //�����ջ��С
//                (void*          )NULL,                  //���ݸ��������Ĳ���
//                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
//                (TaskHandle_t*  )&StartTask_Handler);   //������              
//    vTaskStartScheduler();          //�����������
	while(1){
		Touch_Scan();
		if(touchInfo.flag==1)
		{
			printf("���꣺%d %d",touchInfo.x[0],touchInfo.y[0]);
		}
		delay_ms(100);
	}
}
void FAT_Init(void){
	FF_Disk_t *pxDisk;
	while((pxDisk=FF_SDDiskInit("/"))==NULL){
		printf("SD����ʼ������!\n");
		delay_ms(1000);
	}
}
////��ʼ����������
//void start_task(void *pvParameters)
//{
//	FAT_Init();
//    //����LED0����
//    xTaskCreate((TaskFunction_t )led0_task,     	
//                (const char*    )"led0_task",   	
//                (uint16_t       )LED0_STK_SIZE, 
//                (void*          )NULL,				
//                (UBaseType_t    )LED0_TASK_PRIO,	
//                (TaskHandle_t*  )&LED0Task_Handler);   
//    //����LED1����
//    xTaskCreate((TaskFunction_t )led1_task,     
//                (const char*    )"led1_task",   
//                (uint16_t       )LED1_STK_SIZE, 
//                (void*          )NULL,
//                (UBaseType_t    )LED1_TASK_PRIO,
//                (TaskHandle_t*  )&LED1Task_Handler);        
//    //�����������
//    xTaskCreate((TaskFunction_t )float_task,     
//                (const char*    )"float_task",   
//                (uint16_t       )FLOAT_STK_SIZE, 
//                (void*          )NULL,
//                (UBaseType_t    )FLOAT_TASK_PRIO,
//                (TaskHandle_t*  )&FLOATTask_Handler);
//	xTaskCreate((TaskFunction_t )touch_task,     
//                (const char*    )"touch_task",   
//                (uint16_t       )TOUCH_STK_SIZE, 
//                (void*          )NULL,
//                (UBaseType_t    )TOUCH_TASK_PRIO,
//                (TaskHandle_t*  )&TOUCHTask_Handler);
//	vTaskDelete(StartTask_Handler); //ɾ����ʼ����
//}

////LED0������ 
//void led0_task(void *pvParameters)
//{
//    while(1)
//    {
//        switch(key_scan()){
//			case Once_Press:
//				printf("CPU�¶ȣ�%2.2f\n",(float)Get_Cpu_Temprate());
//				break;
//            default:
//                break;
//        }
//		vTaskDelay(20);
//    }
//}   

////LED1������
//void led1_task(void *pvParameters)
//{
//    while(1)
//    {
//        LED1(0);
//        delay_ms(200);
//        LED1(1);
//        delay_ms(800);
//    }
//}

////�����������
//void float_task(void *pvParameters)
//{
//	static float float_num=0.00;
//	while(1)
//	{
//		float_num+=0.01f;
////		printf("float_num��ֵΪ: %.4f\r\n",float_num);
//        vTaskDelay(1000);
//	}
//}

//void touch_task(void* pvParameters){
//	while(1){
//		Touch_Scan();
//		if(touchInfo.flag)
//			printf("�������꣺%d %d\n",touchInfo.x[0],touchInfo.y[0]);
//		vTaskDelay(10);
//	}
//}

#endif