#include "MouseInclusivoLib.h"


void mouse_initialize() {
	//it should read every EEPROM memory changes, like this from the old code:
	/*
	if (EEPROM.read(5)==1) { 
		BUTTON_LEFT = EEPROM.read(0);
		BUTTON_2CLICK = EEPROM.read(1); 
		BUTTON_RIGHT = EEPROM.read(2); 
		BUTTON_DRAG = EEPROM.read(3); 
	}
	*/
}

void mouse_update() {
	//every cicle of loop, it needs to be called
	usbMultiHID.update();
}

void cleanKeyBuffer(byte key_cursor) {
  //Limpa buffer de teclas
  memset( keypresses, 0, KEYPRESS_BUFFER_LENGTH );
  key_cursor = 1; // ver se essa linha é necessaria mesmo
}

void pressKey(byte key) {
	pressKey2(key, 0,1);
}

void pressKey2(byte key, byte modifiers, byte key_cursor) {
	// Monta, bit a bit do byte 0, com os modificadores passados
	key_cursor = 1;
	if (modifiers != 0) keypresses[0] |= modifiers;
	// Monta, byte a byte, apartir do byte 1 até o limite de 6, o buffer de teclas
	if ((key != 0) && (key_cursor < KEYPRESS_BUFFER_LENGTH)) {
		keypresses[key_cursor++] = key;
	}
}

//inside the library, it is supposed to receive every button
//that the user want to program.
void Key_program(byte aux_1_prog, byte aux_2_prog, byte aux_3_prog, byte aux_4_prog) {
	// configura botão LEFT
	usbMultiHID.update();
	Blink();
  
	do{
		while(digitalRead(aux_1_prog)==LOW){ 
			BUTTON_LEFT = aux_1_prog;
			aux_1_prog=(byte)RESET;
			delay_and_press();
			while(digitalRead(aux_1_prog)==LOW) {}
		} 
		while(digitalRead(aux_2_prog)==LOW){ 
			BUTTON_LEFT = aux_2_prog;
			aux_2_prog=(byte)RESET;
			delay_and_press();
			while(digitalRead(aux_2_prog)==LOW){}
		}
		while(digitalRead(aux_3_prog)==LOW){ 
			BUTTON_LEFT = aux_3_prog;
			aux_3_prog=(byte)RESET;
			delay_and_press();
			while(digitalRead(aux_3_prog)==LOW) {}
		}
		while(digitalRead(aux_4_prog)==LOW){ 
			BUTTON_LEFT = aux_4_prog;
			aux_4_prog=(byte)RESET;
			delay_and_press();
			while(digitalRead(aux_4_prog)==LOW) {}
		} 
		usbMultiHID.update();
	}while(prog_press == false);

    prog_press = false;
    
    // configura botão 2CLICK
    Blink();
    do{
		while(digitalRead(aux_1_prog)==LOW){ 
			BUTTON_2CLICK = aux_1_prog;
			aux_1_prog=(byte)RESET;
			delay_and_press();
			while(digitalRead(aux_1_prog)==LOW) {}
		} 
		while(digitalRead(aux_2_prog)==LOW){ 
			BUTTON_2CLICK = aux_2_prog;
			aux_2_prog=(byte)RESET;
			delay_and_press();
			while(digitalRead(aux_2_prog)==LOW){}
		}
		while(digitalRead(aux_3_prog)==LOW){ 
			BUTTON_2CLICK = aux_3_prog;
			aux_3_prog=(byte)RESET;
			delay_and_press();
			while(digitalRead(aux_3_prog)==LOW) {}
		}
		while(digitalRead(aux_4_prog)==LOW){ 
			BUTTON_2CLICK = aux_4_prog;
			aux_4_prog=(byte)RESET;
			delay_and_press();
			while(digitalRead(aux_4_prog)==LOW) {}
		} 
		usbMultiHID.update();
    }while(prog_press == false);

    prog_press = false;
  
    // configura botão RIGHT
    Blink();
    do{
		while(digitalRead(aux_1_prog)==LOW){ 
			BUTTON_RIGHT = aux_1_prog;
			aux_1_prog=(byte)RESET;
			delay_and_press();
			while(digitalRead(aux_1_prog)==LOW) {}
		} 
		while(digitalRead(aux_2_prog)==LOW){ 
			BUTTON_RIGHT = aux_2_prog;
			aux_2_prog=(byte)RESET;
			delay_and_press();
			while(digitalRead(aux_2_prog)==LOW){}
		}
		while(digitalRead(aux_3_prog)==LOW){ 
			BUTTON_RIGHT = aux_3_prog;
			aux_3_prog=(byte)RESET;
			delay_and_press();
			while(digitalRead(aux_3_prog)==LOW) {}
		}
		while(digitalRead(aux_4_prog)==LOW){ 
			BUTTON_RIGHT = aux_4_prog;
			aux_4_prog=(byte)RESET;
			delay_and_press();
			while(digitalRead(aux_4_prog)==LOW) {}
		}             
		usbMultiHID.update();
    }while(prog_press == false);

    prog_press = false;
     
    // configura botão DRAG
    Blink();
    do{
		while(digitalRead(aux_1_prog)==LOW){ 
			BUTTON_DRAG = aux_1_prog;
			aux_1_prog=(byte)RESET;
			delay_and_press();
			while(digitalRead(aux_1_prog)==LOW) {}
		} 
		while(digitalRead(aux_2_prog)==LOW){ 
			BUTTON_DRAG = aux_2_prog;
			aux_2_prog=(byte)RESET;
			delay_and_press();
			while(digitalRead(aux_2_prog)==LOW){}
		}
		while(digitalRead(aux_3_prog)==LOW){ 
			BUTTON_DRAG = aux_3_prog;
			aux_3_prog=(byte)RESET;
			delay_and_press();
			while(digitalRead(aux_3_prog)==LOW) {}
		}
		while (digitalRead(aux_4_prog)==LOW){ 
			BUTTON_DRAG = aux_4_prog;
			aux_4_prog=(byte)RESET;
			delay_and_press();
			while(digitalRead(aux_4_prog)==LOW) {}
		} 
		usbMultiHID.update();
    }while(prog_press == false);
    
    Blink();
        
    EEPROM.write(0, BUTTON_LEFT);
    EEPROM.write(1, BUTTON_2CLICK); 
    EEPROM.write(2, BUTTON_RIGHT);
    EEPROM.write(3, BUTTON_DRAG);  
    EEPROM.write(5, 1);
} //End of Key_program

void click(Mouse_click mouse_click) {	
	if(mouse_click == click){
		mouse_buttons |= 0x01;
	}else if(right_click_action){
		mouse_buttons |= 0x01 << 1;
	}else if(drag_click_action){ //usb drag_click communication
		mouse_buttons |= 0x01;
	}else if(double_click_action){
		//usbMultiHID.update();
		mouse_buttons=0;
		usbMultiHID.send_mouse_report(mouse_buttons, mouse_x, mouse_y, 0);
		mouse_buttons |= 0x01;
		delayMs(70);
		usbMultiHID.send_mouse_report(mouse_buttons, mouse_x, mouse_y, 0);
		mouse_buttons=0;
		usbMultiHID.send_mouse_report(mouse_buttons, mouse_x, mouse_y, 0);
		mouse_buttons |= 0x01;
		delayMs(70);
		//usbMultiHID.send_mouse_report(mouse_buttons, mouse_x, mouse_y, 0);       
	}
		  
	usbMultiHID.send_mouse_report(mouse_buttons, mouse_x, mouse_y, 0);
}

void keyboard_write(byte key) {
	cleanKeyBuffer(1);// no caso de apertar duas teclas ao mesmo tempo, é necessário?
	pressKey(key);
	usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);
}

