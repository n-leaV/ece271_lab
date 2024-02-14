#ifndef KEYPAD_H
#define KEYPAD_H
#include "stm32l476xx.h"


unsigned char keypad_scan();
void keypad_pin_init();

#endif