#include "stm32l476xx.h"
#include "SysClock.h"

// PA.5  <--> Green LED
// PC.13 <--> Blue user button
#define LED_PIN    5
#define BUTTON_PIN 13

int main(void){

	System_Clock_Init(); // Switch System Clock = 80 MHz
	
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;	// Enable the clock of Port A
	
	GPIOA->MODER &= ~3U << 6;
  // GPIO Push-Pull: No pull-up, pull-down (00), 
  //  Pull-up (01), Pull-down (10), Reserved (11)
  GPIOA->PUPDR &= ~3U << 6;
  GPIOA->PUPDR |= 2U << 6;    // Pull down
	
  NVIC_EnableIRQ(EXTI3_IRQn); // Enable Interrupt
	
  // Connect External Line to the GPI
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
  SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI3; 
  SYSCFG->EXTICR[0] |=  SYSCFG_EXTICR1_EXTI3_PA; 
	
  // Interrupt Mask Register
  // 0 = marked, 1 = not masked (enabled)
  EXTI->IMR1  |= EXTI_IMR1_IM3;     
	
  // Rising trigger selection
  // 0 = trigger disabled, 1 = trigger enabled
  EXTI->RTSR1 |= EXTI_RTSR1_RT3;  
	//use a volatile for you interrupt var, other wise compiler will shit itself

	while(1);
}
