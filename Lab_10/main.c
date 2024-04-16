#include <string.h>
#include <stdio.h>
#include "stm32l476xx.h"
#include "SysClock.h"


// PA.5  <--> Green LED
// PC.13 <--> Blue user button
// PA.1	 <--> ADC Input
#define LED_PIN    			5
#define ADC_PIN					1
#define SERV_PIN				0
#define oneV						1199.56
#define twoV						2398.63
#define adc_to_volts_factor	1199.06
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

volatile uint32_t ADC_Output = 0;
int i;
float volts = 0;

// In this program, Vref = approx 3.416V

// Turn on HSI in register RCC->CR and wait for it is ready
void System_Clock(void) {
	RCC->CR &= ~RCC_CR_HSION;									// Turn off the HSI clock
	RCC->CR |= RCC_CR_HSION;									// Re-enable HSION bit back to 1
	while ((RCC->CR & RCC_CR_HSIRDY) ==0 ) {	// Wait until MSI is ready by checking if the HSIRDY bit in RCC->CR is set to 1
		// Wait
	}
	RCC->CFGR &= ~RCC_CFGR_SW; 								// Clear bits for CFGR for clock setting
	RCC->CFGR |= RCC_CFGR_SW_HSI;							// Make sure HSI clock is correctly selected in RCC->CFGR
}

// Configure pin PA.5 as output with push-pull to drive the green LED
void Pin_Init() {
	// Enable the clock of Port A
	RCC->AHB2ENR &= ~RCC_AHB2ENR_GPIOAEN;			// Clear all bits of interests
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;			// Set bits of interests to target value
	
	// Set pin PA 5 as GPIO output with pushpull
	// GPIO Mode: Input(00), Output(01), AlterFunc(10), Analog(11, reset)
	GPIOA->MODER &= ~GPIO_MODER_MODER5;				// Clear mode bits for pin 5
	GPIOA->MODER |= GPIO_MODER_MODER5_0;			// Set the mode bits to 01
	// GPIO Output Type: Output push-pull (0, reset), Output open drain (1) 
	// Set the output type of Pin 5 as push-pull
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT5;				// Clear output type bits for pin 5
	
	// GPIO Push-Pull: No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
	// Set PA 5 to no pullup and no pulldown
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD5;				// Clear pull-up/pull-down bits for pin 5
}

// Configure pin PA.1 as analog mode
void ADC_INPUT_GPIO_Init(){
	// Enable the clock of Port A
	RCC->AHB2ENR &= ~RCC_AHB2ENR_GPIOAEN;			// Clear all bits of interests
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;			// Set bits of interests to target value
	
	// Set pin PA 1 as analog mode
	// GPIO Mode: Input(00), Output(01), AlterFunc(10), Analog(11, reset)
	GPIOA->MODER &= ~GPIO_MODER_MODER1;				// Clear mode bits for pin 1
	GPIOA->MODER |= GPIO_MODER_MODE1;					// Set the mode bits to 11
	
	GPIOA->ASCR &= ~GPIO_ASCR_ASC1;						// Clear ASCR bits for pin 1
	GPIOA->ASCR |= GPIO_ASCR_ASC1; 						// connect analog pin to ADC
}

void ADC_Wakeup (void) {
	int wait_time;
	// Exit deep power down mode if still in that state
	// DEEPPWD = 0: ADC not in deep-power down
	// DEEPPWD = 1: ADC in deep-power-down (default reset state)
	if ((ADC1->CR & ADC_CR_DEEPPWD) == ADC_CR_DEEPPWD)
		ADC1->CR &= ~ADC_CR_DEEPPWD;
	// Enable the ADC internal voltage regulator
	// Before performing any operation, such as launching a calibration or enabling the ADC,
	// the ADC voltage regulator must first be enabled, and the software must wait for the
	// regulator start-up time.
	ADC1->CR |= ADC_CR_ADVREGEN;
	// Wait for ADC voltage regulator start-up time
	// The software must wait for the startup time of the ADC voltage regulator
	// (T_ADCVREG_STUP, i.e., 20 us) before launching a calibration or enabling the ADC.
	wait_time = 20 * (80000000 / 1000000);
	while (wait_time != 0) {
		wait_time--;
	}
}

// Initialization ADC 1
void ADC_Setup(){
	
	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;				// Enable ADC clock bit
	ADC1->CR &= ~ADC_CR_ADEN;									// Clear ADC_CR_ADEN bit to disable ADC1
	ADC123_COMMON->CCR |= SYSCFG_CFGR1_BOOSTEN; // Enable I/O analog switches voltage booster
	//SYSCFG->CFGR1 |= SYSCFG_CFGR1_BOOSTEN;		// Enable I/O analog switches voltage booster
	ADC123_COMMON->CCR |= ADC_CCR_VREFEN;			// Enable conversion of internal channels
	ADC123_COMMON->CCR |= ADC_CCR_PRESC;			// Select the ADC clock as not divided (set bits to 0)
	ADC123_COMMON->CCR |= ADC_CCR_CKMODE;			// Select synchronous clock mode (HCLK/1) (0b01)
	ADC123_COMMON->CCR &= ~ADC_CCR_DUAL;			// Configure all ADCs as indepdent (clear ADC_CCR_DUAL bits)
	ADC_Wakeup();														// Wake up from deep sleep using ADC1_Wakeup()
	ADC1->CFGR &= ~ADC_CFGR_RES;							// Configure ADC to have 12 bit resolution: 0b00							[unsure if i did this step correctly]
	ADC1->CFGR &= ~ADC_CFGR_ALIGN;						// Set right-alignment of 12-bit result inside result register (clear ADC_CFGR_ALIGN)
	ADC1->SQR1 &= ~ADC_SQR1_L;								// Select 1 conversion in regular channel conversion sequence
	
	// Specify the channel 6 as the 1st conversion in regular sequence
	ADC1->SQR1 &= ~ADC_SQR1_SQ1;							// Clear the SQR1 bits
	ADC1->SQR1 |= 6U<<6;											// PA.1: ADC12_IN6
	
	ADC1->DIFSEL &= ~ADC_DIFSEL_DIFSEL_6;			// Configure the channel 6 as single-ended
	
	// Select the ADC sampling time. Set it to 1
	ADC1->SMPR1 &= ~ADC_SMPR1_SMP6;						// clear bits
	ADC1->SMPR1 |= ADC_SMPR1_SMP6_1;					// Set it to 1
	
	ADC1->CFGR &= ~ADC_CFGR_CONT;							// Select the discontinuous mode
	ADC1->CFGR &= ~ADC_CFGR_EXTEN;						// Select software trigger
	ADC1->CR |= ADC_CR_ADEN;									// Reenable ADC1
	while((ADC1->ISR & ADC_ISR_ADRDY) == 0);	// Wait until ADC1 is ready
}

void Software_Trigger(){
	ADC1->CR |= ADC_CR_ADSTART;													// Start the conversion by setting the ADC_CR_ADSTART bit in the ADC1->CR register
	while((ADC123_COMMON->CSR & ADC_CSR_EOC_MST) == 0);	// Wait for the completion of ADC conversion (ADC_CSR_EOC_MST bit in the ADC123_COMMON->CSR register to go high)
	ADC_Output = ADC1->DR;															// The conversion result is saved in register ADC1->DR
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

volatile int vout=0;
int main(void){

	System_Clock();
	Pin_Init();
	ADC_INPUT_GPIO_Init();
	ADC_Setup();
	
	
  // program hangs here
	while(1){
		Software_Trigger();
		vout = adc_to_volts_factor*ADC_Output;
		// Part B - LED turns on when the voltage is larger than 2V and turns off when the voltage is smaller than 1V
		if (ADC_Output < oneV)
			GPIOA->ODR &= ~GPIO_ODR_ODR_5;
		if (ADC_Output > twoV)
			GPIOA->ODR |= GPIO_ODR_OD5;
		
		
		
	}
}