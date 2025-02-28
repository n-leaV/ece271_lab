#include <string.h>
#include <stdio.h>
#include "keypad.h"
#include "stm32l476xx.h"
#include "SysClock.h"
#include "I2C.h"
#include "ssd1306.h"
#include "ssd1306_tests.h"

// Note: The code gets Warning L6989W
// Report: this is an known issue for STM32L4xx.
// https://www.keil.com/support/docs/3853.htm
// We can ignore this warning because we are not using
// external memory.

uint8_t Data_Receive[6];
uint8_t Data_Send[6];


void I2C_GPIO_init(void);
	
int main(void){
	
	keypad_pin_init();
	System_Clock_Init(); // Switch System Clock = 80 MHz
	I2C_GPIO_init();
	I2C_Initialization(I2C1);

	//ssd1306_TestAll();
	ssd1306_Init();
	ssd1306_Fill(White);
	ssd1306_SetCursor(2,0);
//	ssd1306_WriteString(message, Font_11x18, Black);		
	ssd1306_UpdateScreen();	
	
//	while(1){
//		for(i=0; i<100000; i++);
//		sprintf(message, "%d", count);
//		ssd1306_Fill(White);
//		ssd1306_SetCursor(2,0);
//		ssd1306_WriteString(message, Font_11x18, Black);
//		ssd1306_UpdateScreen();	
//		count++;
//	}
	
//			WORKING LAB CODE												//
	int color=Black;
	while(1){
	char key;
	key = keypad_scan();
	if(key=='*'){
		color ^=0x1;
	}
	else{
		ssd1306_WriteChar(key, Font_11x18, color);
	}	
	ssd1306_UpdateScreen();
	} // Deadloop




//char buffer[10] = "";
//int i;
//int count = 0;
//char key;
//while (1){
//	ssd1306_WriteString(buffer, Font_11x18, Black);
//	key=keypad_scan();
//	if(!(key == 0xff) & !(key == '*') &(count<10)){
//		buffer[count] = key;
//		count ++;
//	}	
//	if((key == '*')&&(count>0)){
//		count = count-1;
//		buffer[count] = key;
//		count ++;
//	}
//	ssd1306_UpdateScreen();
//}

}

