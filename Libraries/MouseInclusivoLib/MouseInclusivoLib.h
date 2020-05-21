#ifndef MOUSEINCLUSIVOLIB_H		
#define	MOUSEINCLUSIVOLIB_H

#include <Arduino.h>

#include "UsbMultiHID.h" // Biblioteca USB

typedef enum { 
click = 0, 
double_click, 
right_click,
drag,
} Mouse_click;


void cleanKeyBuffer(byte key_cursor);
void pressKey(byte key);
void pressKey2(byte key, byte modifiers, byte key_cursor);
void Key_program();
void click(Mouse_click);


#endif