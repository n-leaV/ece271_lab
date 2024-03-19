#include "stm32l476xx.h"
#include "SysClock.h"

// PA.5  <--> Green LED
// PC.13 <--> Blue user button
#define LED_PIN    5

void led_init(void);
void Delay(uint32_t nTime);
void SysTick_Initialize (uint32_t ticks);

int main(void){

	System_Clock_Init(); // Switch System Clock = 80 MHz
	
	led_init();
	
	SysTick_Initialize (7999);
	
	while(1) {
	Delay(1000);
	GPIOA->ODR ^= (1<<(LED_PIN));								//toggle led
	}
		
	
	while(1);
}

void SysTick_Initialize (uint32_t ticks) {

	SysTick->CTRL = 0;						//disable Systick IRQ & counter
	
	SysTick->LOAD = ticks - 1;		//Sets the value of reload register
	
	NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);		//Set interrupt priority of systick
	
	SysTick->VAL = 0;			//reset systick counter value
	
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;		//select the processors clocl as systicks
	
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;			//enable systick exception request
	
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;				//enable systick timer
	
}

volatile int timedelay;

void SysTick_Handler (void) {
	if (timedelay > 0)			//decrement time delay
		timedelay --;					//decrement
}

void Delay(uint32_t nTime) {
	timedelay = nTime;			
	while(timedelay != 0);			//busy loop
}
		


void led_init(void){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;	// Enable the clock of Port A
	GPIOA->MODER &= ~(3<<(2*LED_PIN)); // Clear mode bits for pin 5
	GPIOA->MODER |= (1<<(2*LED_PIN)); // Set the mode bits to 01
	GPIOA->OTYPER &= ~(1<<(LED_PIN)); // Clear bit 5
	GPIOA->OTYPER |= (0);			// Set OTYPER to push pull
	GPIOA->PUPDR &= ~(3<<(2*LED_PIN));	//Clear bits 11 and 10 and set PUPDR to 0
	GPIOA->PUPDR |= (0);			//set PUPDR to no pullup/nopulldown
}
