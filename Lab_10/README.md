##  Lab Report ##

Your Name: 
-----------


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
Set GPIOA->MODER 
MASK MODER = 0xFFFFFF3F
VALUE MODER = 0x0000000C
Set GPIOA->ASCR for pin 1
MASK ASCR = 0x00000002 
VALUE ASCR = 0x00000002

* Question 2:
1. How many volts does each bit increment in the value read from the ADC represent?
Voltage increment per Bit = (refV/(2^bits)) 
= (3/4096)V

2. If you want to check if the input voltage exceeds 2V, what value from the ADC would
you compare against?
((Resolution of ADC)/(System Voltage))*((ADC Reading)/(Analog Voltage Measured))
=2730
We would compare againts 2730

* Question 3:


Answers to After-lab Questions (if applicable)
-------

* Question 1:


* Question 2:


* Question 3:


Suggestions and Comments
-------

* Do you have comments or suggestions for the lab and course?


* Do you have comments or suggestions for the textbook and lab handouts? Any typos or errors?



Lab Credits
-------
Did you received any help from someone other than the instructor and lab teaching assistants?
- Yes [ ]
- No []

If yes, please give a short explanation.

