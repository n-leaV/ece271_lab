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
A_PIN	EQU 5
NOT_A_PIN	EQU 6
B_PIN	EQU 8
NOT_B_PIN	EQU 9
	
	AREA    main, CODE, READONLY
	EXPORT	__main				; make __main visible to linker
	ENTRY			
			
stepper_pin_init PROC
	PUSH LR
	
	LDR r0, =RCC_BASE					;Initializing Timer C
	LDR r1, [r0, #RCC_AHB2ENR]
	ORR r1, r1, #RCC_AHB2ENR_GPIOCEN
	STR r1, [r0, #RCC_AHB2ENR]
	
	LDR r0, =GPIOC_BASE					;Clearing MODER bits
	LDR r1, [r0, #GPIO_MODER]
	BIC r1, #(3<<(2*A_PIN))
	BIC r1, #(3<<(2*NOT_A_PIN))
	BIC r1, #(3<<(2*B_PIN))
	BIC r1, #(3<<(2*NOT_B_PIN))
	STR r1, [r0, #GPIO_MODER]
	
	LDR r1, #(1<<(2*A_PIN))				;Setting the MODER bits to 01
	ORR r1, r1, #(1<<(2*NOT_A_PIN))
	ORR r1 r1, #(1<<(2*B_PIN))
	ORR r1, r1, #(1<<(2*NOT_B_PIN))
	STR r1, [r0, #GPIO_MODER]
	
	LDR r1, [r0, #GPIO_OTYPER]			;Clearing OTYPER/Setting OTYPER to 0
	BIC r1, #(1<<(A_PIN))
	BIC r1, #(1<<(NOT_A_PIN))
	BIC r1, #(1<<(B_PIN))
	BIC r1, #(1<<(NOT_B_PIN))
	STR r1, [r0, #GPIO_OTYPER]
	
	LDR r1, [r0, #GPIO_OSPEEDR]			;Clearing OSPEEDR/Setting OSPEEDR to 00
	BIC r1, #(3<<(2*A_PIN))
	BIC r1, #(3<<(2*NOT_A_PIN))
	BIC r1, #(3<<(2*B_PIN))
	BIC r1, #(3<<(2*NOT_B_PIN))
	STR r1, [r0, #GPIO_OSPEEDR]
	
	LDR r1, [r0, #GPIO_PUPDR]			;Clearing PUPDR/Setting OSPEEDR to 00
	BIC r1, #(3<<(2*A_PIN))
	BIC r1, #(3<<(2*NOT_A_PIN))
	BIC r1, #(3<<(2*B_PIN))
	BIC r1, #(3<<(2*NOT_B_PIN))
	STR r1, [r0, #GPIO_PUPDR]
	
	LDR r1, [ro, #GPIO_ODR]				;Clearing ODR/Setting ODR to 0
	BIC r1, #(1<<(2*A_PIN))
	BIC r1, #(1<<(2*NOT_A_PIN))
	BIC r1, #(1<<(2*B_PIN))
	BIC r1, #(1<<(2*NOT_B_PIN))
	STR r1, [r0, #GPIO_ODR]
	
	POP LR
	BX LR
	ENDP

delay PROC
	PUSH LR
loop	
	BHI loop
	
	POP LR
	BX LR
	ENDP

full_step PROC
	PUSH LR	
	
	POP LR
	BX LR
	ENDP
		

__main	PROC
		

stop 	B 		stop     		; dead loop & program hangs here

	ENDP
					
	ALIGN			

	AREA    myData, DATA, READWRITE
	ALIGN
array	DCD   1, 2, 3, 4
	END