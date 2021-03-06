#include "stm32f10x.h"
////////////////Pin Config//////////////////////
#define	LCD_CS_SET  GPIOA->BSRR |= GPIO_BSRR_BS9
#define	LCD_RS_SET	GPIOA->BSRR|= GPIO_BSRR_BS10       
#define	LCD_WR_SET	GPIOA->BSRR|= GPIO_BSRR_BS11    
#define	LCD_RD_SET	GPIOA->BSRR|= GPIO_BSRR_BS12    
#define	LCD_RST_SET	GPIOA->BSRR|= GPIO_BSRR_BS8     
								    
#define	LCD_CS_CLR  GPIOA->BSRR|= GPIO_BSRR_BR9       
#define	LCD_RS_CLR	GPIOA->BSRR|= GPIO_BSRR_BR10        
#define	LCD_WR_CLR	GPIOA->BSRR|= GPIO_BSRR_BR11       
#define	LCD_RD_CLR	GPIOA->BSRR|= GPIO_BSRR_BR12   
#define	LCD_RST_CLR	GPIOA->BSRR|= GPIO_BSRR_BR8      
////////////////////////////////////////////////

///////////////565RGB Colors////////////////////
#define WHITE       0xFFFF
#define BLACK      	0x0000	  
#define BLUE       	0x001F  
#define BRED        0XF81F
#define GRED 			 	0XFFE0
#define GBLUE			 	0X07FF
#define RED         0xF800
#define MAGENTA     0xF81F
#define GREEN       0x07E0
#define CYAN        0x7FFF
#define YELLOW      0xFFE0
#define BROWN 			0XBC40 
#define BRRED 			0XFC07 
#define GRAY  			0X8430

#define DARKBLUE      	 0X01CF	
#define LIGHTBLUE      	 0X7D7C	 
#define GRAYBLUE       	 0X5458 

 
#define LIGHTGREEN     	0X841F 
#define LIGHTGRAY     0XEF5B
#define LGRAY 	0XC618 

#define LGRAYBLUE      	0XA651 
#define LBBLUE          0X2B12 
	    															  
#define POINT_COLOR  0xFFFF
#define BACK_COLOR  0x0000 
	 
////////////////////////////////////////////////

///////////Register for comunication////////////	 
#define wramcmd  0x2C //SRAM(data) -Register  (Nx Bytes)
#define setxcmd  0x2A //Xstart-Xend  - Register (Need 2Bytes)
#define setycmd  0x2B //Ystart-Yend  - Register (Need 2Bytes)

////////////////Function declaret////////

////////////////DELAY//////////////////

void ms(uint32_t time) ;
	
//////////////////////////////////////


/////////LCD-Konfiguracija////////////

void LCD_Init(void);
void LCD_GPIOInit(void);
void LCD_RESET(void);
void LCD_SetParam(void);
void LCD_WriteRAM_Prepare(void);


/////////LCD-8bit Interface///////////

void LCD_WR_REG(uint8_t data);
void LCD_WR_DATA(uint8_t data);//2Byte = (2ciclye->1Byte)


////////Standart graphic comand//////

void LCD_SetWindows(uint16_t xStar, uint16_t yStar, uint16_t xEnd, uint16_t yEnd);
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos);
void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color);	
void LCD_Clear(uint16_t Color);
	
/////////////////GPU/////////////////
void LCD_numbers(uint8_t digit, uint16_t color, uint16_t posx);
void LCD_print(char *buf, uint16_t color);

////////////////////////////////////////

////////////////DELAY//////////////////
void ms(uint32_t time) {
	
	uint32_t i;
	
	for (i = 0; i < time * 2000; i++) {;}	
}
//////////////////////////////////////


//////////////Data for Display//////////////////
uint8_t numbers[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x07, 0x80, 0x00, 0x00, 0x38, 0x70, 0x00, 0x00, 0x60, 0x18, 0x00, 0x00, 0xC0, 0x0C, 0x00,
	0x00, 0x80, 0x0C, 0x00, 0x01, 0x80, 0x06, 0x00, 0x01, 0x00, 0x06, 0x00, 0x03, 0x00, 0x02, 0x00,
	0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00,
	0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00,
	0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x02, 0x00,
	0x01, 0x80, 0x06, 0x00, 0x01, 0x80, 0x06, 0x00, 0x00, 0xC0, 0x0C, 0x00, 0x00, 0xC0, 0x18, 0x00,
	0x00, 0x70, 0x30, 0x00, 0x00, 0x1F, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00,
	0x00, 0x3C, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0xC4, 0x00, 0x00, 0x03, 0x84, 0x00, 0x00,
	0x06, 0x04, 0x00, 0x00, 0x0C, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
	0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
	0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
	0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
	0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
	0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x80, 0x00, 0x00, 0x30, 0xE0, 0x00,
	0x00, 0xE0, 0x30, 0x00, 0x00, 0x80, 0x18, 0x00, 0x01, 0x80, 0x0C, 0x00, 0x01, 0x00, 0x0C, 0x00,
	0x03, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x0C, 0x00,
	0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x60, 0x00,
	0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x03, 0x00, 0x00,
	0x00, 0x06, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00,
	0x00, 0x60, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x03, 0xFF, 0xFE, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x80, 0x00, 0x00, 0x70, 0xE0, 0x00, 0x00, 0xC0, 0x38, 0x00,
	0x01, 0x80, 0x18, 0x00, 0x01, 0x80, 0x0C, 0x00, 0x03, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x0C, 0x00,
	0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x18, 0x00,
	0x00, 0x00, 0x70, 0x00, 0x00, 0x1F, 0xC0, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x18, 0x00,
	0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x06, 0x00,
	0x00, 0x00, 0x06, 0x00, 0x02, 0x00, 0x06, 0x00, 0x03, 0x00, 0x06, 0x00, 0x03, 0x00, 0x0C, 0x00,
	0x01, 0x80, 0x0C, 0x00, 0x00, 0xC0, 0x38, 0x00, 0x00, 0x3F, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0xF0, 0x00,
	0x00, 0x01, 0xB0, 0x00, 0x00, 0x03, 0x30, 0x00, 0x00, 0x03, 0x30, 0x00, 0x00, 0x06, 0x30, 0x00,
	0x00, 0x0C, 0x30, 0x00, 0x00, 0x1C, 0x30, 0x00, 0x00, 0x18, 0x30, 0x00, 0x00, 0x30, 0x30, 0x00,
	0x00, 0x60, 0x30, 0x00, 0x00, 0x60, 0x30, 0x00, 0x00, 0xC0, 0x30, 0x00, 0x01, 0x80, 0x30, 0x00,
	0x01, 0x00, 0x30, 0x00, 0x03, 0x00, 0x30, 0x00, 0x07, 0xFF, 0xFF, 0x80, 0x00, 0x00, 0x30, 0x00,
	0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x30, 0x00,
	0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0x00,
	0x01, 0xC0, 0x00, 0x00, 0x01, 0xC0, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00,
	0x01, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00,
	0x01, 0x80, 0x00, 0x00, 0x01, 0x9F, 0xF0, 0x00, 0x01, 0xE0, 0x18, 0x00, 0x01, 0xC0, 0x0C, 0x00,
	0x01, 0x80, 0x06, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00,
	0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00,
	0x01, 0x00, 0x06, 0x00, 0x01, 0x80, 0x06, 0x00, 0x00, 0xC0, 0x0C, 0x00, 0x00, 0x60, 0x18, 0x00,
	0x00, 0x3F, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xC0, 0x00, 0x00, 0x18, 0x70, 0x00,
	0x00, 0x60, 0x18, 0x00, 0x00, 0x40, 0x0C, 0x00, 0x00, 0xC0, 0x06, 0x00, 0x01, 0x80, 0x06, 0x00,
	0x01, 0x80, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x07, 0x80, 0x00,
	0x03, 0x18, 0x70, 0x00, 0x03, 0x60, 0x18, 0x00, 0x03, 0xC0, 0x0C, 0x00, 0x03, 0x80, 0x06, 0x00,
	0x03, 0x80, 0x02, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00,
	0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x01, 0x00, 0x03, 0x00, 0x01, 0x80, 0x06, 0x00,
	0x00, 0xC0, 0x06, 0x00, 0x00, 0xE0, 0x0C, 0x00, 0x00, 0x70, 0x18, 0x00, 0x00, 0x1F, 0xE0, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x03, 0xFF, 0xFE, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x0C, 0x00,
	0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x30, 0x00,
	0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0xC0, 0x00,
	0x00, 0x00, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x03, 0x00, 0x00,
	0x00, 0x02, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00,
	0x00, 0x08, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00,
	0x00, 0x20, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x0F, 0xC0, 0x00, 0x00, 0x38, 0x70, 0x00, 0x00, 0xE0, 0x18, 0x00, 0x00, 0xC0, 0x0C, 0x00,
	0x01, 0x80, 0x06, 0x00, 0x01, 0x00, 0x06, 0x00, 0x01, 0x00, 0x06, 0x00, 0x01, 0x80, 0x06, 0x00,
	0x01, 0x80, 0x06, 0x00, 0x01, 0x80, 0x04, 0x00, 0x00, 0xC0, 0x0C, 0x00, 0x00, 0x70, 0x30, 0x00,
	0x00, 0x1F, 0xE0, 0x00, 0x00, 0x70, 0x38, 0x00, 0x00, 0xC0, 0x0C, 0x00, 0x01, 0x80, 0x06, 0x00,
	0x03, 0x00, 0x02, 0x00, 0x03, 0x00, 0x03, 0x00, 0x02, 0x00, 0x03, 0x00, 0x02, 0x00, 0x03, 0x00,
	0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x01, 0x80, 0x06, 0x00, 0x01, 0xC0, 0x0C, 0x00,
	0x00, 0xE0, 0x18, 0x00, 0x00, 0x3F, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x80, 0x00,
	0x00, 0x70, 0xE0, 0x00, 0x00, 0xC0, 0x30, 0x00, 0x01, 0x80, 0x18, 0x00, 0x01, 0x00, 0x0C, 0x00,
	0x03, 0x00, 0x06, 0x00, 0x02, 0x00, 0x06, 0x00, 0x02, 0x00, 0x06, 0x00, 0x06, 0x00, 0x06, 0x00,
	0x06, 0x00, 0x07, 0x00, 0x02, 0x00, 0x07, 0x00, 0x03, 0x00, 0x07, 0x00, 0x03, 0x00, 0x07, 0x00,
	0x01, 0x80, 0x0F, 0x00, 0x01, 0xC0, 0x1B, 0x00, 0x00, 0x60, 0x33, 0x00, 0x00, 0x3F, 0xC3, 0x00,
	0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x06, 0x00, 0x03, 0x00, 0x04, 0x00,
	0x03, 0x00, 0x0C, 0x00, 0x01, 0x80, 0x08, 0x00, 0x01, 0x80, 0x18, 0x00, 0x00, 0xE0, 0x30, 0x00,
	0x00, 0x3F, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	};//0-9digital
////////////////////////////////////////////////

/////////////Functions for comunication////////
void LCD_WR_REG(uint8_t data)
{ 
	LCD_RS_CLR;  
	LCD_CS_CLR; 
	GPIOB->ODR = (data << 8); 
	LCD_WR_CLR; 
	LCD_WR_SET; 
	LCD_CS_SET; 	
}


void LCD_WR_DATA(uint8_t data)
{ 
	LCD_RS_SET;
	LCD_CS_CLR; 
	GPIOB->ODR = (data << 8);\
	LCD_WR_CLR;\
	LCD_WR_SET;\
	LCD_CS_SET; 

}


void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue)
{	
	LCD_WR_REG(LCD_Reg);  
	LCD_WR_DATA(LCD_RegValue);	    		 
}	   
	 

void LCD_WriteRAM_Prepare(void)
{
	LCD_WR_REG(wramcmd);
}	 
void LCD_RESET(void)
{
	LCD_RST_CLR;
	ms(100);	
	LCD_RST_SET;
	ms(50);
}
////////////////////////////////////////////////

/////////Function Display Init//////////////////

void LCD_GPIOInit(void)
{
	
	RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_AFIOEN);// GPIOA GPIOB Altentativi funkcija enable
	GPIOC->CRH |= 0x00300000; 
	
	
	GPIOA->CRH = 0x00033333; //GPIOA 8RST 9CS 10RS 11WR 12RD
	GPIOB->CRH = 0x33333333;
	
	GPIOA->ODR = 0x1F00;//8-12 
	GPIOB->ODR = 0xFF00;//8-15 8bit DATA Bus
	
}

void LCD_SetParam(void)
{ 
		
	LCD_WriteReg(0x36, 0x0A);//0x28 -> 0x0A
	
}	
void LCD_Init(void)
{  
					 
	LCD_GPIOInit();
	LCD_RESET();

		//************* Ekrano inicilizacija **********//		
	LCD_WR_REG(0x11); //Sleep OUT (11h) 
	
	ms(20);
	////////////////////////////
	LCD_WR_REG(0xD0);	LCD_WR_DATA(0x07);	LCD_WR_DATA(0x42);	LCD_WR_DATA(0x18);
	////////////////////////////
	LCD_WR_REG(0xD1);	LCD_WR_DATA(0x55);	LCD_WR_DATA(0xAA);	LCD_WR_DATA(0x66);
	///////////////////////////
	LCD_WR_REG(0xD2);	LCD_WR_DATA(0x01);	LCD_WR_DATA(0x02);
	///////////////////////////
	LCD_WR_REG(0xC0);	LCD_WR_DATA(0x10);	LCD_WR_DATA(0x3B);	LCD_WR_DATA(0x00);	LCD_WR_DATA(0x02);	LCD_WR_DATA(0x11);
	///////////////////////////
	LCD_WR_REG(0xC5);	LCD_WR_DATA(0x03);
	///////////////////////////
	LCD_WR_REG(0xC8);	LCD_WR_DATA(0x00);	LCD_WR_DATA(0x32);	LCD_WR_DATA(0x36);	LCD_WR_DATA(0x45);	LCD_WR_DATA(0x06);	LCD_WR_DATA(0x16);	LCD_WR_DATA(0x37);	LCD_WR_DATA(0x75); 
	LCD_WR_DATA(0x77);	LCD_WR_DATA(0x54);	LCD_WR_DATA(0x0C);	LCD_WR_DATA(0x00);
	///////////////////////////
	LCD_WR_REG(0x36);	LCD_WR_DATA(0x28);
	///////////////////////////
	LCD_WR_REG(0x3A);	LCD_WR_DATA(0x55);
	///////////////////////////
	LCD_WR_REG(0xB6);	LCD_WR_DATA(0x02);
	///////////////////////////
	LCD_WR_REG(0x2A);	LCD_WR_DATA(0x00);	LCD_WR_DATA(0x00);	LCD_WR_DATA(0x01);	LCD_WR_DATA(0x3f);//3f
	///////////////////////////
	LCD_WR_REG(0x2B);	LCD_WR_DATA(0x00);	LCD_WR_DATA(0x00);	LCD_WR_DATA(0x01);	LCD_WR_DATA(0xe0);//e0
	
	ms(120);
	
	
	LCD_WR_REG(0x29);// Display ON (29h) 
	///////////////////////////
	LCD_WR_REG(0x2c);//Memory Write (2Ch) 

	LCD_SetParam(); 
	LCD_WR_REG(0x36); LCD_WR_DATA(0x28);
}

////////////////////////////////////////////////


////////Standart graphic comand/////////////////


void LCD_SetWindows(uint16_t xStar, uint16_t yStar, uint16_t xEnd, uint16_t yEnd)
{	
	LCD_WR_REG(setxcmd);	
	LCD_RS_SET;\
	LCD_CS_CLR;\
		
	GPIOB->ODR = (xStar);\
	LCD_WR_CLR;\
	LCD_WR_SET;\
	GPIOB->ODR = (xStar << 8);\
	LCD_WR_CLR;\
	LCD_WR_SET;\

	GPIOB->ODR = (xEnd);\
	LCD_WR_CLR;\
	LCD_WR_SET;\
	GPIOB->ODR = (xEnd << 8);\
	LCD_WR_CLR;\
	LCD_WR_SET;\

/////////////////////////
	LCD_WR_REG(setycmd);
	LCD_RS_SET;
	LCD_CS_CLR;\

	GPIOB->ODR = (yStar);\
	LCD_WR_CLR;\
	LCD_WR_SET;\
	GPIOB->ODR = (yStar << 8);\
	LCD_WR_CLR;\
	LCD_WR_SET;\
	

	GPIOB->ODR = (yEnd);\
	LCD_WR_CLR;\
	LCD_WR_SET;\
	GPIOB->ODR = (yEnd << 8);\
	LCD_WR_CLR;\
	LCD_WR_SET;\
	LCD_CS_SET;\
	
//*/
	LCD_WriteRAM_Prepare();			
}   

void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{	  	    			
	LCD_SetWindows(Xpos, Ypos, Xpos, Ypos);
}

void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color)
{

	LCD_SetCursor(x, y);
	LCD_RS_SET; 
	LCD_CS_CLR;	  
	
	GPIOB->ODR = (color);	
	LCD_WR_CLR;
	LCD_WR_SET;	
	
	GPIOB->ODR = (color) << 8;	
	LCD_WR_CLR;
	LCD_WR_SET;	 
	
	LCD_CS_SET;
}

void LCD_Clear(uint16_t Color)  
{
	uint32_t index = 0;      
	LCD_SetWindows(0, 0, 480, 320);	
	LCD_RS_SET; 
	LCD_CS_CLR;	   
	for (index = 0; index < 480 * 320; index++)
	{
		GPIOB->ODR = (Color);	
		LCD_WR_CLR;
		LCD_WR_SET;	
		
		GPIOB->ODR = (Color << 8);	
		LCD_WR_CLR;
		LCD_WR_SET;	   	
	}
	LCD_CS_SET;		
} 

////////////////////////////////////////////////


//////////////////////GPU//////////////////////

void LCD_numbers(uint8_t digit, uint16_t color, uint16_t posx)
{
	
	uint16_t index = 0;
	uint8_t i = 0b10000000; // (1<<7)
	uint16_t x = 0;  
	uint16_t y = 0;
	uint16_t adres = digit*(4*43); //4Byte - Vertical  (43 Horizontal line)  

	for (index = 0; index <= 32 * 43; index++) //32*43 Pixels on 1 Char
	{
		if (numbers[adres] & i) 
		{
			LCD_DrawPoint(x+posx , y , color);
		}
		
		x++;
		if (x == 32)//Ending read line
		{
			y++; x = 0;
		}
		i >>= 1;
		if (i == 0b0000000)
		{
			i = 0b10000000; adres++;
		}
	}
	
} 

void LCD_text(char *buf, uint16_t color)
{
	int ii = 0;
	int i;
	int a = 1;
	int c = 28;
	while (a == 1)
	{
		
		
		if (buf[ii] ==0)
		{
			a = 0;
		}
		ii++;
	}
	for (i = 0; i <= ii; i++)
	{
		LCD_numbers(buf[i] - 48, color, c*i);
		//if (c == 20)
		//{
		//	c = 28;
		//}
		//if (buf[i] == 49)
		//{
	//		c = 20;
	//	}
		
	}
}



/////