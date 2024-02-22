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
	IMPORT full_step
	IMPORT half_step

LED_PIN			EQU	5
A_PIN			EQU 5
NOT_A_PIN		EQU 6
B_PIN			EQU 8
NOT_B_PIN		EQU 9
DLY				EQU 6500
FULL_360		EQU 512
HALF_360		EQU 1024
	
	AREA    main, CODE, READONLY
	EXPORT	__main				; make __main visible to linker
	ENTRY			
			

		

__main	PROC
		
	BL stepper_pin_init
	
	MOV r4, #FULL_360
loop	
	BL full_step
	SUBS r4, #1
	BNE	loop


stop 	B 		stop     		; dead loop & program hangs here

	ENDP
					
	ALIGN			

	AREA    myData, DATA, READWRITE
	ALIGN
array	DCD   1, 2, 3, 4
	END