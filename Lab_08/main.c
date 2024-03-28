#include "stm32l476xx.h"
#include "SysClock.h"

// PA.5  <--> Green LED
// PC.13 <--> Blue user button
#define LED_PIN    	5
#define SERV_PIN		0
#define BUTTON_PIN	13

#define AF0_msk			0x00f0000f;
#define AF0_val			0x00100002;

#define pos0				75			//Duty cycle of 7.5% ~1.5 ms/20 ms		75/1000 ->servo
#define pos90				125
#define neg90				25
#define LED_CCR1		500			//50% duty cycle

#define	LED_PSC			39			//4Mhz/1+39  = 100khz				->for led
#define SERV_PSC		79			//4Mhz/1+79  = 50khz			->for servo
#define LED_ARR			999			//pwm period =999+1 * (1/100khz) = 0.01s		->led
#define SERV_ARR		999			//pwm period =999+1 * (1/50khz) = 0.02s		->servo
	

void TIM2_CH1_Init(void);

void TIM5_CH1_Init(void);

void LED_init(void);

void SERV_init(void);

void waitms(int);

void button_init(void);


int main(void){
	
	int i;
	int brightness = 1;
	int stepSize = 1;
	
	button_init();
	LED_init();
	SERV_init();
	TIM2_CH1_Init();
	TIM5_CH1_Init();
	
//	while(1) {																					//--->for led
//		if ((brightness >=999) || (brightness <= 0))
//			stepSize = -stepSize;
//		
//		brightness += stepSize;
//		TIM2->CCR1 = brightness;
//		for(i = 0; i < 1000; i++);
//		
//	}																										//--->for led
														
		
		
		TIM5->CCR1 = 75;															//--->for servo
		waitms(50);
		//delay(100);

		TIM5->CCR1 = 25;
		waitms(50);
		//delay(100);

		TIM5->CCR1 = 125;
		waitms(50);
		//delay(100);

		TIM5->CCR1 = 75;															//--->for servo
	
	
															//something cool
		
		int spin = 75;
		TIM5->CCR1 = spin;
		while(1){
			if(!(GPIOC->IDR & GPIO_IDR_IDR_13)){					//if button pressed
				spin ++;								//toggle led
				TIM5->CCR1 = spin;
				waitms(1);
			//	while(!(GPIOC->IDR & GPIO_IDR_IDR_13)) {};	//wait for button to unpress	
			
			};
		};																							//COOL something
	
	
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

void SERV_init(){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;	// Enable the clock of Port A
	
	GPIOA->MODER &= ~(3<<(2*SERV_PIN)); // Clear mode bits for pin 5
	GPIOA->MODER	|= (2<<(2*SERV_PIN)); 	//set the mode bits to 10 - alternate function
	
	//GPIOA->AFR[0] &= ~(0xf<<(4*SERV_PIN));	//selecting alternate function 1
	//GPIOA->AFR[0] |= 2UL<<(4*SERV_PIN);	//TIM channel 2 is defined as 01
	GPIOA->AFR[0] &= ~AF0_msk;
	GPIOA->AFR[0] |= AF0_val;
	
	
	GPIOA->OTYPER &= ~(1<<(SERV_PIN)); // Clear bit 5
	GPIOA->OTYPER |= (0);			// Set OTYPER to push pull
	
	GPIOA->PUPDR &= ~(3<<(2*SERV_PIN));	//Clear bits 11 and 10
	GPIOA->PUPDR |= (0);			//set PUPDR to no pullup/nopulldown
	
	GPIOA->OSPEEDR &= ~(3<<(2*SERV_PIN));
	GPIOA->OSPEEDR |= 3<<(2*SERV_PIN);
	
	GPIOA->ODR &= ~(1<<(SERV_PIN));  // clears bit 5 of ODR/set bit 5 to zero
}

void TIM2_CH1_Init() {
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;		//enable timer clock
	
	TIM2->CR1 &= ~TIM_CR1_DIR; //select up counting
	
	TIM2->PSC = LED_PSC;		//->for led	
	
	TIM2->ARR = LED_ARR;			//->for led
	
	TIM2->CCR1 = LED_CCR1;						//inital duty cycle of 50%
	
	TIM2->CCMR1 &= ~TIM_CCMR1_OC1M;	// clear output compare mode bits
	
	TIM2->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;	//select pwm mode 1
	
	TIM2->CCMR1 |= TIM_CCMR1_OC1PE;	//output 1 preload enable - synchronicty
	
	TIM2->CCER &= ~TIM_CCER_CC1P;		//select output polarity
	
	TIM2->CCER |= TIM_CCER_CC1E;		//enable output channel 1
	
	TIM2->BDTR |= TIM_BDTR_MOE;		//Maine output enable
	
	TIM2->CR1 |= TIM_CR1_CEN;		//Start counter
}

void TIM5_CH1_Init() {
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM5EN;		//enable timer clock
	
	TIM5->CR1 &= ~TIM_CR1_DIR; //select up counting
	
	TIM5->PSC = SERV_PSC;		//->for servo	
	
	TIM5->ARR = SERV_ARR;			//->for servo
	
	TIM5->CCR1 = 500;						//inital duty cycle of 50% 
	
	TIM5->CCMR1 &= ~TIM_CCMR1_OC1M;	// clear output compare mode bits
	
	TIM5->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;	//select pwm mode 1
	
	TIM5->CCMR1 |= TIM_CCMR1_OC1PE;	//output 1 preload enable - synchronicty
	
	TIM5->CCER &= ~TIM_CCER_CC1P;		//select output polarity
	
	TIM5->CCER |= TIM_CCER_CC1E;		//enable output channel 1
	
	TIM5->BDTR |= TIM_BDTR_MOE;		//Maine output enable
	
	TIM5->CR1 |= TIM_CR1_CEN;		//Start counter
}

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

void button_init(void){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;	//Enable clock for Port C
	
	GPIOC->MODER &= ~(3<<(2*BUTTON_PIN)); // Clear mode bits for pin 5
	GPIOC->MODER |= (0<<(2*BUTTON_PIN)); // Set the mode bits to 00
	
	GPIOC->PUPDR &= ~(3<<(2*BUTTON_PIN));	//Clear bits 26 and 27
	GPIOC->PUPDR |= (0);			//set PUPDR to no pullup/nopulldown
}

