
	INCLUDE	core_cm4_constants.s
	INCLUDE stm32l476xx_constants.s

	EXPORT stepper_pin_init
	EXPORT delay
	EXPORT full_step_cw
	EXPORT half_step
	EXPORT butt_init
	EXPORT full_step_ccw

;-------------------------------Definitions-------------------------------------------------------------------------

LED_PIN			EQU	5				
A_PIN			EQU 5
NOT_A_PIN		EQU 6
B_PIN			EQU 8
NOT_B_PIN		EQU 9
BUTTON_PIN 		EQU 13
DLY				EQU 6500

;-------------------------------Functions-------------------------------------------------------------------------

butt_init PROC
	PUSH {LR}
	
	LDR r0, =GPIOC_BASE
	LDR r1, [r0, #GPIO_MODER]
	BIC r1, r1, #(3<<(2*BUTTON_PIN))
	STR r1, [r0, #GPIO_MODER]			;Clearing and setting Button MODER to input

	LDR r1, [r0, #GPIO_PUPDR]
	BIC r1, r1, #(3<<(2*BUTTON_PIN))
	STR r1, [r0, #GPIO_PUPDR]			;Clearing and setting button PUPDR to no pull up, no pull down
	
	POP{PC}
	ENDP


stepper_pin_init PROC
	PUSH {LR}
	
	LDR r0, =RCC_BASE					;Initializing Timer C
	LDR r1, [r0, #RCC_AHB2ENR]
	ORR r1, r1, #RCC_AHB2ENR_GPIOCEN
	STR r1, [r0, #RCC_AHB2ENR]
	
	LDR r0, =GPIOC_BASE					;Clearing MODER bits
	LDR r1, [r0, #GPIO_MODER]
	BIC r1, r1, #(3<<(2*A_PIN))
	BIC r1, r1, #(3<<(2*NOT_A_PIN))
	BIC r1, r1, #(3<<(2*B_PIN))
	BIC r1, r1, #(3<<(2*NOT_B_PIN))		;We dont need to store, since the next operations are happening to MODER
										
	ORR r1, r1, #(1<<(2*A_PIN))			;Setting the MODER bits to 01(general purpose output  mode)
	ORR r1, r1, #(1<<(2*NOT_A_PIN))
	ORR r1, r1, #(1<<(2*B_PIN))
	ORR r1, r1, #(1<<(2*NOT_B_PIN))
	STR r1, [r0, #GPIO_MODER]
	
	LDR r1, [r0, #GPIO_OTYPER]		;Clearing OTYPER/Setting OTYPER to 0
	BIC r1, r1, #(1<<(A_PIN))
	BIC r1, r1, #(1<<(NOT_A_PIN))
	BIC r1, r1, #(1<<(B_PIN))
	BIC r1, r1, #(1<<(NOT_B_PIN))
	STR r1, [r0, #GPIO_OTYPER]
	
	LDR r1, [r0, #GPIO_OSPEEDR]		;Clearing OSPEEDR/Setting OSPEEDR to 00
	BIC r1, r1, #(3<<(2*A_PIN))
	BIC r1, r1, #(3<<(2*NOT_A_PIN))
	BIC r1, r1, #(3<<(2*B_PIN))
	BIC r1, r1, #(3<<(2*NOT_B_PIN))
	STR r1, [r0, #GPIO_OSPEEDR]
	
	LDR r1, [r0, #GPIO_PUPDR]		;Clearing PUPDR/Setting OSPEEDR to 00
	BIC r1, r1, #(3<<(2*A_PIN))
	BIC r1, r1, #(3<<(2*NOT_A_PIN))
	BIC r1, r1, #(3<<(2*B_PIN))
	BIC r1, r1, #(3<<(2*NOT_B_PIN))
	STR r1, [r0, #GPIO_PUPDR]
	
	LDR r1, [r0, #GPIO_ODR]			;Clearing ODR/Setting ODR to 0
	BIC r1, r1, #(1<<A_PIN)
	BIC r1, r1, #(1<<NOT_A_PIN)
	BIC r1, r1, #(1<<B_PIN)
	BIC r1, r1, #(1<<NOT_B_PIN)
	STR r1, [r0, #GPIO_ODR]
	
	POP {PC}
	ENDP

delay PROC							;Delay function//make sure you load DLY into r0 before calling
	PUSH {LR}
loop	SUBS r0, r0, #1
		BNE	loop
	POP {PC}
	ENDP


full_step_cw PROC
	PUSH {LR}	
	
	LDR r0, =GPIOC_BASE
	LDR r1, [r0, #GPIO_ODR]
	BIC r1, r1, #(1<<A_PIN)
	BIC r1, r1, #(1<<NOT_A_PIN)
	BIC r1, r1, #(1<<B_PIN)
	BIC r1, r1, #(1<<NOT_B_PIN)		;Clearing ODR
	
	ORR r1, r1, #(1<<A_PIN)
	ORR r1, r1, #(1<<NOT_B_PIN)			
	STR r1, [r0, #GPIO_ODR]			;Phase 01 (Setting A and Bnot to high, else to low)
	
	MOV r0, #DLY
	BL delay						;Delay func
	
	LDR r0, =GPIOC_BASE
	LDR r1, [r0, #GPIO_ODR]
	BIC r1, r1, #(1<<A_PIN)
	BIC r1, r1, #(1<<NOT_A_PIN)
	BIC r1, r1, #(1<<B_PIN)
	BIC r1, r1, #(1<<NOT_B_PIN)		;Clearing ODR
	
	ORR r1, r1, #(1<<A_PIN)
	ORR r1, r1, #(1<<B_PIN)
	STR r1, [r0, #GPIO_ODR]			;Phase 02 (Setting A and B to high, else to low)

	MOV r0, #DLY
	BL delay						;Delay func
	
	LDR r0, =GPIOC_BASE
	LDR r1, [r0, #GPIO_ODR]
	BIC r1, r1, #(1<<A_PIN)
	BIC r1, r1, #(1<<NOT_A_PIN)
	BIC r1, r1, #(1<<B_PIN)
	BIC r1, r1, #(1<<NOT_B_PIN)		;Clearing ODR
	
	ORR r1, r1, #(1<<NOT_A_PIN)
	ORR r1, r1, #(1<<B_PIN)
	STR r1, [r0, #GPIO_ODR]			;Phase 03 (Setting Anot and B to high, else to low)
	
	MOV r0, #DLY
	BL delay						;Delay func
	
	LDR r0, =GPIOC_BASE
	LDR r1, [r0, #GPIO_ODR]
	BIC r1, r1, #(1<<A_PIN)
	BIC r1, r1, #(1<<NOT_A_PIN)
	BIC r1, r1, #(1<<B_PIN)
	BIC r1, r1, #(1<<NOT_B_PIN)		;Clearing ODR
	
	ORR r1, r1, #(1<<NOT_A_PIN)
	ORR r1, r1, #(1<<NOT_B_PIN)
	STR r1, [r0, #GPIO_ODR]			;Phase 04 (Setting Anot and Bnot to high, else to low)
	
	MOV r0, #DLY
	BL delay						;Delay func
	
	POP {PC}
	ENDP

full_step_ccw PROC
	PUSH {LR}
	
	LDR r0, =GPIOC_BASE
	LDR r1, [r0, #GPIO_ODR]
	BIC r1, r1, #(1<<A_PIN)
	BIC r1, r1, #(1<<NOT_A_PIN)
	BIC r1, r1, #(1<<B_PIN)
	BIC r1, r1, #(1<<NOT_B_PIN)		;Clearing ODR
	
	ORR r1, r1, #(1<<NOT_A_PIN)
	ORR r1, r1, #(1<<NOT_B_PIN)
	STR r1, [r0, #GPIO_ODR]			;Phase 04 (Setting Anot and Bnot to high, else to low)
	
	MOV r0, #DLY
	BL delay						;Delay func
	
	LDR r0, =GPIOC_BASE
	LDR r1, [r0, #GPIO_ODR]
	BIC r1, r1, #(1<<A_PIN)
	BIC r1, r1, #(1<<NOT_A_PIN)
	BIC r1, r1, #(1<<B_PIN)
	BIC r1, r1, #(1<<NOT_B_PIN)		;Clearing ODR
	
	ORR r1, r1, #(1<<NOT_A_PIN)
	ORR r1, r1, #(1<<B_PIN)
	STR r1, [r0, #GPIO_ODR]			;Phase 03 (Setting Anot and B to high, else to low)
	
	MOV r0, #DLY
	BL delay						;Delay func
	
	LDR r0, =GPIOC_BASE
	LDR r1, [r0, #GPIO_ODR]
	BIC r1, r1, #(1<<A_PIN)
	BIC r1, r1, #(1<<NOT_A_PIN)
	BIC r1, r1, #(1<<B_PIN)
	BIC r1, r1, #(1<<NOT_B_PIN)		;Clearing ODR
	
	ORR r1, r1, #(1<<A_PIN)
	ORR r1, r1, #(1<<B_PIN)
	STR r1, [r0, #GPIO_ODR]			;Phase 02 (Setting A and B to high, else to low)
	
	MOV r0, #DLY
	BL delay						;Delay func
	
	LDR r0, =GPIOC_BASE
	LDR r1, [r0, #GPIO_ODR]
	BIC r1, r1, #(1<<A_PIN)
	BIC r1, r1, #(1<<NOT_A_PIN)
	BIC r1, r1, #(1<<B_PIN)
	BIC r1, r1, #(1<<NOT_B_PIN)		;Clearing ODR
	
	ORR r1, r1, #(1<<A_PIN)
	ORR r1, r1, #(1<<NOT_B_PIN)			
	STR r1, [r0, #GPIO_ODR]			;Phase 01 (Setting A and Bnot to high, else to low)
	
	MOV r0, #DLY
	BL delay						;Delay func
	
	POP {PC}
	ENDP


half_step PROC
	PUSH {LR}	
	
	LDR r0, =GPIOC_BASE
	LDR r1, [r0, #GPIO_ODR]
	BIC r1, r1, #(1<<A_PIN)
	BIC r1, r1, #(1<<NOT_A_PIN)
	BIC r1, r1, #(1<<B_PIN)
	BIC r1, r1, #(1<<NOT_B_PIN)		;Clearing ODR
	
	ORR r1, r1, #(1<<A_PIN)
	ORR r1, r1, #(1<<NOT_B_PIN)			
	STR r1, [r0, #GPIO_ODR]			;Phase 01 (Setting A and Bnot to high, else to low)
	
	MOV r0, #DLY
	BL delay						;Delay func
	
	LDR r0, =GPIOC_BASE
	BIC r1, r1, #(1<<NOT_B_PIN)		;Turning Bnot off
	STR r1, [r0, #GPIO_ODR]			;Phase 02 (Setting A to high, else to low)
	
	MOV r0, #DLY
	BL delay						;Delay func
	
	LDR r0, =GPIOC_BASE
	ORR r1, r1, #(1<<B_PIN)			;Turning B on	
	STR r1, [r0, #GPIO_ODR]			;Phase 03 (Setting A and B to high, else to low)
	
	MOV r0, #DLY
	BL delay						;Delay func
	
	LDR r0, =GPIOC_BASE
	BIC r1, r1, #(1<<A_PIN)			;Turning A off
	STR r1, [r0, #GPIO_ODR]			;Phase 04 (B to high, else to low)
	
	MOV r0, #DLY
	BL delay						;Delay func
	
	LDR r0, =GPIOC_BASE
	ORR r1, r1, #(1<<NOT_A_PIN)		;Turn Anot on	
	STR r1, [r0, #GPIO_ODR]			;Phase 05 (B and Anot to high, else to low)
	
	MOV r0, #DLY
	BL delay						;Delay func
	
	LDR r0, =GPIOC_BASE
	BIC r1, r1, #(1<<B_PIN)			;Turn B off
	STR r1, [r0, #GPIO_ODR]			;Phase 06 (Anot to high, else to low)

	MOV r0, #DLY
	BL delay						;Delay func
	
	LDR r0, =GPIOC_BASE
	ORR r1, r1, #(1<<NOT_B_PIN)		;Turning Bnot on
	STR r1, [r0, #GPIO_ODR]			;Phase 07 (Bnot and Anot to high, else to low)
	
	MOV r0, #DLY
	BL delay						;Delay func
	
	LDR r0, =GPIOC_BASE
	BIC r1, r1, #(1<<NOT_A_PIN)		;Turning Anot off
	STR r1, [r0, #GPIO_ODR]			;Phase 08 (Bnot to high, else to low)
	
	MOV r0, #DLY
	BL delay						;Delay func
	
	POP {PC}
	ENDP
		
		
	ALIGN			

	AREA    myData, DATA, READWRITE
	ALIGN
array	DCD   1, 2, 3, 4
	END
		
		
		