;******************** (C) Yifeng ZHU *******************************************
; @file    main.s
; @author  Yifeng Zhu
; @date    May-17-2015
; @note
;           This code is for the book "Embedded Systems with ARM Cortex-M 
;           Microcontrollers in Assembly Language and C, Yifeng Zhu, 
;           ISBN-13: 978-0982692639, ISBN-10: 0982692633
; @attension
;           This code is provided for education purpose. The author shall not be 
;           held liable for any direct, indirect or consequential damages, for any 
;           reason whatever. More information can be found from book website: 
;           http:;www.eece.maine.edu/~zhu/book
;*******************************************************************************


	INCLUDE core_cm4_constants.s		; Load Constant Definitions
	INCLUDE stm32l476xx_constants.s      

; Green LED <--> PA.5
LED_PIN	EQU	5
BUTTON_PIN EQU 13
	
	AREA    main, CODE, READONLY
	EXPORT	__main				; make __main visible to linker
	ENTRY			
				
__main	PROC
		
    ; Enable the clock to GPIO Port A	
	LDR r0, =RCC_BASE
	LDR r1, [r0, #RCC_AHB2ENR]
	ORR r1, r1, #RCC_AHB2ENR_GPIOAEN
	STR r1, [r0, #RCC_AHB2ENR]
	
	; Enable clock for GPIO Port C
	LDR r1, [r0, #RCC_AHB2ENR]
	ORR r1, r1, #RCC_AHB2ENR_GPIOCEN
	STR r1, [r0, #RCC_AHB2ENR]
	
	; MODE: 00: Input mode, 01: General purpose output mode
    ;       10: Alternate function mode, 11: Analog mode (reset state)
	LDR r0, =GPIOA_BASE
	LDR r1, [r0, #GPIO_MODER]
	BIC r1, r1, #(3<<(2*LED_PIN))
	ORR r1, r1, #(1<<(2*LED_PIN))
	STR r1, [r0, #GPIO_MODER]			;Clearing and setting LED MODER to output
									
	LDR r1, [r0, #GPIO_ODR]
	BIC r1, r1, #(1<<LED_PIN)
	STR r1, [r0, #GPIO_ODR]				;Setting LED ODR to 0 (off)
	
	LDR r1, [r0, #GPIO_PUPDR]
	BIC r1, r1, #(3<<(2*LED_PIN))
	STR r1, [r0, #GPIO_PUPDR]			;Clearing and setting LED PUPDR to no pull up, no pull down
	
	LDR r1, [r0,#GPIO_OTYPER]
	BIC r1, r1, #(1<<LED_PIN)
	STR r1, [r0, #GPIO_OTYPER]			;Clearing and setting LED OTYPER to push pull
	
	; MODE: 00: Input mode, 01: General purpose output mode
    ;       10: Alternate function mode, 11: Analog mode (reset state)
	LDR r0, =GPIOC_BASE
	LDR r1, [r0, #GPIO_MODER]
	BIC r1, r1, #(3<<(2*BUTTON_PIN))
	STR r1, [r0, #GPIO_MODER]			;Clearing and setting Button MODER to input

	LDR r1, [r0, #GPIO_PUPDR]
	BIC r1, r1, #(3<<(2*BUTTON_PIN))
	STR r1, [r0, #GPIO_PUPDR]			;Clearing and setting button PUPDR to no pull up, no pull down
	
	LDR r2, =GPIOA_BASE					;using r2 as GPIOA base







loop
	LDR r1, [r0, #GPIO_IDR]			;Load Idr
	ROR r1, r1, #BUTTON_PIN			;Rotate until button pin is in the LSB position
	BIC r1, r1, #0xfffffffe			;Clear everything but LSB
	EOR r1, r1, #1					;Button is high when open, so this flips the bit
	CMP r1, #1						;Compare LSB to #1
	BEQ	wait						;If equal, go to pres//means the button was pressed
	B skip							;Skip the button pressed code
wait
	LDR r1, [r0, #GPIO_IDR]			;Testing if the button is still pressed//same code as above
	ROR r1, r1, #BUTTON_PIN
	BIC r1, r1, #0xfffffffe
	EOR r1, r1, #1					
	CMP r1, #1
	BEQ wait
notp
;-------------------
	LDR r1, [r0, #GPIO_IDR]			;Load Idr
	ROR r1, r1, #BUTTON_PIN			;Rotate until button pin is in the LSB position
	BIC r1, r1, #0xfffffffe			;Clear everything but LSB
	EOR r1, r1, #1					;Button is high when open, so this flips the bit
	CMP r1, #1						;Compare LSB to #1
	BEQ	pres						;If equal, go to pres//means the button was pressed
	B notp							;Skip the button pressed code
pres
	LDR r3, [r2, #GPIO_ODR]
	EOR r3, r3, #(1<<LED_PIN)		;Toggle the ODR pin (flip on/off)
	STR r3, [r2, #GPIO_ODR]
hold
	LDR r1, [r0, #GPIO_IDR]			;Testing if the button is still pressed//same code as above
	ROR r1, r1, #BUTTON_PIN
	BIC r1, r1, #0xfffffffe
	EOR r1, r1, #1					
	CMP r1, #1
	BEQ hold
skip
	B	loop					;Loop to start
  
stop 	B 		stop     		; dead loop & program hangs here

	ENDP
					
	ALIGN			

	AREA    myData, DATA, READWRITE
	ALIGN
array	DCD   1, 2, 3, 4
	END