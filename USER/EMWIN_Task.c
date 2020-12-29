#include "main.h"

void EMWin_MainTask(void){
	GUI_SetColor(GUI_RED);
	GUI_SetBkColor(GUI_BLACK);
	GUI_Clear();
	GUI_SetFont(&GUI_Font16_1);
	GUI_DispString("hello!\n");
	GUI_DispString("test");
}