#include "stm32f4xx_sys.h"
#include <rt_sys.h>
#include <stdarg.h>
#include "ff_headers.h"
#include "ff_stdio.h"

#if defined(__clang__)
__asm(".global __use_no_semihosting\n\t");
#if SYSTEM_SUPPORT_OS==1
__asm(".global __use_no_heap_region\n\t");
#endif

//__asm(".global __use_full_stdio\n\t");

#elif defined(__CC_ARM)
#pragma import(__use_no_semihosting)
#if SYSTEM_SUPPORT_OS==1
#pragma import(__use_no_heap_region)
#endif
#endif
#define STDIN	0
#define STDOUT	1
#define STDERR	2
#define IS_STD(fn)	((fn)>=0&&(fn)<=2)

/*
* These names are used during library initialization as the
* file names opened for stdin, stdout, and stderr.
* As we define _sys_open() to always return the same file handle,
* these can be left as their default values.
*/
const char __stdin_name[] = ":tt";
const char __stdout_name[] = ":tt";
const char __stderr_name[] = ":tt";


FILEHANDLE _sys_open(const char *pcFile, int openmode)
{
	extern const char *prvABSPath( const char *pcPath );

	FF_FILE *pxStream = NULL;
	FF_DirHandler_t xHandler;
	FF_Error_t xError;
	uint8_t ucMode=0;
	
	if(pcFile==__stdin_name)
		return STDIN;
	else if(pcFile==__stdout_name){
		return STDOUT;
	}
	else if(pcFile==__stderr_name)
		return STDERR;
	
	pcFile = prvABSPath( pcFile );
	if( FF_FS_Find( pcFile, &xHandler ) == pdFALSE ){
		return -1;
	}
	if(openmode&OPEN_PLUS)
		ucMode|=FF_MODE_READ|FF_MODE_WRITE;
	if(openmode&OPEN_W){
		ucMode|=FF_MODE_CREATE|FF_MODE_WRITE|FF_MODE_TRUNCATE;
	}
	if(openmode&OPEN_A){
		ucMode|=FF_MODE_APPEND|FF_MODE_WRITE|FF_MODE_CREATE;
	}
	if(openmode&OPEN_R){
		ucMode|=FF_MODE_READ;
	}
	if(openmode&OPEN_B){
		printf("This file openmode is not supported!!\r\n");
		ucMode|=FF_MODE_READ;
	}
	
	pxStream = FF_Open( xHandler.pxManager, xHandler.pcPath, ucMode, &xError );
	if(FF_isERR( xError )==pdTRUE)
		return -1;
	
	return (FILEHANDLE)pxStream; /* everything goes to the same output */
}

int _sys_close(FILEHANDLE fh)
{
	FF_Error_t xError;
	
	if(IS_STD(fh))
		return 0;
	xError=FF_Close((FF_FILE*)fh);
	
	if(FF_isERR( xError )==pdTRUE)
		return -1;
	return 0;
}

int _sys_write(FILEHANDLE fh, const unsigned char *buf,\
	unsigned len, int mode)
{
	extern UART_HandleTypeDef UART1_Handler;
	int32_t iReturned;
	
	if(fh==STDIN)
		return -1;
	
	if(fh==STDOUT||fh==STDERR){
		HAL_UART_Transmit(&UART1_Handler,(uint8_t*)buf,len,1000);
		return 0;
	}
	
	iReturned = FF_Write( (FF_FILE*)fh, 1, len, (uint8_t *)buf );
	if(FF_isERR( iReturned )==pdTRUE)
		return -1;
	
	return len-(uint32_t)iReturned;
}

int _sys_read(FILEHANDLE fh, unsigned char *buf,\
	unsigned len, int mode)
{
	int32_t iReturned;
	int ff_errno;
	
	
	if(IS_STD(fh))
		return -1;
	
	iReturned = FF_Read((FF_FILE*)fh, 1, len, (uint8_t *)buf );
	if(FF_isERR( iReturned )==pdTRUE)
		return -1;
	
	return len-(uint32_t)iReturned;
}

void _ttywrch(int ch)
{
	uint8_t c = ch;
	extern UART_HandleTypeDef UART1_Handler;
	HAL_UART_Transmit(&UART1_Handler,&c,1,1000);
}

int _sys_istty(FILEHANDLE fh)
{
	return IS_STD(fh); /* buffered output */
}

int _sys_seek(FILEHANDLE fh, long pos)
{
	FF_Error_t xError;
	int iReturn, ff_errno;
	extern int prvFFErrorToErrno( FF_Error_t xError );
	if(IS_STD(fh))
		return -1;
	xError = FF_Seek( (FF_FILE*)fh, pos, FF_SEEK_SET );
	
	ff_errno = prvFFErrorToErrno( xError );

	if( ff_errno == 0 )
	{
		iReturn = 0;
	}
	else
	{
		iReturn = -1;
	}

	return iReturn;
}

long _sys_flen(FILEHANDLE fh)
{
	FF_Error_t xReturned;
	uint32_t ulLength;

	if(IS_STD(fh))
		return -1;
	
	xReturned = FF_BytesLeft((FF_FILE*)fh);

	if( FF_isERR( xReturned ) != pdFALSE )
	{
		return -1;
	}
	return xReturned;
}

void _sys_exit(int code){
	printf("This Program unexpected exit! err:%d\r\n",code);
	while(1);
}

int remove(const char *pcPath){
	FF_DirHandler_t xHandler;
	FF_Error_t xError;
	int iReturn, ff_errno;
	extern const char *prvABSPath( const char *pcPath );
	extern int prvFFErrorToErrno( FF_Error_t xError );
	
	if(pcPath==__stdin_name||pcPath==__stdout_name||pcPath==__stderr_name)
		return -1;
	/* In case a CWD is used, get the absolute path */
	pcPath = prvABSPath( pcPath );

	/* Find the i/o manager which can handle this path. */
	if( FF_FS_Find( pcPath, &xHandler ) == pdFALSE )
	{
		/* No such device or address */
		ff_errno = pdFREERTOS_ERRNO_ENXIO;

		/* Return -1 for error as per normal remove() semantics. */
		iReturn = -1;
	}
	else
	{
		xError = FF_RmFile( xHandler.pxManager, xHandler.pcPath );
		ff_errno = prvFFErrorToErrno( xError );

		if( ff_errno == 0 )
		{
			iReturn = 0;
		}
		else
		{
			/* Return -1 for error as per normal remove() semantics. */
			iReturn = -1;
		}
	}

	return iReturn;
}

//不清楚如何实现
int _sys_tmpnam(char * name, int sig, unsigned maxlen){
	
	return 0;
}
//#ifdef USE_MY_PRINTF
//static char print_buf[256] __EXRAM;

//int printf(const char* fmt,...){
//	va_list args;
//	int ret=0;
//	va_start(args,fmt);
//	ret=vsnprintf(print_buf,sizeof(print_buf)/sizeof(print_buf[0]),fmt,args);
//	va_end(args);
//	return ret;
//}
//int puts(const char* s){
//	return printf("%s\n",s);
//}
//#else
////重定义fputc函数 
//int fputc(int ch, FILE *f)
//{ 	
//	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
//	USART1->DR = (uint8_t) ch;      
//	return ch;
//}
//#endif

#if SYSTEM_SUPPORT_OS==1
void *malloc (size_t size){
	return OS_Malloc(size);
}

void free(void* p){
	OS_Free(p);
}

void *realloc(void* p,size_t want){
	return OS_Realloc(p,want);
}

void *calloc(size_t nmemb, size_t size){
	return OS_Calloc(nmemb,size);
}

#endif



