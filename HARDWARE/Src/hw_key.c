#include "hw_key.h"

#define KEY_CHECK_TIME			30

void HW_KEY_Init(void){
	
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_GPIOI_CLK_ENABLE();
	
	GPIO_Initure.Mode=GPIO_MODE_INPUT;
	GPIO_Initure.Pin=GPIO_PIN_8;
	GPIO_Initure.Pull=GPIO_PULLUP;
	GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;
	
	HAL_GPIO_Init(GPIOI,&GPIO_Initure);
	
}

KEY_Press key_scan(void)
{
	uint32_t i=0;
	uint8_t res=1;
	uint8_t k=0;
	if(HAL_GPIO_ReadPin(GPIOI,GPIO_PIN_8)==0)
	{
		delay_ms(10);//消抖
		if(HAL_GPIO_ReadPin(GPIOI,GPIO_PIN_8)==0)//检测到按压按键
		{
			for(i=0;i<KEY_CHECK_TIME;i++)//普遍情况下留300ms进行后续判断
			{
				if(HAL_GPIO_ReadPin(GPIOI,GPIO_PIN_8)==1)//检测到在此时间内释放按键
				{
					while(HAL_GPIO_ReadPin(GPIOI,GPIO_PIN_8)==1)//等待再次按压
					{
						delay_ms(10);
						k++;
						if(k>KEY_CHECK_TIME)//无论按压多少次，最后一定会进入到这里
						{
							switch(res)
							{									//按压次数扩展区
								case 1:return Once_Press;
								case 2:return Double_Press;
								case 3:return Third_Press;
								default :return No_Press;
							}
						}
					}
					i=0;//已检测到一个跳变沿判断时间延长
					k=0;//清空内部延时标志位
					res++;//跳变沿计数加一
				}
				delay_ms(10);
			}
			for(i=0;HAL_GPIO_ReadPin(GPIOI,GPIO_PIN_8)==0;i++)//进行长按判断
			{
				delay_ms(10);
			}
			if(i<50)						
				return Once_Press;
			else if(i>=50 && i<300)								//按压时间扩展区
				return Mid_Press;
			else return Long_Press;
		}
	}
	return No_Press;
}

static TimerHandle_t Key_Handle=NULL;
static StaticTimer_t Key_TimeBuf;
static bool Key_is_press=false;
static bool Key_is_longpress=false;
bool Key_IS_Press(void){
	if(Key_is_press){
		Key_is_press=false;
		return true;
	}
	return false;
}

bool Key_IS_LongPress(void){
	if(Key_is_press&&Key_is_longpress){
		Key_is_press=false;
		Key_is_longpress=false;
		return true;
	}
	return false;
}
static void Key_Timer_Scan(xTimerHandle pcTimer){
	static uint8_t state=0;
	static uint32_t key_press_time=0;
	switch(state){
		case 0:
			if(HAL_GPIO_ReadPin(GPIOI,GPIO_PIN_8)==0)
				state=1;
			break;
		case 1:
			if(HAL_GPIO_ReadPin(GPIOI,GPIO_PIN_8)==0){
				state=2;
				Key_is_press=true;
			}
			else
				state=0;
			break;
		case 2:
			if(HAL_GPIO_ReadPin(GPIOI,GPIO_PIN_8)==0){
				key_press_time++;
				if(key_press_time==300){
					Key_is_longpress=true;
				}
			}
			else{
				state=0;
				key_press_time=0;
			}
			break;
		default:
			state=0;
			key_press_time=0;
			break;
	}
}
void Key_Timer_Create(void){
	BaseType_t sta=pdFAIL;
	Key_Handle=xTimerCreateStatic("Key_Timer",\
						10,\
						pdTRUE,\
						NULL,\
						Key_Timer_Scan,\
						&Key_TimeBuf);
	sta=xTimerStart(Key_Handle,0);
	if(sta!=pdPASS)
		printf("按键扫描定时器启动错误！\n");
}







