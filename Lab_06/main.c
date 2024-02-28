#include "stm32l476xx.h"
#include "SysClock.h"

// PA.5  <--> Green LED
// PC.13 <--> Blue user button
#define LED_PIN    5
#define BUTTON_PIN 13

void led_init(void);
void butt_init(void);


int main(void){

	System_Clock_Init(); // Switch System Clock = 80 MHz
	
	led_init();
	butt_init();
	
  NVIC_EnableIRQ(EXTI3_IRQn); // Enable Interrupt
	
  // Connect External Line to the GPI
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
  SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR1_EXTI3; 
  SYSCFG->EXTICR[3] |=  SYSCFG_EXTICR1_EXTI3_PA; 
	
  // Interrupt Mask Register
  // 0 = marked, 1 = not masked (enabled)
  EXTI->IMR1  |= EXTI_IMR1_IM13;     
	
  // Rising trigger selection
  // 0 = trigger disabled, 1 = trigger enabled
  EXTI->RTSR1 |= EXTI_RTSR1_RT13;  
	//use a volatile for you interrupt var, other wise compiler will shit itself

	while(1);
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

void butt_init(void){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;	//Enable clock for Port C
	
	GPIOC->MODER &= ~(3<<(2*BUTTON_PIN)); // Clear mode bits for pin 5
	GPIOC->MODER |= (0<<(2*BUTTON_PIN)); // Set the mode bits to 00
	
	GPIOC->PUPDR &= ~(3<<(2*BUTTON_PIN));	//Clear bits 26 and 27
	GPIOC->PUPDR |= (0);			//set PUPDR to no pullup/nopulldown
}
