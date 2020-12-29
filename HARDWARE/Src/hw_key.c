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
		delay_ms(10);//����
		if(HAL_GPIO_ReadPin(GPIOI,GPIO_PIN_8)==0)//��⵽��ѹ����
		{
			for(i=0;i<KEY_CHECK_TIME;i++)//�ձ��������300ms���к����ж�
			{
				if(HAL_GPIO_ReadPin(GPIOI,GPIO_PIN_8)==1)//��⵽�ڴ�ʱ�����ͷŰ���
				{
					while(HAL_GPIO_ReadPin(GPIOI,GPIO_PIN_8)==1)//�ȴ��ٴΰ�ѹ
					{
						delay_ms(10);
						k++;
						if(k>KEY_CHECK_TIME)//���۰�ѹ���ٴΣ����һ������뵽����
						{
							switch(res)
							{									//��ѹ������չ��
								case 1:return Once_Press;
								case 2:return Double_Press;
								case 3:return Third_Press;
								default :return No_Press;
							}
						}
					}
					i=0;//�Ѽ�⵽һ���������ж�ʱ���ӳ�
					k=0;//����ڲ���ʱ��־λ
					res++;//�����ؼ�����һ
				}
				delay_ms(10);
			}
			for(i=0;HAL_GPIO_ReadPin(GPIOI,GPIO_PIN_8)==0;i++)//���г����ж�
			{
				delay_ms(10);
			}
			if(i<50)						
				return Once_Press;
			else if(i>=50 && i<300)								//��ѹʱ����չ��
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
		printf("����ɨ�趨ʱ����������\n");
}







