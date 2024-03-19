#include "stm32l476xx.h"
#include "SysClock.h"

// PA.5  <--> Green LED
// PC.13 <--> Blue user button
#define LED_PIN    5
#define BUTTON_PIN 13

int main(void){

	System_Clock_Init(); // Switch System Clock = 80 MHz
	
	while(1) {
	Delay(1000);
	
	}
		
	
	while(1);
}

void SysTick_Initialize (uint32_t ticks) {

	SysTick->CTRL = 0;						//disable Systick IRQ & counter
	
	SysTick->LOAD = ticks - 1;
	
	NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
	
	SysTick->VAL = 0;
	
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
	
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	
}

volatile int timedelay;

void SysTick_Handler (void) {
	if (timedelay > 0)
		timedelay --;
}

void Delay(uint32_t nTime) {
	timedelay = nTime;
	while(timedelay != 0);
}
		



