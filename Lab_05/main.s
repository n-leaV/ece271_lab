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
	IMPORT stepper_pin_init
	IMPORT delay
	IMPORT full_step_cw
	IMPORT full_step_ccw
	IMPORT half_step
	IMPORT butt_init

A_PIN			EQU 5
NOT_A_PIN		EQU 6
B_PIN			EQU 8
NOT_B_PIN		EQU 9
BUTTON_PIN 		EQU 13
DLY				EQU 6500
FULL_360		EQU 512
HALF_360		EQU 1024
	
	AREA    main, CODE, READONLY
	EXPORT	__main				; make __main visible to linker
	ENTRY			
			

		

__main	PROC
		
	BL stepper_pin_init
	BL butt_init
;;;;-------------------------SOMETHING COOL-----------------------------------------------------
;;;;	MOV r5, #0						;Initializing r5, I use this to determine the direction.
;;;;start
;;;;	LDR r0, =GPIOC_BASE
;;;;	LDR r1, [r0, #GPIO_IDR]			;Load Idr
;;;;	ROR r1, r1, #BUTTON_PIN			;Rotate until button pin is in the LSB position
;;;;	BIC r1, r1, #0xfffffffe			;Clear everything but LSB
;;;;	EOR r1, r1, #1					;Button is high when open, so this flips the bit
;;;;	CMP r1, #1						;Compare LSB to #1
;;;;	BEQ	pres						;If equal, go to pres//means the button was pressed
;;;;	B	start
;;;;loop
;;;;pres
;;;;	CMP r5, #0						;Checking r5 to determine direction of spin
;;;;	BEQ	cw							;If 0, clockwise
;;;;	B	ccw							;Else Counter Clockwise
;;;;cw	
;;;;	BL 	full_step_cw				;Runs only when button is held down
;;;;	B	skip
;;;;ccw	
;;;;	BL	full_step_ccw				;Runs only when button is held down
;;;;skip	
;;;;	LDR r0, =GPIOC_BASE
;;;;	LDR r1, [r0, #GPIO_IDR]			;Testing if the button is still pressed//same code as above
;;;;	ROR r1, r1, #BUTTON_PIN
;;;;	BIC r1, r1, #0xfffffffe	
;;;;	EOR r1, r1, #1					
;;;;	CMP r1, #1
;;;;	BNE	break
;;;;	B	loop
;;;;break
;;;;	EOR r5, r5, #1					;Flips r5, therefore changing direction
;;;;	B	start
;;;;-------------------------------------------------------------------------------------------------


;;;;--------WORKING LAB CODE---------
	MOV r4, #FULL_360
loop	
	BL full_step_cw
	SUBS r4, #1
	BNE	loop
;;;;---------------------------------

stop 	B 		stop     		; dead loop & program hangs here

	ENDP
					
	ALIGN			

	AREA    myData, DATA, READWRITE
	ALIGN
array	DCD   1, 2, 3, 4
	END