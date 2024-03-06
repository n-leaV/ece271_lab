##  Lab Report ##

Your Name: Nick Leavitt
-----------
I say hello from a new branch

Lab Status
-------
Answer the following questions by put a "x" in the bracket.
- Yes [x], No []: Complete pre-lab
- Yes [], No []: Complete basic lab requirements
- Yes [], No []: Complete something cool
  - If you have completed something cool, give it a short summary below: 


Answers to Pre-lab Questions (if applicable)
-------
This part is required only if you are requested to write your answers here. 

* Question 1:
  Polling checks for an input every few cycles, whereas the interrupt waits for the hardware to produce an input then runs the specified code. The interrupots are more efficient as it doesnt waste clock cycles checking for an input, the response time is based off the NVIC, and is generally better too.

* Question 2:
  Determine which EXTI Line is PC13 - EXTI Line 13
  Find NVIC register responsible for that
  Set corresponding bit, enabling interrupts

  Pin 13 must be input, as it's checking for external processes.
  EXTI Line 13 must correspond to PC13 - need to write to EXTI registers
  Enable EXTI interrupt handling - setting bit in EXTI_IMR  


* Question 3:
  EXTI15_10_IRQHandler

* Question 4:
  Enable GPIO Port C
  PC13 input pin
  Enable SYSCFG clock
  EXTI Line -> Pin 13
  Set interrupt trigger to type of falling edge
  Enable interrupt in NVIC

* Question 5:
  100100
  Pin 13 and Falling edge are 1, everything else 0.

Answers to After-lab Questions (if applicable)
-------

* Question 1:
  You can clear a pending request by writing a 1 to the register.

* Question 2:
  If you don't clear the interrupt flag, the computer will believe that it still has an interrupt to run, and you will never exit your interrupt code.

* Question 3:
  It is possible to trigger an EXTI programmatically. This is usefule for exception Handling. Exception handling is when there are abnormal situations that we dont want to run, like dividing by zero, or running an illegal opcode.It can also be used for Privilidged hardware access, which can allow for unprivilidged code to run otherwise inaccessable instructions.

Suggestions and Comments
-------

* Do you have comments or suggestions for the lab and course?


* Do you have comments or suggestions for the textbook and lab handouts? Any typos or errors?



Lab Credits
-------
Did you received any help from someone other than the instructor and lab teaching assistants?
- Yes [x]
- No []

If yes, please give a short explanation.
I worked through the pre lab with Swaroop Handral. Owen and Atticus helped me debug my main code.
