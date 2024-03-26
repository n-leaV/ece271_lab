#include "stm32l476xx.h"
#include "SysClock.h"

// PA.5  <--> Green LED
// PC.13 <--> Blue user button
#define LED_PIN    5
#define BUTTON_PIN 13

#define zer 75
#define pos90 100
#define neg90 50

void TIM2_CH1_Init(void);

void LED_init(void);

int main(void){
	
	int i;
	int brightness = 1;
	int stepSize = 1;
	
	LED_init();
	TIM2_CH1_Init();
	
	while(1) {																					//--->for led
		if ((brightness >=999) || (brightness <= 0))
			stepSize = -stepSize;
		
		brightness += stepSize;
		TIM2->CCR1 = brightness;
		for(i = 0; i < 1000; i++);
		
	}																										//--->for led
	
	
  // Dead loop & program hangs here
	while(1);
}

void LED_init(){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;	// Enable the clock of Port A
	
	GPIOA->MODER &= ~(3<<(2*LED_PIN)); // Clear mode bits for pin 5
	GPIOA->MODER	|= (2<<(2*LED_PIN)); 	//set the mode bits to 10 - alternate function
	
	GPIOA->AFR[0] &= ~(0xf<<(4*LED_PIN));	//selecting alternate function 1
	GPIOA->AFR[0] |= 1UL<<(4*LED_PIN);	//TIM channel 2 is defined as 01

	GPIOA->OTYPER &= ~(1<<(LED_PIN)); // Clear bit 5
	GPIOA->OTYPER |= (0);			// Set OTYPER to push pull
	
	GPIOA->PUPDR &= ~(3<<(2*LED_PIN));	//Clear bits 11 and 10
	GPIOA->PUPDR |= (0);			//set PUPDR to no pullup/nopulldown
	
	GPIOA->ODR &= ~(1<<(LED_PIN));  // clears bit 5 of ODR/set bit 5 to zero
}

void TIM2_CH1_Init() {
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;		//enable timer clock
	
	TIM2->CR1 &= ~TIM_CR1_DIR; //select up counting
	
	TIM2->PSC = 39;	//4Mhz/1+39  = 100khz				->for led
	
	//TIM2->PSC = 79;	//4Mhz/1+79  = 50khz			->for servo	
	
	TIM2->ARR = 999;	//pwm period =999+1 * (1/100khz) = 0.01s		->led
	
	//TIM2->ARR = 999;	//pwm period =999+1 * (1/50khz) = 0.02s		->servo
	
	TIM2->CCR1 = 500;	//initial duty cycle = 50%							->led
	
	//TIM2->CCR1 = zer;	//inital duty cycle of 7.5% ~1.5 ms/20 ms		75/1000 ->servo
	
	TIM2->CCMR1 &= ~TIM_CCMR1_OC1M;	// clear output compare mode bits
	
	TIM2->CCMR1 |= TIM_CCMR1_OC1M | TIM_CCMR1_OC1M_2;	//select pwm mode 1
	
	TIM2->CCMR1 |= TIM_CCMR1_OC1PE;	//output 1 preload enable - synchronicty
	
	TIM2->CCER &= ~TIM_CCER_CC1P;		//select output polarity
	
	TIM2->CCER |= TIM_CCER_CC1E;		//enable output channel 1
	
	TIM2->BDTR |= TIM_BDTR_MOE;		//Maine output enable
	
	TIM2->CR1 |= TIM_CR1_CEN;		//Start counter
}