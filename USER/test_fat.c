#include "main.h"

#ifdef TEST_FAT_ENABLE


#include "ff_sddisk.h"
#include "ff_stdio.h"
#define mainSD_CARD_DISK_NAME            "/"
#define mainSD_CARD_TESTING_DIRECTORY    "/fattest"

#define fsRAM_BUFFER_SIZE             200
#define fsPUTC_FILE_SIZE                100

/* Names of directories that are created. */
static const char *pcDirectory1 = "SUB1", *pcDirectory2 = "SUB2", *pcFullPath = "/SUB1/SUB2";

static FF_Disk_t *pxDisk = NULL;
static HAL_SD_CardInfoTypedef *cardinfo;

extern SD_HandleTypeDef xSDHandle;

void Fat_ff_write(const char * pcMountPath);
void Fat_ff_read(const char*);
void Fat_ff_putc(const char*);
void Fat_ff_getc(const char*);
void Fat_Init(void)
{
    while((pxDisk = FF_SDDiskInit(mainSD_CARD_DISK_NAME)) == NULL)
    {
        vTaskDelay(1000);
    }    
    
    //SDCard_Show_Info();
    
    if(pxDisk != NULL)
    {
        ff_mkdir(mainSD_CARD_TESTING_DIRECTORY);
        ff_mkdir("/wy");
        
        Fat_ff_write(mainSD_CARD_TESTING_DIRECTORY);
        Fat_ff_read(mainSD_CARD_TESTING_DIRECTORY);
        Fat_ff_putc(mainSD_CARD_TESTING_DIRECTORY);
        Fat_ff_getc(mainSD_CARD_TESTING_DIRECTORY);
    }
}

void Fat_ff_write(const char * pcMountPath)
{
    BaseType_t xFileNumber, xWriteNumber;
    const BaseType_t xMaxFiles = 5;
    int32_t lItemsWritten;
    int32_t lResult;
    FF_FILE *pxFile;
    char *pcRAMBuffer, *pcFileName;

    /* Allocate buffers used to hold date written to/from the disk, and the
    file names. */
    pcRAMBuffer = ( char * ) pvPortMalloc( fsRAM_BUFFER_SIZE );
    pcFileName = ( char * ) pvPortMalloc( ffconfigMAX_FILENAME );
    configASSERT( pcRAMBuffer );
    configASSERT( pcFileName );

    /* Ensure in the root of the mount being used. */
    lResult = ff_chdir( pcMountPath );
    configASSERT( lResult >= 0 );

    /* Create xMaxFiles files.  Each created file will be
    ( xFileNumber * fsRAM_BUFFER_SIZE ) bytes in length, and filled
    with a different repeating character. */
    for( xFileNumber = 1; xFileNumber <= xMaxFiles; xFileNumber++ )
    {
        /* Generate a file name. */
        snprintf( pcFileName, ffconfigMAX_FILENAME, "root%03d.txt", ( int ) xFileNumber );

        /* Obtain the current working directory and print out the file name and
        the    directory into which the file is being written. */
        ff_getcwd( pcRAMBuffer, fsRAM_BUFFER_SIZE );
        FF_PRINTF( "Creating file %s in %s\r\n", pcFileName, pcRAMBuffer );

        /* Open the file, creating the file if it does not already exist. */
        pxFile = ff_fopen( pcFileName, "w" );
        configASSERT( pxFile );

        /* Fill the RAM buffer with data that will be written to the file.  This
        is just a repeating ascii character that indicates the file number. */
        memset( pcRAMBuffer, ( int ) ( '0' + xFileNumber ), fsRAM_BUFFER_SIZE );

        /* Write the RAM buffer to the opened file a number of times.  The
        number of times the RAM buffer is written to the file depends on the
        file number, so the length of each created file will be different. */
        for( xWriteNumber = 0; xWriteNumber < xFileNumber; xWriteNumber++ )
        {
            lItemsWritten = ff_fwrite( pcRAMBuffer, fsRAM_BUFFER_SIZE, 1, pxFile );
            configASSERT( lItemsWritten == 1 );
        }

        /* Close the file so another file can be created. */
        ff_fclose( pxFile );
    }

    vPortFree( pcRAMBuffer );
    vPortFree( pcFileName );
}
void Fat_ff_read(const char * pcMountPath)
{
    BaseType_t xFileNumber, xReadNumber;
    const BaseType_t xMaxFiles = 5;
    size_t xItemsRead, xChar;
    FF_FILE *pxFile;
    char *pcRAMBuffer, *pcFileName;

    /* Allocate buffers used to hold date written to/from the disk, and the
    file names. */
    pcRAMBuffer = ( char * ) pvPortMalloc( fsRAM_BUFFER_SIZE );
    pcFileName = ( char * ) pvPortMalloc( ffconfigMAX_FILENAME );
    configASSERT( pcRAMBuffer );
    configASSERT( pcFileName );

    /* Read back the files that were created by
    prvCreateDemoFilesUsing_ff_fwrite(). */
    for( xFileNumber = 1; xFileNumber <= xMaxFiles; xFileNumber++ )
    {
        /* Generate the file name. */
        snprintf( pcFileName, ffconfigMAX_FILENAME, "root%03d.txt", ( int ) xFileNumber );

        /* Obtain the current working directory and print out the file name and
        the    directory from which the file is being read. */
        ff_getcwd( pcRAMBuffer, fsRAM_BUFFER_SIZE );
        FF_PRINTF( "Reading file %s from %s\r\n", pcFileName, pcRAMBuffer );

        /* Open the file for reading. */
        pxFile = ff_fopen( pcFileName, "r" );
        configASSERT( pxFile );

        /* Read the file into the RAM buffer, checking the file contents are as
        expected.  The size of the file depends on the file number. */
        for( xReadNumber = 0; xReadNumber < xFileNumber; xReadNumber++ )
        {
            /* Start with the RAM buffer clear. */
            memset( pcRAMBuffer, 0x00, fsRAM_BUFFER_SIZE );

            xItemsRead = ff_fread( pcRAMBuffer, fsRAM_BUFFER_SIZE, 1, pxFile );
            configASSERT( xItemsRead == 1 );

            /* Check the RAM buffer is filled with the expected data.  Each
            file contains a different repeating ascii character that indicates
            the number of the file. */
            for( xChar = 0; xChar < fsRAM_BUFFER_SIZE; xChar++ )
            {
                configASSERT( pcRAMBuffer[ xChar ] == ( '0' + ( char ) xFileNumber ) );
            }
        }

        /* Close the file. */
        ff_fclose( pxFile );
    }

    vPortFree( pcRAMBuffer );
    vPortFree( pcFileName );

    /*_RB_ also test what happens when attempting to read using too large item
    sizes, etc. */
}
void Fat_ff_putc(const char * pcMountPath)
{
    int32_t iReturn, iByte, iReturned;
    FF_FILE *pxFile;
    char *pcRAMBuffer, *pcFileName;

    /* Allocate buffers used to hold date written to/from the disk, and the
    file names. */
    pcRAMBuffer = ( char * ) pvPortMalloc( fsRAM_BUFFER_SIZE );
    pcFileName = ( char * ) pvPortMalloc( ffconfigMAX_FILENAME );
    configASSERT( pcRAMBuffer );
    configASSERT( pcFileName );

    /* Obtain and print out the working directory. */
    ff_getcwd( pcRAMBuffer, fsRAM_BUFFER_SIZE );
    FF_PRINTF( "In directory %s\r\n", pcRAMBuffer );

    /* Create a sub directory. */
    iReturn = ff_mkdir( pcDirectory1 );
    configASSERT( iReturn == pdFREERTOS_ERRNO_NONE );

    /* Move into the created sub-directory. */
    iReturn = ff_chdir( pcDirectory1 );
    configASSERT( iReturn == pdFREERTOS_ERRNO_NONE );

    /* Obtain and print out the working directory. */
    ff_getcwd( pcRAMBuffer, fsRAM_BUFFER_SIZE );
    FF_PRINTF( "In directory %s\r\n", pcRAMBuffer );

    /* Create a subdirectory in the new directory. */
    iReturn = ff_mkdir( pcDirectory2 );
    configASSERT( iReturn == pdFREERTOS_ERRNO_NONE );

    /* Move into the directory just created - now two directories down from
    the root. */
    iReturn = ff_chdir( pcDirectory2 );
    configASSERT( iReturn == pdFREERTOS_ERRNO_NONE );

    /* Obtain and print out the working directory. */
    ff_getcwd( pcRAMBuffer, fsRAM_BUFFER_SIZE );
    FF_PRINTF( "In directory %s\r\n", pcRAMBuffer );
    snprintf( pcFileName, ffconfigMAX_FILENAME, "%s%s", pcMountPath, pcFullPath );
    configASSERT( strcmp( pcRAMBuffer, pcFileName ) == 0 );

    /* Generate the file name. */
    snprintf( pcFileName, ffconfigMAX_FILENAME, "%s.txt", pcDirectory2 );

    /* Print out the file name and the directory into which the file is being
    written. */
    FF_PRINTF( "Writing file %s in %s\r\n", pcFileName, pcRAMBuffer );

    pxFile = ff_fopen( pcFileName, "w" );
    configASSERT( pxFile );

    /* Create a file 1 byte at a time.  The file is filled with incrementing
    ascii characters starting from '0'. */
    for( iByte = 0; iByte < fsPUTC_FILE_SIZE; iByte++ )
    {
        iReturned = ff_fputc( ( ( int ) '0' + iByte ), pxFile );
        configASSERT( iReturned ==  ( ( int ) '0' + iByte ) );
    }

    /* Finished so close the file. */
    ff_fclose( pxFile );

    /* Move back to the root directory. */
    iReturned = ff_chdir( "../.." );
    configASSERT( iReturn == pdFREERTOS_ERRNO_NONE );

    /* Obtain and print out the working directory. */
    ff_getcwd( pcRAMBuffer, fsRAM_BUFFER_SIZE );
    FF_PRINTF( "Back in root directory %s\r\n", pcRAMBuffer );
    configASSERT( strcmp( pcRAMBuffer, pcMountPath ) == 0 );

    vPortFree( pcRAMBuffer );
    vPortFree( pcFileName );
}
void Fat_ff_getc(const char * pcMountPath)
{
    int iByte, iReturned;
    FF_FILE *pxFile;
    char *pcRAMBuffer, *pcFileName;

    /* Allocate buffers used to hold date written to/from the disk, and the
    file names. */
    pcRAMBuffer = ( char * ) pvPortMalloc( fsRAM_BUFFER_SIZE );
    pcFileName = ( char * ) pvPortMalloc( ffconfigMAX_FILENAME );
    configASSERT( pcRAMBuffer );
    configASSERT( pcFileName );

    /* Move into the directory in which the file was created. */
    snprintf( pcFileName, ffconfigMAX_FILENAME, "%s%s", pcMountPath, pcFullPath );
    iReturned = ff_chdir( pcFileName );
    configASSERT( iReturned == pdFREERTOS_ERRNO_NONE );

    /* Obtain and print out the working directory. */
    ff_getcwd( pcRAMBuffer, fsRAM_BUFFER_SIZE );
    FF_PRINTF( "Back in directory %s\r\n", pcRAMBuffer );
    configASSERT( strcmp( pcRAMBuffer, pcFileName ) == 0 );

    /* pcFileName is about to be overwritten - take a copy. */
    strcpy( pcRAMBuffer, pcFileName );

    /* Generate the file name. */
    sprintf( pcFileName, "%s.txt", pcDirectory2 );

    /* Print out the file name and the directory from which the file is being
    read. */
    FF_PRINTF( "Reading file %s in %s\r\n", pcFileName, pcRAMBuffer );

    /* This time the file is opened for reading. */
    pxFile = ff_fopen( pcFileName, "r" );

    /* Read the file 1 byte at a time. */
    for( iByte = 0; iByte < fsPUTC_FILE_SIZE; iByte++ )
    {
        iReturned = ff_fgetc( pxFile );
        configASSERT( iReturned ==  ( ( int ) '0' + iByte ) );
    }

    /* Should not be able to read another bytes. */
    iReturned = ff_fgetc( pxFile );
    configASSERT( iReturned ==  FF_EOF );

    /* Finished so close the file. */
    ff_fclose( pxFile );

    /* Move back to the root directory. */
    iReturned = ff_chdir( "../.." );

    /* Obtain and print out the working directory. */
    ff_getcwd( pcRAMBuffer, fsRAM_BUFFER_SIZE );
    FF_PRINTF( "Back in root directory %s\r\n", pcRAMBuffer );

    vPortFree( pcRAMBuffer );
    vPortFree( pcFileName );
}
//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		1024  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define LED0_TASK_PRIO		2
//任务堆栈大小	
#define LED0_STK_SIZE 		512  
//任务句柄
TaskHandle_t LED0Task_Handler;
//任务函数
void led0_task(void *pvParameters);

//任务优先级
#define LED1_TASK_PRIO		3
//任务堆栈大小	
#define LED1_STK_SIZE 		50  
//任务句柄
TaskHandle_t LED1Task_Handler;
//任务函数
void led1_task(void *pvParameters);

//任务优先级
#define FLOAT_TASK_PRIO		4
//任务堆栈大小	
#define FLOAT_STK_SIZE 		128
//任务句柄
TaskHandle_t FLOATTask_Handler;
//任务函数
void float_task(void *pvParameters);


extern void APP_Task_Start(void* p_rag);
int main(void){
	HAL_Init();		//进行了必要组件的初始化
	HW_LED_Init();
	HW_KEY_Init();
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
     if(pxDisk != NULL)
    {
        ff_mkdir(mainSD_CARD_TESTING_DIRECTORY);
        ff_mkdir("/wy");
        
        Fat_ff_write(mainSD_CARD_TESTING_DIRECTORY);
        Fat_ff_read(mainSD_CARD_TESTING_DIRECTORY);
        Fat_ff_putc(mainSD_CARD_TESTING_DIRECTORY);
        Fat_ff_getc(mainSD_CARD_TESTING_DIRECTORY);
    }
}
//开始任务任务函数
void start_task(void *pvParameters)
{
	Fat_Init();
    //创建LED0任务
    xTaskCreate((TaskFunction_t )led0_task,     	
                (const char*    )"led0_task",   	
                (uint16_t       )LED0_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )LED0_TASK_PRIO,	
                (TaskHandle_t*  )&LED0Task_Handler);   
    //创建LED1任务
    xTaskCreate((TaskFunction_t )led1_task,     
                (const char*    )"led1_task",   
                (uint16_t       )LED1_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )LED1_TASK_PRIO,
                (TaskHandle_t*  )&LED1Task_Handler);        
    //浮点测试任务
    xTaskCreate((TaskFunction_t )float_task,     
                (const char*    )"float_task",   
                (uint16_t       )FLOAT_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )FLOAT_TASK_PRIO,
                (TaskHandle_t*  )&FLOATTask_Handler);  
	vTaskDelete(StartTask_Handler); //删除开始任务
}
char test_c='0';
//LED0任务函数 
void led0_task(void *pvParameters)
{
    FF_FILE* handle;
    char* buf;
    while(1)
    {
        switch(key_scan()){
            case No_Press:
                break;
            case Once_Press:
                // if(ff_mkdir("/my")!=0){
                //     printf("目录创建成功!\n");
                // }
                // else{
                //     printf("目录创建失败！\n");
                //     break;
                // }
                handle=ff_fopen("/my/test.txt","a+");
                if(handle==NULL){
                    printf("文件打开失败!\n");
                    break;
                }
                else{
                    printf("文件打开成功!\n");
                }
                
                ff_fwrite(&test_c,sizeof(test_c),1,handle);
                buf=OS_Malloc(ff_filelength(handle));
                
                ff_fread(buf,ff_filelength(handle),1,handle);
                for(int j=0;j<ff_filelength(handle)/sizeof(buf[0]);j++){
                    printf(" %c",buf[j]);
                    if(j%10==0)
                        printf("\n");
                }
				OS_Free(buf);
                ff_fclose(handle);
				if(test_c<='9')
                    test_c++;
                else{
                    test_c='0';
                }
                break;
            case Double_Press:
                if(ff_remove("/my/test.txt")==0)
                    printf("文件删除成功！\n");
                else{
                    printf("文件删除失败!\n");
                }
                
                
                break;
            default:
                break;
        }
		delay_ms(20);
    }
}   

//LED1任务函数
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

//浮点测试任务
void float_task(void *pvParameters)
{
	static float float_num=0.00;
	while(1)
	{
		float_num+=0.01f;
//		printf("float_num的值为: %.4f\r\n",float_num);
        vTaskDelay(1000);
	}
}


#endif
