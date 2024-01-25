#include "stm32l476xx.h"
#include "SysClock.h"

// PA.5  <--> Green LED
// PC.13 <--> Blue user button
#define LED_PIN    5
#define BUTTON_PIN 13
void TIM2_CH1_Init() {
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;		//enable timer clock
	
	TIM2->CR1 &= ~TIM_CR1_DIR; //select up counting
	
	TIM2->PSC = 39;
	
	TIM2->ARR = 999;
	
	TIM2->CCR1 = 500;
	
	TIM2->CCMR1 &= ~TIM_CCMR1_OC1M;
	
	TIM2->CCMR1 |= TIM_CCMR1_OC1M | TIM_CCMR1_OC1M_2;
	
	TIM2->CCMR1 |= TIM_CCMR1_OC1PE;	//output 1 preload enable
	
	TIM2->CCER &= ~TIM_CCER_CC1P;		//select output polarity
	
	TIM2->CCER |= TIM_CCER_CC1E;		//enable output channel 1
	
	TIM2->BDTR |= TIM_BDTR_MOE;		//Maine output enable
	
	TIM2->CR1 |= TIM_CR1_CEN;		//Start counter
}


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
	/*while(1){
		if(!(GPIOC->IDR & GPIO_IDR_IDR_13)){					//if button pressed
			GPIOA->ODR ^= (1<<(LED_PIN));								//toggle led
			while(!(GPIOC->IDR & GPIO_IDR_IDR_13)) {};	//wait for button to unpress	
			
		};
	};*/
	
	int i;
	int brightness = 1;
	int stepSize = 1;
	
	TIM2_CH1_Init();
	while(1) {
		if ((brightness >=999) || (brightness <= 0))
			stepSize = -stepSize;
		
		brightness += stepSize;
		TIM2->CCR1 = brightness;
		for(i = 0; i < 1000; i++);
	}
}
