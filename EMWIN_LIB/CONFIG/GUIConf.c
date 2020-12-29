#include "GUI.h"
#include "stm32f4xx_sys.h"

					  
////////////////////////////////////////////////////////////////////////////////// 

#define GUI_NUMBYTES  (4*1024*1024)
#define GUI_BLOCKSIZE 0X80  //���С

static U32 aMemory[GUI_NUMBYTES/4]__EXRAM;
//GUI_X_Config
//��ʼ����ʱ�����,��������emwin��ʹ�õ��ڴ�
void GUI_X_Config(void) {
		
        GUI_ALLOC_AssignMemory((void*)aMemory, GUI_NUMBYTES); //Ϊ�洢����ϵͳ����һ���洢��
		GUI_ALLOC_SetAvBlockSize(GUI_BLOCKSIZE); //���ô洢���ƽ���ߴ�,����Խ��,���õĴ洢������Խ��
		GUI_SetDefaultFont(GUI_DEFAULT_FONT); //����Ĭ������
	
}

