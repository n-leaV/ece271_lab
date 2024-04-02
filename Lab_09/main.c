#include "stm32l476xx.h"
#include "SysClock.h"

// PA.5  <--> Green LED
// PC.13 <--> Blue user button
#define LED_PIN    5
#define BUTTON_PIN 13

void System_Clock_init(void);


int main(void){

	System_Clock_init(); // Switch System Clock = 80 MHz
	
	// Enable the clock of Port A
	
	// Set pin PA.5 as GPIO output with push pull

	// Turn on the LED
	
  // Dead loop & program hangs here
	while(1);
}
void System_Clock_init(void){
	RCC->CR |= RCC_CR_HSION;			//Disable MSION
	
	while((RCC->CR & RCC_CR_HSIRDY) != RCC_CR_HSIRDY);
	
	RCC->CFGR &= RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_1;
	
	
	
}