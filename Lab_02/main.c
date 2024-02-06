#include "stm32l476xx.h"
#include "SysClock.h"

// PC.5 <--> A
// PC.6 <--> notA (!A)
// PC.8 <--> B
// PC.8 <--> notB (!B)
#define A_PIN							5
#define NOT_A_PIN					6
#define B_PIN							8
#define NOT_B_PIN					9
#define max_delay_full		13000
#define max_delay_half		6500
#define BUTTON_PIN				13

void setup_pushbutton_gpio();
void setup_motor_gpio();
int button_press();
void run_full_stepping_sequence();
void run_half_stepping_sequence();
void full_stepping_sequence_360();
void half_stepping_sequence_360();
void full_stepping_sequence_90();

int main(void){
	System_Clock_Init(); // Switch System Clock = 80 MHz
	setup_pushbutton_gpio();
	setup_motor_gpio();
	while(1){
	if(!(GPIOC->IDR & GPIO_IDR_IDR_13)){					//if button pressed
			full_stepping_sequence_90();					//rotate 90
		
			
		};
	}
}


int button_press(){
	if(GPIOC->IDR & GPIO_IDR_IDR_13)		return 0;					//if button pressed
	else return 1;
	
}
void setup_pushbutton_gpio(){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;	
	//Enable clock for Port C
	GPIOC->MODER &= ~(3<<(2*BUTTON_PIN)); 
	GPIOC->MODER |= (0<<(2*BUTTON_PIN)); 
	// Clear mode bits for pin 5
	// Set the mode bits to 00
}
void setup_motor_gpio(){
	// Setup GPIOC for motor control pins
	// Enable step signal clock for stepper motor signals
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN; // Port C
	// Setting Mode Registers to output mode (01):
	// Clear mode registers of driver pins
	GPIOC->MODER &= ~(3UL << (2 * A_PIN));
	GPIOC->MODER &= ~(3UL << (2 * NOT_A_PIN));
	GPIOC->MODER &= ~(3UL << (2 * B_PIN));
	GPIOC->MODER &= ~(3UL << (2 * NOT_B_PIN));
	// Set mode registers to output mode (01):
	GPIOC->MODER |= 1UL << (2 * A_PIN);
	GPIOC->MODER |= 1UL << (2 * NOT_A_PIN);
	GPIOC->MODER |= 1UL << (2 * B_PIN);
	GPIOC->MODER |= 1UL << (2 * NOT_B_PIN);
	// Set output type registers to push-pull (0)
	GPIOC->OTYPER &= ~(1UL << A_PIN);
	GPIOC->OTYPER &= ~(1UL << NOT_A_PIN);
	GPIOC->OTYPER &= ~(1UL << B_PIN);
	GPIOC->OTYPER &= ~(1UL << NOT_B_PIN);
	
	GPIOC -> OSPEEDR	&= ~(GPIO_OSPEEDR_OSPEED5 | GPIO_OSPEEDR_OSPEED6 | GPIO_OSPEEDR_OSPEED8 | GPIO_OSPEEDR_OSPEED9);
	// https://components101.com/motors/28byj-48-stepper-motor
	// Set to no pull up no pull down (00)
	GPIOC->PUPDR &= ~(3UL << 2 * A_PIN);
	GPIOC->PUPDR &= ~(3UL << 2 * NOT_A_PIN);
	GPIOC->PUPDR &= ~(3UL << 2 * B_PIN);
	GPIOC->PUPDR &= ~(3UL << 2 * NOT_B_PIN);
	// Make sure ODR is clear
	GPIOC->ODR &= ~(1 << A_PIN | NOT_A_PIN | B_PIN | NOT_B_PIN);
}

void run_half_stepping_sequence(){
	int i, j, k;
	while(1){
		for(i=0; (i<=512); i++){
			for(j=0; j<8; j++){
				for(k=0; k<max_delay_half; k++); // Max smallest delay
				switch(j){
					case 0:
						GPIOC->ODR |= 1UL<<A_PIN;
						GPIOC->ODR |= 1UL<<NOT_B_PIN;
						break;
					case 1:
						GPIOC->ODR &= ~(1UL<<NOT_B_PIN);
						break;
					case 2:
						GPIOC->ODR |= 1UL<<B_PIN;
						break;
					case 3:
						GPIOC->ODR &= ~(1UL<<A_PIN);
						break;
					case 4:
						GPIOC->ODR |= (1<<NOT_A_PIN);
						break;
					case 5:
						GPIOC->ODR &= ~(1UL<<B_PIN);
						break;
					case 6:
						GPIOC->ODR |= 1UL<<NOT_B_PIN;
						break;
					case 7:
						GPIOC->ODR &= ~(1UL<<NOT_A_PIN);
						break;
					default:
						break;
					}
			}
		}
	}
}

void half_stepping_sequence_360(){
	int i, j, k;
	int FULL_ROTATION_STEPS = 512;
	int step_count = 0;
	while(step_count<FULL_ROTATION_STEPS){
		for(i=0; i<=512; i++){
			for(j=0; j<8; j++){
				for(k=0; k<max_delay_half; k++); // Max smallest delay
				switch(j){
					case 0:
						GPIOC->ODR |= 1UL<<A_PIN;
						GPIOC->ODR |= 1UL<<NOT_B_PIN;
						break;
					case 1:
						GPIOC->ODR &= ~(1UL<<NOT_B_PIN);
						break;
					case 2:
						GPIOC->ODR |= 1UL<<B_PIN;
						break;
					case 3:
						GPIOC->ODR &= ~(1UL<<A_PIN);
						break;
					case 4:
						GPIOC->ODR |= (1<<NOT_A_PIN);
						break;
					case 5:
						GPIOC->ODR &= ~(1UL<<B_PIN);
						break;
					case 6:
						GPIOC->ODR |= 1UL<<NOT_B_PIN;
						break;
					case 7:
						GPIOC->ODR &= ~(1UL<<NOT_A_PIN);
						break;
					default:
						break;
					}
			}
			step_count++;
		}
	}
}

void full_stepping_sequence_90(){
	int i, j, k;
	int FULL_ROTATION_STEPS = 128;
	int step_count = 0;
	while (step_count < FULL_ROTATION_STEPS){
		// Full Steping Sequence
		for (i = 0; i<=128; i++){
			for (j=0; j<4; j++){
				for (k=0; k<max_delay_full; k++); //delay to allow proper movement
				switch(j){
					case 0:
						GPIOC->ODR &= ~(1UL<< NOT_A_PIN);
						GPIOC->ODR |= (1UL<<A_PIN);
						GPIOC->ODR |= (1UL<<NOT_B_PIN);
						break;
					case 1:
						GPIOC->ODR &= ~(1UL<<NOT_B_PIN);
						GPIOC->ODR |= (1UL<<B_PIN);
						break;
					case 2:
						GPIOC->ODR &= ~(1UL<<A_PIN);
						GPIOC->ODR |= (1UL<<NOT_A_PIN);
						break;
					case 3:
						GPIOC->ODR &= ~(1UL << B_PIN);
						GPIOC->ODR |= (1UL << NOT_B_PIN);
						break;
					default:
						break;
				}
			}
			step_count++;
		}
	}
}

void full_stepping_sequence_360(){
	int i, j, k;
	int FULL_ROTATION_STEPS = 512;
	int step_count = 0;
	while (step_count < FULL_ROTATION_STEPS){
		// Full Steping Sequence
		for (i = 0; i<=128; i++){
			for (j=0; j<4; j++){
				for (k=0; k<max_delay_full; k++); //delay to allow proper movement
				switch(j){
					case 0:
						GPIOC->ODR &= ~(1UL<< NOT_A_PIN);
						GPIOC->ODR |= (1UL<<A_PIN);
						GPIOC->ODR |= (1UL<<NOT_B_PIN);
						break;
					case 1:
						GPIOC->ODR &= ~(1UL<<NOT_B_PIN);
						GPIOC->ODR |= (1UL<<B_PIN);
						break;
					case 2:
						GPIOC->ODR &= ~(1UL<<A_PIN);
						GPIOC->ODR |= (1UL<<NOT_A_PIN);
						break;
					case 3:
						GPIOC->ODR &= ~(1UL << B_PIN);
						GPIOC->ODR |= (1UL << NOT_B_PIN);
						break;
					default:
						break;
				}
			}
			step_count++;
		}
	}
}

void run_full_stepping_sequence(){
	int FULL_ROTATION_STEPS = 512;
	int i, j, k;
	while (1){
		// Full Steping Sequence
		for (i = 0; i<=128; i++){
			for (j=0; j<4; j++){
				for (k=0; k<max_delay_full; k++); //delay to allow proper movement
				switch(j){
					case 0:
						GPIOC->ODR &= ~(1UL<< NOT_A_PIN);
						GPIOC->ODR |= (1UL<<A_PIN);
						GPIOC->ODR |= (1UL<<NOT_B_PIN);
						break;
					case 1:
						GPIOC->ODR &= ~(1UL<<NOT_B_PIN);
						GPIOC->ODR |= (1UL<<B_PIN);
						break;
					case 2:
						GPIOC->ODR &= ~(1UL<<A_PIN);
						GPIOC->ODR |= (1UL<<NOT_A_PIN);
						break;
					case 3:
						GPIOC->ODR &= ~(1UL << B_PIN);
						GPIOC->ODR |= (1UL << NOT_B_PIN);
						break;
					default:
						break;
				}
			}
		}
	}
}