#include "keypad.h"
#include "stm32l476xx.h"


//Initialize functions

unsigned char keypad_scan();
void keypad_pin_init();





// Define functions

void keypad_pin_init(void) {
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN; // Port C clock enable
	GPIOC->MODER &= ~(0x03f003ff);	//clear out bits
	GPIOC->MODER |= 0x00000055;			//set 0,1,2,3 as digital out and 4, 10, 11, 12 as digital in
}



unsigned char keypad_scan(void){
	
	
	unsigned char row, col, colpressed;
	unsigned char key = 0xff;
	unsigned char key_map [4][4] ={
		{'1','2','3','A'},
		{'4','5','6','B'},
		{'7','8','9','C'},
		{'*','2','#','D'},
	};
	uint32_t inputmask, outputmask;
	int rows[] = {0, 1, 2, 3};			//port c
	int cols[] = {4, 10, 11, 12};		//port c

	for(col=0; col<4; col++){				//initializing inputmask for cols
		inputmask |= 1<<cols[col];
	}
	for(row=0; row<4; row++){				//initializing output mask rows
		outputmask |= 1<<rows[row];
	}
	GPIOC->ODR &= ~outputmask;
	waitms(3);
	if ((GPIOC->IDR & inputmask) == inputmask){
			return 0xff;
	}
	//else loop?
	waitms(3);
	for(col=0; col<4; col++){
		if ((GPIOC->IDR & (1<<cols[col])) == 0){
			colpressed = col;
		}	
	}
	for(row=0; row<4; row++){
		GPIOC->ODR |= outputmask;
		GPIOC->ODR &= ~(1<<rows[row]);
		waitms(1);
		if ((GPIOC->IDR & (1<<cols[colpressed])) == 0){
			key = key_map[row][colpressed];
			GPIOC->ODR |= (1<<rows[r]);
			break;
		}
		GPIOC->ODR |= (1<<rows[r]);
	}
	GPIOC->ODR &= ~(outputmask);
	return key;
}
