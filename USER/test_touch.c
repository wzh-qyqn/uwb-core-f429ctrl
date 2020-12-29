#include "main.h"

#ifdef TEST_TOUCH_ENABLE
#include "test_iic_touch.h"

////任务优先级
//#define START_TASK_PRIO		6
////任务堆栈大小	
//#define START_STK_SIZE 		1024  
////任务句柄
//TaskHandle_t StartTask_Handler;
////任务函数
//void start_task(void *pvParameters);

////任务优先级
//#define LED0_TASK_PRIO		5
////任务堆栈大小	
//#define LED0_STK_SIZE 		512  
////任务句柄
//TaskHandle_t LED0Task_Handler;
////任务函数
//void led0_task(void *pvParameters);

////任务优先级
//#define LED1_TASK_PRIO		4
////任务堆栈大小	
//#define LED1_STK_SIZE 		50  
////任务句柄
//TaskHandle_t LED1Task_Handler;
////任务函数
//void led1_task(void *pvParameters);

////任务优先级
//#define FLOAT_TASK_PRIO		3
////任务堆栈大小	
//#define FLOAT_STK_SIZE 		128
////任务句柄
//TaskHandle_t FLOATTask_Handler;
////任务函数
//void float_task(void *pvParameters);

//#define TOUCH_TASK_PRIO		2
//#define TOUCH_STK_SIZE		128
//TaskHandle_t TOUCHTask_Handler;
//void touch_task(void* pvParameters);

extern void APP_Task_Start(void* p_rag);
int main(void){
	HAL_Init();		//进行了必要组件的初始化
//	HW_LED_Init();
//	HW_KEY_Init();
//	HW_SPIFlash_Init();
	Touch_Init();
//	HW_CPU_TEMP_Init();
	//创建开始任务
//    xTaskCreate((TaskFunction_t )start_task,            //任务函数
//                (const char*    )"start_task",          //任务名称
//                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
//                (void*          )NULL,                  //传递给任务函数的参数
//                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
//                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
//    vTaskStartScheduler();          //开启任务调度
	while(1){
		Touch_Scan();
		if(touchInfo.flag==1)
		{
			printf("坐标：%d %d",touchInfo.x[0],touchInfo.y[0]);
		}
		delay_ms(100);
	}
}
void FAT_Init(void){
	FF_Disk_t *pxDisk;
	while((pxDisk=FF_SDDiskInit("/"))==NULL){
		printf("SD卡初始化错误!\n");
		delay_ms(1000);
	}
}
////开始任务任务函数
//void start_task(void *pvParameters)
//{
//	FAT_Init();
//    //创建LED0任务
//    xTaskCreate((TaskFunction_t )led0_task,     	
//                (const char*    )"led0_task",   	
//                (uint16_t       )LED0_STK_SIZE, 
//                (void*          )NULL,				
//                (UBaseType_t    )LED0_TASK_PRIO,	
//                (TaskHandle_t*  )&LED0Task_Handler);   
//    //创建LED1任务
//    xTaskCreate((TaskFunction_t )led1_task,     
//                (const char*    )"led1_task",   
//                (uint16_t       )LED1_STK_SIZE, 
//                (void*          )NULL,
//                (UBaseType_t    )LED1_TASK_PRIO,
//                (TaskHandle_t*  )&LED1Task_Handler);        
//    //浮点测试任务
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
//	vTaskDelete(StartTask_Handler); //删除开始任务
//}

////LED0任务函数 
//void led0_task(void *pvParameters)
//{
//    while(1)
//    {
//        switch(key_scan()){
//			case Once_Press:
//				printf("CPU温度：%2.2f\n",(float)Get_Cpu_Temprate());
//				break;
//            default:
//                break;
//        }
//		vTaskDelay(20);
//    }
//}   

////LED1任务函数
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

////浮点测试任务
//void float_task(void *pvParameters)
//{
//	static float float_num=0.00;
//	while(1)
//	{
//		float_num+=0.01f;
////		printf("float_num的值为: %.4f\r\n",float_num);
//        vTaskDelay(1000);
//	}
//}

//void touch_task(void* pvParameters){
//	while(1){
//		Touch_Scan();
//		if(touchInfo.flag)
//			printf("触摸坐标：%d %d\n",touchInfo.x[0],touchInfo.y[0]);
//		vTaskDelay(10);
//	}
//}

#endif