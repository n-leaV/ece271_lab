#include "stm32l476xx.h"
#include "SysClock.h"

// PA.5  <--> Green LED
// PC.13 <--> Blue user button
#define B_PIN6    6
#define B_PIN5		5
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

void System_Clock_init(void);
void GPIOB_pin_init(int B_PIN);
void TIM4_CH1_Init(void);
void TIM3_CH2_Init(void);
void TIM2_CH1_Init(void);
void TIM5_CH1_Init(void);
void LED_init(void);
void SERV_init(void);
void waitms(int);

volatile int32_t pulse_width = 0;
volatile int32_t last_captured = 0;
volatile int32_t signal_polarity = 0;
volatile uint32_t OCT = 0;
volatile int32_t inches;

int main(void){

	System_Clock_init(); // Switch System Clock = 16 MHz
	//RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOCEN); //turning on clock for gpio C
	GPIOB_pin_init(B_PIN6);
	GPIOB_pin_init(B_PIN5);
	LED_init();
	SERV_init();
	TIM4_CH1_Init();
	TIM3_CH2_Init();
	TIM2_CH1_Init();
	TIM5_CH1_Init();
	
	int spin = 75;
	while(1){

		if(inches<8){					//if button pressed							
				TIM5->CCR1 = pos90;
				waitms(1);	
			}
		else {
			TIM5->CCR1 = neg90;
		}
	}
	
	
	while(1);
}
void System_Clock_init(void){
	RCC->CR |= RCC_CR_HSION;			//Disable MSION
	
	while((RCC->CR & RCC_CR_HSIRDY) == 0);
	
	RCC->CFGR &= ~(RCC_CFGR_SW);
	RCC->CFGR |= RCC_CFGR_SW_HSI;
	
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);
}

void GPIOB_pin_init(int B_PIN){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;	// Enable the clock of Port B
	
	GPIOB->MODER &= ~(3<<(2*B_PIN)); // Clear mode bits for pin 6
	GPIOB->MODER	|= (2<<(2*B_PIN)); 	//set the mode bits to 10 - alternate function
	
	GPIOB->AFR[0] &= ~(0xf<<(4*B_PIN));	//selecting alternate function 1
	GPIOB->AFR[0] |= 2UL<<(4*B_PIN);	//TIM4 channel 1 is defined as 02

//	GPIOB->OTYPER &= ~(1<<(B_PIN)); // Clear bit 5
//	GPIOB->OTYPER |= (0);			// Set OTYPER to push pull
	
	GPIOB->PUPDR &= ~(3<<(2*B_PIN));	//Clear bits 11 and 10
	GPIOB->PUPDR |= (0);			//set PUPDR to no pullup/nopulldown
	
	GPIOB->ODR &= ~(1<<(B_PIN));  // clears bit 5 of ODR/set bit 5 to zero
}



void TIM4_CH1_Init() {
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN;		//enable timer clock
	
	//TIM4->CR1 &= ~TIM_CR1_DIR; //select up counting
	
	TIM4->PSC = 15;		//16MHz/(1+PSC) = 1 MHz   ->PSC=15
	
	TIM4->ARR = 0xffff;			//Maximum ARR value
	
	
	TIM4->CCMR1 &= ~TIM_CCMR1_CC1S;		// clear capture/compare mode bits
	TIM4->CCMR1 |= TIM_CCMR1_CC1S_0;	// set input timer 1
	
	TIM4->CCMR1 &= ~TIM_CCMR1_IC1F;  // No filtering
	
	TIM4->CCER |= TIM_CCER_CC1P|TIM_CCER_CC1NP; // Both edges generate interrupts
	
	TIM4->CCMR1 &= ~(TIM_CCMR1_IC1PSC);		//clear psc and capture each transition
	
	TIM4->CCER |= TIM_CCER_CC1E;			//enable capture for channel 1
	
	TIM4->DIER |= TIM_DIER_CC1IE;		//allow TIM4 channel 1 to generate interrupts
	
//	TIM4->DIER |= TIM_DIER_CC1DE; // allow TIM4 channel 1 to generate DMA requests
	
	TIM4->DIER |= TIM_DIER_UIE;

	TIM4->CR1 |= TIM_CR1_CEN;		//enable the timer counter

	NVIC_SetPriority(TIM4_IRQn, 0); //set to 0, highest priority

	NVIC_EnableIRQ(TIM4_IRQn);		// enable TIM4 in the interrupt controller

	//while(1)
}

void TIM3_CH2_Init(void){
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN;        //timer 3    clock
    
    TIM3->CR1 &= ~TIM_CR1_DIR;    //choosing upcounting
    
    TIM3->PSC = 15;            //16MHz/(1+PSC) = 1MHz psc value to make a 1 MHz clock
    
    TIM3->ARR = 0xFFFF;            //maximum arr value
    
    TIM3->CCR2 = 10;                                        //10us pulse
    
    TIM3->CCMR1 &= ~TIM_CCMR1_OC2M;            //clear output compare bits
    
    TIM3->CCMR1 |= TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2;        //select upcounting
    
    TIM3->CCMR1 |= TIM_CCMR1_OC1PE;                //output 1 preload enable
    
    TIM3->CCER &= ~TIM_CCER_CC2P;                    //output Active High
    
    TIM3->CCER |= TIM_CCER_CC2E;            //starting the TIM 3 
    
    //TIM2->BDTR |= TIM_BDTR_MOE;
    
    TIM3->CR1 |= TIM_CR1_CEN;                    //enable
}

void TIM4_IRQHandler() {
	//NVIC_ClearPendingIRQ(TIM4_IRQn);
	volatile uint32_t current_captured;
	
	if((TIM4->SR & TIM_SR_CC1IF) != 0) {
		current_captured = TIM4->CCR1;  //setting the current captured to the CCR1
		
		signal_polarity =  1-signal_polarity; // flips polarity
		if(signal_polarity != 0) OCT =0;
		if(signal_polarity == 0){
			pulse_width = ((current_captured-last_captured) + (OCT*(TIM4->ARR)));
			inches = pulse_width/148;
			
			//pulse_width = current_captured - last_captured;
//			if(pulse_width <0)
//				pulse_width = pulse_width + TIM4->ARR;
		}
		last_captured = current_captured;
	}
	if((TIM4->SR & TIM_SR_UIF) !=0){
//		if(signal_polarity){
			OCT ++;
//		}	
		TIM4->SR &= ~TIM_SR_UIF;
	}
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
