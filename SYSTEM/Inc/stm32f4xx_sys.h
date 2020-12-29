#ifndef STM32F4XX_SYS__H
#define STM32F4XX_SYS__H
#include "stm32f4xx.h"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#define SYSTEM_SUPPORT_OS		1		//定义系统文件夹是否支持OS
#define DEBUG_SHOW				0		//定义调试信息显示的方式

#if SYSTEM_SUPPORT_OS==1
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#include "stream_buffer.h"
enum OS_Prio
{
	Start_Prio=configMAX_PRIORITIES-2,
	Fresh_Prio=3,
	Touch_Prio=2,
	EMWin_Prio=1,
};
//-Wno-invalid-source-encoding
#endif

//声明不初始化的外部存储
//使用时注意，此声明所定义的变量的初值是随机的
//依赖于上次运行时写入的值
//使用外部SDRAM时应注意资源争夺的问题
#if defined(__clang__)
#define __EXRAM			__attribute__((section(".bss.EXRAM")))
#define __INRAM			__attribute__((section(".bss.INRAM")))
#elif  defined(__CC_ARM)
#define __EXRAM			__attribute__((section(".bss.EXRAM"),zero_init))
#define __INRAM			__attribute__((section(".bss.INRAM"),zero_init))
#else
#define __EXRAM
#define __INRAM
#endif
//位带操作,实现51类似的GPIO控制功能
//具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).M4同M3类似,只是寄存器地址变了.
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+20) //0x40020014
#define GPIOB_ODR_Addr    (GPIOB_BASE+20) //0x40020414 
#define GPIOC_ODR_Addr    (GPIOC_BASE+20) //0x40020814 
#define GPIOD_ODR_Addr    (GPIOD_BASE+20) //0x40020C14 
#define GPIOE_ODR_Addr    (GPIOE_BASE+20) //0x40021014 
#define GPIOF_ODR_Addr    (GPIOF_BASE+20) //0x40021414    
#define GPIOG_ODR_Addr    (GPIOG_BASE+20) //0x40021814   
#define GPIOH_ODR_Addr    (GPIOH_BASE+20) //0x40021C14    
#define GPIOI_ODR_Addr    (GPIOI_BASE+20) //0x40022014 
#define GPIOJ_ODR_ADDr    (GPIOJ_BASE+20) //0x40022414
#define GPIOK_ODR_ADDr    (GPIOK_BASE+20) //0x40022814

#define GPIOA_IDR_Addr    (GPIOA_BASE+16) //0x40020010 
#define GPIOB_IDR_Addr    (GPIOB_BASE+16) //0x40020410 
#define GPIOC_IDR_Addr    (GPIOC_BASE+16) //0x40020810 
#define GPIOD_IDR_Addr    (GPIOD_BASE+16) //0x40020C10 
#define GPIOE_IDR_Addr    (GPIOE_BASE+16) //0x40021010 
#define GPIOF_IDR_Addr    (GPIOF_BASE+16) //0x40021410 
#define GPIOG_IDR_Addr    (GPIOG_BASE+16) //0x40021810 
#define GPIOH_IDR_Addr    (GPIOH_BASE+16) //0x40021C10 
#define GPIOI_IDR_Addr    (GPIOI_BASE+16) //0x40022010 
#define GPIOJ_IDR_Addr    (GPIOJ_BASE+16) //0x40022410 
#define GPIOK_IDR_Addr    (GPIOK_BASE+16) //0x40022810 

//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入

#define PHout(n)   BIT_ADDR(GPIOH_ODR_Addr,n)  //输出 
#define PHin(n)    BIT_ADDR(GPIOH_IDR_Addr,n)  //输入

#define PIout(n)   BIT_ADDR(GPIOI_ODR_Addr,n)  //输出 
#define PIin(n)    BIT_ADDR(GPIOI_IDR_Addr,n)  //输入
//////////////////////////////////////////////////////////////////////////////////  

#if SYSTEM_SUPPORT_OS==1
__STATIC_FORCEINLINE void* OS_Malloc(size_t len){
	return pvPortMalloc(len);
}
__STATIC_FORCEINLINE void OS_Free(void* block){
	vPortFree(block);
}
__STATIC_FORCEINLINE void* OS_Realloc(void* block,size_t want_size){
	extern void *pvPortRealloc( void* pv,size_t xWantedSize );
	return pvPortRealloc(block,want_size);
}
__STATIC_FORCEINLINE void* OS_Calloc(size_t nmemb, size_t size){
	void* p;
	p=OS_Malloc(size*nmemb);
	if(p!=NULL)
		memset(p,'\0',size*nmemb);
	return p;
}
__STATIC_FORCEINLINE void OS_Lock(void){
	vTaskSuspendAll();
}
__STATIC_FORCEINLINE void OS_Unlock(void){
	xTaskResumeAll();
}
#elif SYSTEM_SUPPORT_OS==0 
__STATIC_FORCEINLINE void* OS_Malloc(size_t len){
	return malloc(len);
}
__STATIC_FORCEINLINE void OS_Free(void* block){
	free(block);
}
#define OS_Lock()
#define OS_Unlock()

#endif

#ifdef __cplusplus
 extern "C" {
#endif
#if (DEBUG_SHOW==1)	
extern typedef struct Debug_Ctrl
{
	const uint32_t DEBUG_MAX_BUF;
	char* const Debug_Buffer;
	bool fresh_flag;
}Debug_Ctrl;
extern void GUI_Printf(const char* ,...)__attribute__((__nonnull__(1)));
#else
//#define GUI_Printf(fmt,arg...)			printf("<<DEBUG>>"fmt"\r\n",##arg)	
#endif

void delay_ms(uint32_t nms);
void delay_xms(uint32_t nms);
void delay_us(uint32_t nus);

#ifdef __cplusplus
}
#endif

#endif

