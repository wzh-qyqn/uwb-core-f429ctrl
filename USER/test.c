#include "main.h"

uint32_t test_exram[EXRAM_SIZE/4]__EXRAM;
uint32_t test_inram[INRAM_SIZE/4]__INRAM;
void RAM_TEST(){
	uint32_t i;
	for(i=0;i<EXRAM_SIZE/4;i++){
		test_exram[i]=i;
	}
	for(i=0;i<INRAM_SIZE/4;i++){
		test_inram[i]=i;
	}
	for(i=0;i<EXRAM_SIZE/4;i++){
		if(i!=test_exram[i]){
			printf("SDRAM测试失败！%d,0x%p\r\n",i,test_exram);
			break;
		}
	}
	if(i==EXRAM_SIZE/4)
		printf("SDRAM测试成功！%d,0x%p\r\n",i,test_exram);
	for(i=0;i<INRAM_SIZE/4;i++){
		if(i!=test_inram[i]){
			printf("INRAM测试失败！%d,0x%p\r\n",i,test_inram);
			break;
		}
	}
	if(i==INRAM_SIZE/4)
		printf("INRAM测试成功！%d,0x%p\r\n",i,test_inram);
}


extern HAL_SD_CardInfoTypedef  SDCardInfo;         //SD卡信息结构体



void show_sdcard_info(void)
{
	switch(SDCardInfo.CardType)
	{
		case STD_CAPACITY_SD_CARD_V1_1:printf("Card Type:SDSC V1.1\r\n");break;
		case STD_CAPACITY_SD_CARD_V2_0:printf("Card Type:SDSC V2.0\r\n");break;
		case HIGH_CAPACITY_SD_CARD:printf("Card Type:SDHC V2.0\r\n");break;
		case MULTIMEDIA_CARD:printf("Card Type:MMC Card\r\n");break;
	}	
  	printf("Card ManufacturerID:%d\r\n",SDCardInfo.SD_cid.ManufacturerID);	//制造商ID
 	printf("Card RCA:%d\r\n",SDCardInfo.RCA);								//卡相对地址
	printf("Card Capacity:%d MB\r\n",(uint32_t)(SDCardInfo.CardCapacity>>20));	//显示容量
 	printf("Card BlockSize:%d\r\n\r\n",SDCardInfo.CardBlockSize);			//显示块大小
}


int main(void){
	HAL_Init();
	Stm32_Clock_Init(360,25,2,8);
	delay_init(180);
	uart_init(115200);
	HW_SDRAM_Init();
	HW_LED_Init();
	HW_KEY_Init();
	while(1){
	}
}




