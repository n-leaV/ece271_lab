#include "stm32l476xx.h"
#include "SysClock.h"

// PA.5  <--> Green LED
// PC.13 <--> Blue user button
#define LED_PIN    5
#define BUTTON_PIN 13

int main(void){

	System_Clock_Init(); // Switch System Clock = 80 MHz
	
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;	// Enable the clock of Port A
	
	GPIOA->MODER &= ~(3<<(2*LED_PIN)); // Clear mode bits for pin 5
	GPIOA->MODER |= (1<<(2*LED_PIN)); // Set the mode bits to 01

	GPIOA->OTYPER &= ~(1<<(LED_PIN)); // Clear bit 5
	GPIOA->OTYPER |= (0);			// Set OTYPER to push pull
	
	GPIOA->PUPDR &= ~(3<<(2*LED_PIN));	//Clear bits 11 and 10
	GPIOA->PUPDR |= (0);			//set PUPDR to no pullup/nopulldown

	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;	//Enable clock for Port C
	
	GPIOC->MODER &= ~(3<<(2*BUTTON_PIN)); // Clear mode bits for pin 5
	GPIOC->MODER |= (0<<(2*BUTTON_PIN)); // Set the mode bits to 00
	
	GPIOC->PUPDR &= ~(3<<(2*BUTTON_PIN));	//Clear bits 26 and 27
	GPIOC->PUPDR |= (0);			//set PUPDR to no pullup/nopulldown
	
	GPIOA->ODR &= ~(1<<(LED_PIN));  // clears bit 5 of ODER/set bit 5 to zero
	//GPIOA->ODR |= (1<<(LED_PIN));		// Sets pin 5 of ODER to 1
	
	// Set pin PA.5 as GPIO output with push pull

	// Turn on the LED
	
  // Dead loop & program hangs here
	while(1){
		if(!(GPIOC->IDR & GPIO_IDR_IDR_13)){					//if button pressed
			GPIOA->ODR ^= (1<<(LED_PIN));								//toggle led
			while(!(GPIOC->IDR & GPIO_IDR_IDR_13)) {};	//wait for button to unpress	
			
		};
	};
}
