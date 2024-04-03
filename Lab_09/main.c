#include "stm32l476xx.h"
#include "SysClock.h"

// PA.5  <--> Green LED
// PC.13 <--> Blue user button
#define B_PIN6    6
#define B_PIN5		5
#define BUTTON_PIN 13

void System_Clock_init(void);
void GPIOB_pin_init(int B_PIN);

volatile int32_t pulse_width = 0;
volatile int32_t last_captured = 0;
volatile int32_t signal_polarity = 0;
volatile int32_t OCT = 0;
volatile int32_t inches;

int main(void){

	System_Clock_init(); // Switch System Clock = 80 MHz
	GPIOB_pin_init(B_PIN6);
	GPIOB_pin_init(B_PIN5);

	
	
	
	while(1);
}
void System_Clock_init(void){
	RCC->CR |= RCC_CR_HSION;			//Disable MSION
	
	while((RCC->CR & RCC_CR_HSIRDY) != RCC_CR_HSIRDY);
	
	RCC->CFGR &= ~(RCC_CFGR_SW);
	RCC->CFGR |= RCC_CFGR_SW_1;
}

void GPIOB_pin_init(int B_PIN){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;	// Enable the clock of Port B
	
	GPIOA->MODER &= ~(3<<(2*B_PIN)); // Clear mode bits for pin 6
	GPIOA->MODER	|= (2<<(2*B_PIN)); 	//set the mode bits to 10 - alternate function
	
	GPIOA->AFR[0] &= ~(0xf<<(4*B_PIN));	//selecting alternate function 1
	GPIOA->AFR[0] |= 2UL<<(4*B_PIN);	//TIM4 channel 1 is defined as 02

	GPIOA->OTYPER &= ~(1<<(B_PIN)); // Clear bit 5
	GPIOA->OTYPER |= (0);			// Set OTYPER to push pull
	
	GPIOA->PUPDR &= ~(3<<(2*B_PIN));	//Clear bits 11 and 10
	GPIOA->PUPDR |= (0);			//set PUPDR to no pullup/nopulldown
	
	GPIOA->ODR &= ~(1<<(B_PIN));  // clears bit 5 of ODR/set bit 5 to zero
}



void TIM4_CH1_Init() {
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN;		//enable timer clock
	
	TIM4->CR1 &= ~TIM_CR1_DIR; //select up counting
	
	TIM4->PSC = 15;		//16MHz/(1+PSC) = 1 MHz   ->PSC=15
	
	TIM4->ARR = 0xffff;			//Maximum ARR value
	
	
	TIM4->CCMR1 &= ~TIM_CCMR1_CC1S;		// clear capture/compare mode bits
	TIM4->CCMR1 |= TIM_CCMR1_CC1S_0;	// set input timer 1
	
	TIM4->CCMR1 &= ~TIM_CCMR1_IC1F;  // No filtering
	
	TIM4->CCER |= TIM_CCER_CC1P|TIM_CCER_CC1NP; // Both edges generate interrupts
	
	TIM4->CCMR1 &= ~(TIM_CCMR1_IC1PSC);		//clear psc and capture each transition
	
	TIM4->CCER |= TIM_CCER_CC1E;			//enable capture for channel 1
	
	TIM4->DIER |= TIM_DIER_CC1IE;		//allow TIM4 channel 1 to generate interrupts
	
	TIM4->DIER |= TIM_DIER_CC1DE; // allow TIM4 channel 1 to generate DMA requests

	TIM4->CR1 |= TIM_CR1_CEN;		//enable the timer counter

	NVIC_SetPriority(TIM4_IRQn, 0); //set to 0, highest priority

	NVIC_EnableIRQ(TIM4_IRQn);		// enable TIM4 in the interrupt controller

	//while(1)
}

//void TIM3_CH2_INIT(void){
//    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN;        //timer 3    clock
//    
//    TIM3->CR1 &= ~TIM_CR1_DIR;    //choosing upcounting
//    
//    TIM3->PSC = 15;            //16MHz/(1+PSC) = 1MHz psc value to make a 1 MHz clock
//    
//    TIM3->ARR = 0xFFFF;            //maximum arr value
//    
//    TIM3->CCR2 = 10;                                        //10us pulse
//    
//    TIM3->CCMR1 &= ~TIM_CCMR1_OC2M;            //clear output compare bits
//    
//    TIM3->CCMR1 |= TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2;        //select upcounting
//    
//    TIM3->CCMR1 |= TIM_CCMR1_OC1PE;                //output 1 preload enable
//    
//    TIM3->CCER &= ~TIM_CCER_CC2P;                    //output Active High
//    
//    TIM3->CCER |= TIM_CCER_CC2E;            //starting the TIM 3 
//    
//    //TIM2->BDTR |= TIM_BDTR_MOE;
//    
//    TIM3->CR1 |= TIM_CR1_CEN;                    //enable
//}

void TIM4_IRQHandler() {
	uint32_t current_captured;
	
	if((TIM4->SR & TIM_SR_CC1IF) != 0) {
		current_captured = TIM4->CCR1;  //setting the current captured to the CCR1
		
		signal_polarity =  1-signal_polarity; // flips polarity
		
		if(signal_polarity == 0){
			pulse_width = ((current_captured-last_captured) + (OCT*(TIM4->ARR)));
			inches = pulse_width/148;
			OCT = 0;
			//pulse_width = current_captured - last_captured;
//			if(pulse_width <0)
//				pulse_width = pulse_width + TIM4->ARR;
		}
		last_captured = current_captured;
	}
	if((TIM4->SR & TIM_SR_UIF) !=0){
		if(signal_polarity){
			OCT = OCT + 1;
		}	
		TIM4->SR &= ~TIM_SR_UIF;
	}
}
