#include "stm32l476xx.h"
#include "SysClock.h"

// PA.5  <--> Green LED
// PC.13 <--> Blue user button
#define LED_PIN			5
#define MSI_RANGE		4
#define MSI_ON			1
#define MSI_RGSEL		3
#define MSI_RDY			1

extern void stepper_pin_init(void);
extern void full_step_cw(void);
extern void full_step_ccw(void);


void GPIOA_led_init(void);
void Delay(uint32_t nTime);
void SysTick_Initialize (uint32_t ticks);
void System_Clock_init(void);

volatile int timedelay;

volatile int ODR_pin;

int main(void){

	ODR_pin = 5;
	
	System_Clock_init(); // Switch System Clock = 80 MHz
	
	GPIOA_led_init();			//led initialized
	
	stepper_pin_init();			//stepper_pin_init pin initialized
	
	SysTick_Initialize (7999);		//initalizing systick with 79999 ticks
	
	while(1) {
	Delay(1000);
	GPIOA->ODR ^= (1<<(LED_PIN));				//toggle led//standard lab code
//	GPIOA->ODR ^= (1<<(ODR_pin));
//	ODR_pin ++;
//	if (ODR_pin == 10) ODR_pin=5;
	}
		
	
	while(1);						//deadloop
}

void SysTick_Initialize (uint32_t ticks) {

	SysTick->CTRL = 0;						//disable Systick IRQ & counter
	
	SysTick->LOAD = ticks - 1;		//Sets the value of reload register
	
	NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);		//Set interrupt priority of systick
	
	SysTick->VAL = 0;			//reset systick counter value
	
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;		//select the processors clocl as systicks
	
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;			//enable systick exception request
	
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;				//enable systick timer
	
}



void SysTick_Handler (void) {
	if (timedelay > 0)			//decrement time delay
		timedelay --;					//decrement
}

void Delay(uint32_t nTime) {
	timedelay = nTime;			
	while(timedelay != 0);			//busy loop
}

void System_Clock_init(void){
	RCC->CR &= ~MSI_ON;			//Disable MSION
	
	RCC->CR &= ~(0xf << MSI_RANGE);		//clear range bits
	RCC->CR |= (0x7 << MSI_RANGE);		//set range bits to 0x7
	
	RCC->CR |= (1<<MSI_RGSEL);				//setting RGSEL to 1
	
	RCC->CR |= MSI_ON;			//Disable MSION
	
	while((RCC->CR & (1<<MSI_RDY)) != (1<<MSI_RDY));
	
}


void GPIOA_led_init(void){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;	// Enable the clock of Port A
	int i =0;
	for(i=0; i<10; i++){
		GPIOA->MODER &= ~(3<<(2*i)); // Clear mode bits for pin 5
		GPIOA->MODER |= (1<<(2*i)); // Set the mode bits to 01
		GPIOA->OTYPER &= ~(1<<(i)); // Clear bit 5
		GPIOA->OTYPER |= (0);			// Set OTYPER to push pull
		GPIOA->PUPDR &= ~(3<<(2*i));	//Clear bits 11 and 10 and set PUPDR to 0
		GPIOA->PUPDR |= (0);			//set PUPDR to no pullup/nopulldown
	}
}
