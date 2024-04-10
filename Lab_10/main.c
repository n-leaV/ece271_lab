#include <string.h>
#include <stdio.h>
#include "stm32l476xx.h"
#include "SysClock.h"
#include "I2C.h"
#include "ssd1306.h"
#include "ssd1306_tests.h"

// PA.5  <--> Green LED
// PC.13 <--> Blue user button
#define LED_PIN    5
#define BUTTON_PIN 13

volatile uint32_t TimeDelay;

void configure_KEYPAD_pin() {
	
	//GPIO Port C already enabled  in configure_MOTOR_BUTTON_pin
	
	// GPIO Mode: Input(00), Output(01), AlterFunc(10), Analog(11, reset)
	//ROW
	GPIOC->MODER &= ~GPIO_MODER_MODE0;  
	GPIOC->MODER |=  GPIO_MODER_MODE0_0;      //  R1 PC0 Output(01)
	
	GPIOC->MODER &= ~GPIO_MODER_MODE1;  
	GPIOC->MODER |=  GPIO_MODER_MODE1_0;      //  R2 PC1
	
	GPIOC->MODER &= ~GPIO_MODER_MODE2;  
	GPIOC->MODER |=  GPIO_MODER_MODE2_0;      //  R3 PC2
	
	GPIOC->MODER &= ~GPIO_MODER_MODE3;  
	GPIOC->MODER |=  GPIO_MODER_MODE3_0;      //  R4 PC3
	
	//COLUMN
	GPIOC->MODER &= ~GPIO_MODER_MODE4;  			//  C1 PC4 Input(00)
	
	GPIOC->MODER &= ~GPIO_MODER_MODE10;				//  C2 PC10
	
	GPIOC->MODER &= ~GPIO_MODER_MODE11;	      //  C3 PC11
	
	GPIOC->MODER &= ~GPIO_MODER_MODE12;				//  C4 PC12
	
	// GPIO Speed: Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
	//ROW
	GPIOC->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED0;  // R1 PC0 Low speed
	
	GPIOC->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED1;  // R2 PC1
	
	GPIOC->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED2;  // R3 PC2
	
	GPIOC->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED3;  // R4 PC3
	
	//COLUMN
	GPIOC->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED4;  // C1 PC4
	
	GPIOC->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED10; // C2 PC10
	
	GPIOC->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED11; // C3 PC11
	
	GPIOC->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED12; // C4 PC12
	
	// GPIO Output Type: Output push-pull (0, reset), Output open drain (1) 
	GPIOC->OTYPER |= GPIO_OTYPER_OT0;      		// R1 PC0 Open Drain
	
	GPIOC->OTYPER |= GPIO_OTYPER_OT1;     	 	// R2 PC1
	
	GPIOC->OTYPER |= GPIO_OTYPER_OT2;     	 	// R3 PC2
	
	GPIOC->OTYPER |= GPIO_OTYPER_OT3;     	 	// R4 PC3
	
/*	GPIOC->OTYPER &= ~GPIO_OTYPER_OT4;      	// C1 PC4
	
	GPIOC->OTYPER &= ~GPIO_OTYPER_OT10;     	// C2 PC10
	
	GPIOC->OTYPER &= ~GPIO_OTYPER_OT11;     	// C3 PC11
	
	GPIOC->OTYPER &= ~GPIO_OTYPER_OT12;     	// C4 PC12*/
	
	// GPIO Push-Pull: No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
	//ROW
	GPIOC->PUPDR  &= ~GPIO_PUPDR_PUPD0;  			// R1 PC0 No pull-up, no pull-down
	
	GPIOC->PUPDR  &= ~GPIO_PUPDR_PUPD1;  			// R2 PC1
	
	GPIOC->PUPDR  &= ~GPIO_PUPDR_PUPD2;  			// R3 PC2
	
	GPIOC->PUPDR  &= ~GPIO_PUPDR_PUPD3;  			// R4 PC3
	
	//COLUMN
	GPIOC->PUPDR  &= ~GPIO_PUPDR_PUPD4;  			// C1 PC4 No pull-up, no pull-down
	
	GPIOC->PUPDR  &= ~GPIO_PUPDR_PUPD10;  			// C2 PC10
	
	GPIOC->PUPDR  &= ~GPIO_PUPDR_PUPD11;				// C3 PC11
	
	GPIOC->PUPDR  &= ~GPIO_PUPDR_PUPD12;  			// C4 PC12
	
}

void SysTick_Initialize(uint32_t ticks) {
	
	//Disable SysTick
	SysTick->CTRL = 0;
	
	//Reload Register
	SysTick->LOAD = ticks - 1;
	
	//Set Interrupt Priority
	NVIC_SetPriority(SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
	
	//Reset counter value
	SysTick->VAL = 0;
	
	//Select Proccessor Clock
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
	
	//Enables SysTick Interrupt
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	
	//Enable SysTick
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	
}

void Delay(uint32_t nTime){
	
	TimeDelay = nTime;
	while(TimeDelay != 0);
	
}

void SysTick_Handler(void){
	
	if(TimeDelay > 0)
		TimeDelay--;
	
}

void configure_LED_pin(){
  // Enable the clock to GPIO Port A and C
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
	
	//LED
	// GPIO Mode: Input(00), Output(01), AlterFunc(10), Analog(11, reset)
	GPIOA->MODER &= ~GPIO_MODER_MODE5;  
	GPIOA->MODER |=  GPIO_MODER_MODE5_1;      // AlterFunc(10)
	
	// GPIO Speed: Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
	GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED5; //Low Speed
	
	// GPIO Output Type: Output push-pull (0, reset), Output open drain (1) 
	GPIOA->OTYPER &= ~GPIO_OTYPER_ODR_5;      // Push-pull
	
	// GPIO Push-Pull: No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
	GPIOA->PUPDR  &= ~GPIO_PUPDR_PUPDR5;  // No pull-up, no pull-down
	
	//Servo
		// GPIO Mode: Input(00), Output(01), AlterFunc(10), Analog(11, reset)
	GPIOA->MODER &= ~GPIO_MODER_MODE0;  
	GPIOA->MODER |=  GPIO_MODER_MODE0_1;      // AlterFunc(10)
	
	// GPIO Speed: Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
	GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED5; //Low Speed
	
	// GPIO Output Type: Output push-pull (0, reset), Output open drain (1) 
	GPIOA->OTYPER &= ~GPIO_OTYPER_ODR_5;      // Push-pull
	
	// GPIO Push-Pull: No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
	GPIOA->PUPDR  &= ~GPIO_PUPDR_PUPDR5;  // No pull-up, no pull-down
	
	//BUTTON
	//Sets the GPIO mode to input (00), no value line required after
	GPIOC->MODER &= ~GPIO_MODER_MODE13; 

	//Sets GPIOC to no pull-up, no pull-down
	GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD13; 
	
	//Configure Alternate Function
	GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL5;
	GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL0;
	
	GPIOA->AFR[0] |= GPIO_AFRL_AFSEL5_0;
	GPIOA->AFR[0] |= GPIO_AFRL_AFSEL0_1;
	
}

//LED
void TIM2_CH1_Init(){
	
	//Enable Timer Clock
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	
	//Counting Direction
	TIM2->CR1 &= ~TIM_CR1_DIR;
	
	//Prescaler
	TIM2->PSC = 39;
	
	//Auto Reload
	TIM2->ARR = 999;
	
	//Output Compare Register
	TIM2-> CCR1 = 500;
	
	//Clear output comepare mode
	TIM2->CCMR1 &=  ~TIM_CCMR1_OC1M;
	
	//Select PWM mode 1
	TIM2->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;
	
	//Output 1 Enable
	TIM2->CCMR1 |= TIM_CCMR1_OC1PE;
	
	//Select output polarity
	TIM2->CCER &= ~TIM_CCER_CC1P;
	
	//Enable the output of channel
	TIM2->CCER |= TIM_CCER_CC1E;
	
	//Main output enable (MOE)
	TIM2->BDTR |= TIM_BDTR_MOE;
	
	//Start the counter
	TIM2->CR1 |= TIM_CR1_CEN;
}

//Servo
void TIM5_CH1_Init(){
	
	//Enable Timer Clock
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM5EN;
	
	//Set timer to counting up
	TIM5->CR1 &= ~TIM_CR1_DIR;
	
	//Prescaler to slow down input clock
	TIM5->PSC = 7;
	
	//Auto Reload
	TIM5->ARR = 9999;
	
	//Output Compare Register, Initial Duty Cycle
	TIM5-> CCR1 = 4999;
	
	//Clear output compare mode, PWM mode 1 due to counting up
	TIM5->CCMR1 &=  ~TIM_CCMR1_OC1M;
	
	//Allows the ARR value to be counted up to
	TIM5->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;
	
	//Output 1 Enable
	TIM5->CCMR1 |= TIM_CCMR1_OC1PE;
	
	//Select output polarity to 0, because we want the signal to be doing something at a high value
	TIM5->CCER &= ~TIM_CCER_CC1P;
	
	//Enable the output of channel 1
	TIM5->CCER |= TIM_CCER_CC1E;
	
	//Main output enable (MOE)
	TIM5->BDTR |= TIM_BDTR_MOE;
	
	//Start the counter
	TIM5->CR1 |= TIM_CR1_CEN;
}