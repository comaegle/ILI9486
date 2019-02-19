#include "stm32f10x.h"
#include "ILI9486.h"





void ALL_Init(void);


void ALL_Init(void)
{
	///////LCD -Start//////////	
	LCD_Init();
	LCD_Clear(BLACK);
	//////LCD - End //////////	
}

