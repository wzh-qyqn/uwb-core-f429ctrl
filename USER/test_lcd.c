#include "main.h"
#include "hw_lcd.h"


#define START_TASK_PRIO		6
#define START_STK_SIZE 		1024  
TaskHandle_t StartTask_Handler;
void start_task(void *pvParameters);

#define TOUCH_TASK_PRIO		2
#define TOUCH_STK_SIZE		128
TaskHandle_t TOUCHTask_Handler;
void touch_task(void* pvParameters);

#define EMWIN_TASK_PRIO		1
#define EMWIN_STK_SIZE		1024*20
TaskHandle_t EMWINTask_Handler;
void EMWIN_task(void* pvParameters);

#define LED_TASK_PRIO		3
#define LED_STK_SIZE		128
TaskHandle_t LEDTask_Handler;
void LED_Task(void* pvParameters);

uint16_t lcd_buf[LCD_PIXEL_HEIGHT*LCD_PIXEL_WIDTH]__EXRAM;
extern void APP_Task_Start(void* p_rag);
int main(void){
	HAL_Init();		//进行了必要组件的初始化
	HW_LED_Init();
	HW_KEY_Init();
	HW_SPIFlash_Init();
	HW_Touch_Init();
	HW_CPU_TEMP_Init();
	LCD_Init();
	LCD_LayerInit(0,(uint32_t)lcd_buf,LTDC_PIXEL_FORMAT_RGB565);
	memset(lcd_buf,5,sizeof(lcd_buf)/2);
	memset(lcd_buf+LCD_PIXEL_HEIGHT*LCD_PIXEL_WIDTH/2,20,sizeof(lcd_buf)/2);
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
	vTaskDelete(StartTask_Handler); //删除开始任务
}

void EMWIN_task(void* pvParameters){
	extern void EMWin_MainTask(void);
	__HAL_RCC_CRC_CLK_ENABLE();		
//	GUI_Init();  			
//	GUI_CURSOR_Show();
//	WM_MULTIBUF_Enable(1); 
//	EMWin_MainTask();
	while(1){
//		GUI_Delay(10);
		vTaskDelay(10);
	}
}

void touch_task(void* pvParameters){
	extern void	GUI_TouchScan(void);
	while(1){
//		GUI_TouchScan();
		vTaskDelay(10);
	}
}

void LED_Task(void* pvParameters){
	while(1){
		LED0_Toggle();
		vTaskDelay(500);
	}
}
