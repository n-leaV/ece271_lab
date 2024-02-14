#include "keypad.h"
#include "stm32l476xx.h"


//Initialize functions

unsigned char keypad_scan(void);
void keypad_pin_init(void);
void waitms(int ms);
char adjust(char key);




// Define functions


void waitms(int ms) {
	if (ms==0) return;
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM7EN;
	TIM7->CR1 &= ~TIM_CR1_CEN;
	TIM7->SR = 0;
	TIM7->CNT = 0;
	TIM7->PSC = 3999;
	TIM7->ARR = 20*ms -1;
	TIM7->RCR = 0;
	TIM7->CR1 |= TIM_CR1_CEN;
	while ( (TIM7->SR & TIM_SR_UIF) == 0);	
}
	


void keypad_pin_init(void) {
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN; // Port C clock enable
	GPIOC->MODER &= ~(0x03f003ff);	//clear out bits
	GPIOC->MODER |= 0x00000055;			//set 0,1,2,3 as digital out and 4, 10, 11, 12 as digital in
	GPIOC->OTYPER &= ~(0x0000000f); // clear bits from 0-3
	GPIOC->OTYPER |= 0x0000000f;
	GPIOC->PUPDR &= ~(0x03f003ff);
	GPIOC->PUPDR |= 0x00000000;
}

char adjust(char key){
	switch (key){
		case '*':
			key ='4';
			break;
		case '2':
			key ='5';
			break;
		case '#':
			key ='6';
			break;
		case '4':
			key ='7';
			break;
		case '5':
			key ='8';
			break;
		case '6':
			key ='9';
			break;
		case '8':
			key ='0';
			break;
		case 'D':
			key ='B';
			break;
		case 'B':
			key ='C';
			break;
		case 'C':
			key ='D';
			break;
		case '7':
			key ='*';
			break;
		case '9':
			key ='#';
			break;
		default:
			return key;
	}
return key;
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
		GPIOC->ODR |= 0xff;
		GPIOC->ODR &= ~(1<<rows[row]);
		waitms(1);
		if ((GPIOC->IDR & (1<<cols[colpressed])) == 0){
			key = key_map[row][colpressed];
			GPIOC->ODR |= (1<<rows[row]);
			break;
		}
		GPIOC->ODR |= (1<<rows[row]);
	}
	GPIOC->ODR &= ~(outputmask);
	return key;
}


