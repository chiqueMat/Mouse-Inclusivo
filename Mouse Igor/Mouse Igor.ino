/*------------------------------------------------------------------------------------------------------
  | Software Mouse Inclusivo Igor - Assistiva | Tecnologia para a Inclusão Social - Versão 2018/2      |                                  
  | Contém 8 botões                                                                                    |
  | Primeiro botão pré-definido: LEFT                                                                  |
  | Segundo botão pré-definido: 2CLICK                                                                 |
  | Terceiro botão pré-definido: RIGHT                                                                 |
  | Quarto botão pré-definido: DRAG                                                                    |
  | Todos os botões podem ser reconfigarado da forma que o usuário quiser                              |
  ------------------------------------------------------------------------------------------------------*/

#include <EEPROM.h>   
#include "UsbMultiHID.h" // Biblioteca USB
                
// Definições de botões e controles

#define MV_LEFT 13
#define MV_RIGHT 11
#define MV_UP 10
#define MV_DOWN 12

#define RESET 9

#define MODE_REFFERENCE_LED_1 3
#define MODE_REFFERENCE_LED_2 A5
#define MODE_REFFERENCE_LED_3 A4

#define REPROG_AUX_PIN A3


#define PRECISION 2
#define TIME_DELAY 20
#define PAUSE 500
#define HALF_PAUSE 250
#define LITTLE_PAUSE 125

#define BYPASS_TIMER_ISR 1

#define combination_01 0x0101 // 0000 0001 0000 0001
#define combination_02 0x0201 // 0000 0010 0000 0001
#define combination_03 0x0202 // 0000 0010 0000 0010
#define combination_04 0x0102 // 0000 0001 0000 0010
#define combination_05 0x0402 // 0000 0100 0000 0010
#define combination_06 0x0404 // 0000 0100 0000 0100
#define combination_07 0x0204 // 0000 0010 0000 0100
#define combination_08 0x0804 // 0000 1000 0000 0100
#define combination_09 0x0808 // 0000 1000 0000 1000
#define combination_10 0x0408 // 0000 0100 0000 1000
#define combination_11 0x1008 // 0001 0000 0000 1000
#define combination_12 0x1010 // 0001 0000 0001 0000
#define combination_13 0x0810 // 0000 1000 0001 0000
#define combination_14 0x2010 // 0010 0000 0001 0000
#define combination_15 0x2020 // 0010 0000 0010 0000
#define combination_16 0x1020 // 0001 0000 0010 0000 
#define combination_17 0x0401 // 0000 0100 0000 0001
#define combination_18 0x0104 // 0000 0001 0000 0100
#define combination_19 0x0802 // 0000 1000 0000 0010
#define combination_20 0x0208 // 0000 0010 0000 1000
#define combination_21 0x1004 // 0001 0000 0000 0100
#define combination_22 0x0410 // 0000 0100 0001 0000
#define combination_23 0x2008 // 0010 0000 0000 1000
#define combination_24 0x0820 // 0000 1000 0010 0000
#define combination_25 0x0801 // 0000 1000 0000 0001
#define combination_26 0x0108 // 0000 0001 0000 1000
#define combination_27 0x1002 // 0001 0000 0000 0010
#define combination_28 0x0210 // 0000 0010 0001 0000
#define combination_29 0x2004 // 0010 0000 0000 0100
#define combination_30 0x0420 // 0000 0100 0010 0000
#define combination_31 0x1001 // 0001 0000 0000 0001
#define combination_32 0x0110 // 0000 0001 0001 0000
#define combination_33 0x2002 // 0010 0000 0000 0010
#define combination_34 0x0220 // 0000 0010 0010 0000
#define combination_35 0x8101 // 1000 0001 0000 0001
#define combination_36 0x8201 // 1000 0010 0000 0001
#define combination_37 0x8202 // 1000 0010 0000 0010
#define combination_38 0x8102 // 1000 0001 0000 0010
#define combination_39 0x8402 // 1000 0100 0000 0010
#define combination_40 0x8404 // 1000 0100 0000 0100
#define combination_41 0x8204 // 1000 0010 0000 0100
#define combination_42 0x8804 // 1000 1000 0000 0100
#define combination_43 0x8808 // 1000 1000 0000 1000
#define combination_44 0x8408 // 1000 0100 0000 1000
#define combination_45 0x9008 // 1001 0000 0000 1000
#define combination_46 0x9010 // 1001 0000 0001 0000
#define combination_47 0x8810 // 1000 1000 0001 0000
#define combination_48 0xa010 // 1010 0000 0001 0000
#define combination_49 0xa020 // 1010 0000 0010 0000
#define combination_50 0x9020 // 1001 0000 0010 0000
#define combination_51 0x8401 // 1000 0100 0000 0001
#define combination_52 0x8104 // 1000 0001 0000 0100
#define combination_53 0x8802 // 1000 1000 0000 0010
#define combination_54 0x8208 // 1000 0010 0000 1000
#define combination_55 0x9004 // 1001 0000 0000 0100
#define combination_56 0x8410 // 1000 0100 0001 0000
#define combination_57 0x8820 // 1000 1000 0010 0000
#define combination_58 0x8801 // 1000 1000 0000 0001
 
byte BUTTON_LEFT =   7;
byte BUTTON_2CLICK = 8;
byte BUTTON_RIGHT =  6;
byte BUTTON_DRAG =   5;

boolean led_flag=false;

//modos de operação:
boolean mode_mouse = true;// original = true
boolean mode_scroll = false;// original = false
boolean mode_keyboard = false;// original = false
boolean mode_fliperama = false;// original = false
boolean mode_velocidade = false;// original = false
boolean mode_reprog = false;// original = false

boolean led_teste=true;

//flags auxiliares para modo de operação:
boolean press_mode_verify = false;// original = false
boolean press_mode_mouse = false;// original = false
boolean press_mode_scroll = false;// original = false
boolean press_mode_keyboard = false;// original = false
boolean press_mode_fliperama = false;// original = false
boolean press_mode_velocidade = false;// original = false
boolean press_mode_reprog = false;// original = false
byte pressed_mode_button;
boolean reset_button=false;
boolean pressed_reset_button=false;
boolean pressed_shift_condition=false;
boolean shift=false;

//keyboard auxiliar variables:
int mouse_pins[8]={7,8,11,10,12,13,6,5};
uint16_t keyboard_buttons=0;
byte count_verify_buttons=0;
byte bit_0_8_keyboard_buttons=0;
boolean bit_0_8_keyboard_buttons_flag=true;//true to bit 0 and false to bit 8
byte first_pressed_button=0;
byte second_pressed_button=0;
boolean pressed_button_flag=false;
boolean pressed_second_combination_condition=false;
boolean second_combination=false;

boolean mode_flag = true;// original = true
boolean bt_drag = false;// original = false
boolean prog_press = false;// original = false
  
byte keypresses[KEYPRESS_BUFFER_LENGTH];
byte key_cursor = 1;

boolean left_click_press=false;//flag that indicate that capacitive button is pressed in
boolean right_click_press=false;//flag that indicate that capacitive button is pressed in
boolean drag_click_switch=false;//auxiliar flag for drag button, drag button logic is like to a light switch
boolean drag_click_action=false;//auxiliar flag for drag button, drag button logic is like to a light switch
boolean double_click_press=false;//flag that indicate that capacitive button is pressed in

boolean press_add_velocity=false;
boolean press_less_velocity=false;
byte travel;

boolean press_up_mouse_wheel=false;
boolean press_down_mouse_wheel=false;

boolean press_mouse_wheel=false;

boolean press_velocity=false;

//----- Início - Implementação alternativa para o Delay

#if BYPASS_TIMER_ISR
void delayMs(unsigned int ms) {
	for (int i = 0; i < ms; i++) {
		delayMicroseconds(1000);
	}
}
#endif

//----- Fim - Implementação alternativa para o Delay

void Blink(){
	for (int i=0; i <= 5; i++){
		digitalWrite(MODE_REFFERENCE_LED_1, HIGH);
		delayMs(LITTLE_PAUSE);
		digitalWrite(MODE_REFFERENCE_LED_1, LOW);
		delayMs(50);
	} 
}

void cleanKeyBuffer(byte key_cursor){
	//Limpa buffer de teclas
	memset( keypresses, 0, KEYPRESS_BUFFER_LENGTH );
	key_cursor = 1; // ver se essa linha é necessaria mesmo
} 

void pressKey(byte key){
	pressKey2(key, 0);
}

void pressKey2(byte key, byte modifiers){
	// Monta, bit a bit do byte 0, com os modificadores passados
	key_cursor = 1;
	if (modifiers != 0) keypresses[0] |= modifiers;
	
	// Monta, byte a byte, apartir do byte 1 até o limite de 6, o buffer de teclas
	if ((key != 0) && (key_cursor < KEYPRESS_BUFFER_LENGTH)) {
		keypresses[key_cursor++] = key;
	}
}

void delay_and_press(){
	delayMs(PAUSE);
	prog_press = true;
}

void verify_mode(){
	if(!pressed_reset_button){
		if(!press_mode_verify){
			if(digitalRead(RESET)==LOW){
				delay(25);
				if(digitalRead(RESET)==LOW){
					mode_mouse=false;
					mode_scroll=false;
					mode_keyboard=false;
					mode_fliperama=false;
					mode_velocidade=false;
					mode_reprog=false;
				  
					digitalWrite(MODE_REFFERENCE_LED_1, HIGH);
					digitalWrite(MODE_REFFERENCE_LED_2, HIGH);
					digitalWrite(MODE_REFFERENCE_LED_3, HIGH);
				  
					pressed_reset_button=true;
				}
			}
		}
	}
	if(pressed_reset_button){
		if(digitalRead(RESET)==HIGH){
			if(digitalRead(mouse_pins[0])==LOW){
				delay(25);
				if(digitalRead(mouse_pins[0])==LOW){
					Blink();
					press_mode_verify=true;
					press_mode_mouse=true;
					pressed_mode_button=0;
				}
			}
			if(digitalRead(mouse_pins[1])==LOW){
				delay(25);
				if(digitalRead(mouse_pins[1])==LOW){
					Blink();
					press_mode_verify=true;
					press_mode_scroll=true;
					pressed_mode_button=1;
				}
			}
			if(digitalRead(mouse_pins[2])==LOW){
				delay(25);
				if(digitalRead(mouse_pins[2])==LOW){
					Blink();
					press_mode_verify=true;
					press_mode_keyboard=true;
					pressed_mode_button=2;
				}
			}
			if(digitalRead(mouse_pins[3])==LOW){
				delay(25);
				if(digitalRead(mouse_pins[3])==LOW){
					Blink();
					press_mode_verify=true;
					press_mode_fliperama=true;
					pressed_mode_button=3;
				}
			}
			if(digitalRead(mouse_pins[4])==LOW){
				delay(25);
				if(digitalRead(mouse_pins[4])==LOW){
					Blink();
					press_mode_verify=true;
					press_mode_velocidade=true;
					pressed_mode_button=4;
				}
			}
			if(digitalRead(mouse_pins[5])==LOW){
				delay(25);
				if(digitalRead(mouse_pins[5])==LOW){
					Blink();
					press_mode_verify=true;
					press_mode_reprog=true;
					pressed_mode_button=5;
				}
			}
		}
	}
    if(press_mode_verify){
		if(digitalRead(mouse_pins[pressed_mode_button])==HIGH){
			press_mode_verify=false;
			pressed_reset_button=false;
			delay(150);
			if(press_mode_mouse){
				mode_mouse=true;
				delay(50);
				Blink();
				digitalWrite(MODE_REFFERENCE_LED_1, LOW);
				digitalWrite(MODE_REFFERENCE_LED_2, LOW);
				digitalWrite(MODE_REFFERENCE_LED_3, LOW);
			}
			if(press_mode_scroll){
				mode_scroll=true;
				delay(50);
				Blink();
				digitalWrite(MODE_REFFERENCE_LED_1, HIGH);
				digitalWrite(MODE_REFFERENCE_LED_2, LOW);
				digitalWrite(MODE_REFFERENCE_LED_3, LOW);
			}
			if(press_mode_keyboard){
				mode_keyboard=true;
				delay(50);
				Blink();
				digitalWrite(MODE_REFFERENCE_LED_1, LOW);
				digitalWrite(MODE_REFFERENCE_LED_2, HIGH);
				digitalWrite(MODE_REFFERENCE_LED_3, LOW);
			}
			if(press_mode_fliperama){
				mode_fliperama=true;
				delay(50);
				Blink();
				digitalWrite(MODE_REFFERENCE_LED_1, HIGH);
				digitalWrite(MODE_REFFERENCE_LED_2, HIGH);
				digitalWrite(MODE_REFFERENCE_LED_3, LOW);
			}
			if(press_mode_velocidade){
				mode_velocidade=true;
				delay(50);
				Blink();
				digitalWrite(MODE_REFFERENCE_LED_1, LOW);
				digitalWrite(MODE_REFFERENCE_LED_2, LOW);
				digitalWrite(MODE_REFFERENCE_LED_3, HIGH);
			}
			if(press_mode_reprog){
				mode_reprog=true;
				delay(50);
				Blink();
				digitalWrite(MODE_REFFERENCE_LED_1, HIGH);
				digitalWrite(MODE_REFFERENCE_LED_2, LOW);
				digitalWrite(MODE_REFFERENCE_LED_3, HIGH);
			}
			
			press_mode_mouse=false;
			press_mode_scroll=false;
			press_mode_keyboard=false;
			press_mode_fliperama=false;
			press_mode_velocidade=false;
			press_mode_reprog=false;
		}
    }
}

void verify_keyboard_buttons(){
    int i;

    if(pressed_second_combination_condition){
        if(digitalRead(mouse_pins[7])==HIGH){
			second_combination=!second_combination;
			pressed_second_combination_condition=false;
			pressed_button_flag=false;
		}
    }
    
    if(!pressed_button_flag){
        for(i=0;i<8;i++){
            if(digitalRead(mouse_pins[i])==LOW){
                delay(25);
                if(digitalRead(mouse_pins[i])==LOW){
                    if(bit_0_8_keyboard_buttons_flag){
                        if(i==7){//condition to set the 15th bit of keyboard_control
							pressed_second_combination_condition=true;
							bit_0_8_keyboard_buttons_flag=true;
                            goto second_combination_jump;
                        }
                        first_pressed_button=i;
                    }else{
						if(i==7){//condition to set the 15th bit of keyboard_control
							pressed_second_combination_condition=true;
							bit_0_8_keyboard_buttons_flag=true;
                            goto second_combination_jump;
                        }
                        second_pressed_button=i;
                    }
                    bit_0_8_keyboard_buttons_flag=!bit_0_8_keyboard_buttons_flag;
                    
                    
					second_combination_jump:
					
					pressed_button_flag=true;
                    break;//finish for()
                }
            }
        }
    }else{
		if(!pressed_second_combination_condition){
			if((digitalRead(mouse_pins[first_pressed_button])==HIGH) && (!bit_0_8_keyboard_buttons_flag)){
				pressed_button_flag=false;
			}
			if((digitalRead(mouse_pins[second_pressed_button])==HIGH) && (bit_0_8_keyboard_buttons_flag)){
				bitSet(keyboard_buttons,first_pressed_button);
				bitSet(keyboard_buttons,second_pressed_button+8);
				if(second_combination){
					bitSet(keyboard_buttons,15);
				}else{
					bitClear(keyboard_buttons,15);
				}
				pressed_button_flag=false;
			}
		}
    }
}

void keyboard_control(){
	switch(keyboard_buttons){
		case 0:
			cleanKeyBuffer(1);
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);
		break;
		
		case 0x8000:
			cleanKeyBuffer(1);
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);
		break;
		
		case combination_01:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_A,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_A);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);
		break;
		
		case combination_02:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_E,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_E);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);     
		break;

		case combination_03:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_O,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_O);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);      
		break;
		
		case combination_04:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_S,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_S);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);      
		break;
		
		case combination_05:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_R,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_R);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);  
		break;
		
		case combination_06:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_I,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_I);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);      
		break;
		
		case combination_07:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_N,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_N);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);  
		break;

		case combination_08:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_D,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_D);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);      
		break;
		
		case combination_09:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_M,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_M);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);  
		break;
		
		case combination_10:
			cleanKeyBuffer(1);
			pressKey(KEY_ENTER);
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);
		break;
		
		case combination_11:
			cleanKeyBuffer(1);
			pressKey(KEY_SPACE);
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);  
		break;
		
		case combination_12:
			cleanKeyBuffer(1);
			pressKey(KEY_DELETE);
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);     
		break;

		case combination_13:
			cleanKeyBuffer(1);
			pressKey(KEY_TAB);
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);    
		break;
		
		case combination_14:
			shift=!shift;     
		break;
		
		case combination_15:
			cleanKeyBuffer(1);
			pressKey(KEY_BACKSLASH);
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);      
		break;
		
		case combination_16:
			cleanKeyBuffer(1);
			pressKey(KEY_ESCAPE);
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);      
		break;
		
		case combination_17:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_U,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_U);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);      
		break;

		case combination_18:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_T,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_T);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);     
		break;
		
		case combination_19:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_C,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_C);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);      
		break;
		
		case combination_20:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_SEMICOLON,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_SEMICOLON);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);      
		break;
		
		case combination_21:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_L,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_L);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);    
		break;
		
		case combination_22:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_P,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_P);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);      
		break;

		case combination_23:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_V,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_V);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);     
		break;
		
		case combination_24:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_G,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_G);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);      
		break;
		
		case combination_25:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_H,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_H);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);     
		break;
		
		case combination_26:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_Q,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_Q);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);    
		break;
		
		case combination_27:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_B,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_B);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);      
		break;

		case combination_28:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_F,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_F);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);      
		break;
		
		case combination_29:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_Z,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_Z);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);      
		break;
		
		case combination_30:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_J,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_J);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);      
		break;
		
		case combination_31:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_X,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_X);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);     
		break;
		
		case combination_32:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_K,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_K);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);      
		break;

		case combination_33:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_W,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_W);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);    
		break;
		
		case combination_34:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_Y,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_Y);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);      
		break;
		
		case combination_35:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_PERIOD,MOD_SHIFT_LEFT);
			}else{
			pressKey(KEY_PERIOD);
		  }
		  usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);      
		break;
		
		case combination_36:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_COMMA,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_COMMA);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);      
		break;
		
		case combination_37:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_SLASH,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_SLASH);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);    
		break;
		
		case combination_38:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_LBRACKET,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_LBRACKET);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);    
		break;
		
		case combination_39:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_QUOTE,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_QUOTE);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);     
		break;
		
		case combination_40:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_MINUS,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_MINUS);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);      
		break;
		
		case combination_41:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_TILDE,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_TILDE);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);  
		break;

		case combination_42:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_1,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_1);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);      
		break;
		
		case combination_43:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_2,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_2);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);      
		break;
		
		case combination_44:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_3,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_3);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);  
		break;
		
		case combination_45:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_4,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_4);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);     
		break;
		
		case combination_46:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_5,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_5);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);     
		break;

		case combination_47:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_6,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_6);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);      
		break;
		
		case combination_48:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_7,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_7);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);      
		break;
		
		case combination_49:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_8,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_8);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);      
		break;
		
		case combination_50:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_9,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_9);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);     
		break;
		
		case combination_51:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_0,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_0);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);      
		break;

		case combination_52:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_A,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_A);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);      
		break;
		
		case combination_53:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_A,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_A);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);      
		break;
		
		case combination_54:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_A,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_A);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);      
		break;
		
		case combination_55:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_A,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_A);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);      
		break;
		
		case combination_56:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_A,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_A);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);      
		break;

		case combination_57:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_A,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_A);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);     
		break;
		
		case combination_58:
			cleanKeyBuffer(1);
			if(shift){
				pressKey2(KEY_A,MOD_SHIFT_LEFT);
			}else{
				pressKey(KEY_A);
			}
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);     
		break;
		
		default:
			cleanKeyBuffer(1);
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);
		break;
	}
}

void Key_program(){
	// configura botão LEFT
	byte aux_1_prog=7;
	byte aux_2_prog=8;
	byte aux_3_prog=6;
	byte aux_4_prog=5;
  
	usbMultiHID.update();
    Blink();
  
    do{
		while (digitalRead(aux_1_prog)==LOW){ 
			BUTTON_LEFT = aux_1_prog;
			aux_1_prog=(byte)REPROG_AUX_PIN;
			delay_and_press();
			while (digitalRead(aux_1_prog)==LOW) {}
		} 
		while (digitalRead(aux_2_prog)==LOW){ 
			BUTTON_LEFT = aux_2_prog;
			aux_2_prog=(byte)REPROG_AUX_PIN;
			delay_and_press();
			while (digitalRead(aux_2_prog)==LOW){}
		}
		while (digitalRead(aux_3_prog)==LOW){ 
			BUTTON_LEFT = aux_3_prog;
			aux_3_prog=(byte)REPROG_AUX_PIN;
			delay_and_press();
			while (digitalRead(aux_3_prog)==LOW) {}
		}
		while (digitalRead(aux_4_prog)==LOW){ 
			BUTTON_LEFT = aux_4_prog;
			aux_4_prog=(byte)REPROG_AUX_PIN;
			delay_and_press();
			while (digitalRead(aux_4_prog)==LOW) {}
		} 
		usbMultiHID.update();
    } 
    while(prog_press == false);

    prog_press = false;
    
    // configura botão 2CLICK
    Blink();
    do{
		while (digitalRead(aux_1_prog)==LOW){ 
			BUTTON_2CLICK = aux_1_prog;
			aux_1_prog=(byte)REPROG_AUX_PIN;
			delay_and_press();
			while (digitalRead(aux_1_prog)==LOW) {}
		} 
		while (digitalRead(aux_2_prog)==LOW){ 
			BUTTON_2CLICK = aux_2_prog;
			aux_2_prog=(byte)REPROG_AUX_PIN;
			delay_and_press();
			while (digitalRead(aux_2_prog)==LOW){}
		}
		while (digitalRead(aux_3_prog)==LOW){ 
			BUTTON_2CLICK = aux_3_prog;
			aux_3_prog=(byte)REPROG_AUX_PIN;
			delay_and_press();
			while (digitalRead(aux_3_prog)==LOW) {}
		}
		while (digitalRead(aux_4_prog)==LOW){ 
			BUTTON_2CLICK = aux_4_prog;
			aux_4_prog=(byte)REPROG_AUX_PIN;
			delay_and_press();
			while (digitalRead(aux_4_prog)==LOW) {}
		} 
		usbMultiHID.update();
    }while(prog_press == false);

    prog_press = false;
  
    // configura botão RIGHT
    Blink();
    do{
		while (digitalRead(aux_1_prog)==LOW){ 
			BUTTON_RIGHT = aux_1_prog;
			aux_1_prog=(byte)REPROG_AUX_PIN;
			delay_and_press();
			while (digitalRead(aux_1_prog)==LOW) {}
		} 
		while (digitalRead(aux_2_prog)==LOW){ 
			BUTTON_RIGHT = aux_2_prog;
			aux_2_prog=(byte)REPROG_AUX_PIN;
			delay_and_press();
			while (digitalRead(aux_2_prog)==LOW){}
		}
		while (digitalRead(aux_3_prog)==LOW){ 
			BUTTON_RIGHT = aux_3_prog;
			aux_3_prog=(byte)REPROG_AUX_PIN;
			delay_and_press();
			while (digitalRead(aux_3_prog)==LOW) {}
		}
		while (digitalRead(aux_4_prog)==LOW){ 
			BUTTON_RIGHT = aux_4_prog;
			aux_4_prog=(byte)REPROG_AUX_PIN;
			delay_and_press();
			while (digitalRead(aux_4_prog)==LOW) {}
		}             
		usbMultiHID.update();
    }while(prog_press == false);

    prog_press = false;
     
    // configura botão DRAG
    Blink();
    do{
		while (digitalRead(aux_1_prog)==LOW){ 
			BUTTON_DRAG = aux_1_prog;
			aux_1_prog=(byte)REPROG_AUX_PIN;
			delay_and_press();
			while (digitalRead(aux_1_prog)==LOW) {}
		} 
		while (digitalRead(aux_2_prog)==LOW){ 
			BUTTON_DRAG = aux_2_prog;
			aux_2_prog=(byte)REPROG_AUX_PIN;
			delay_and_press();
			while (digitalRead(aux_2_prog)==LOW){}
		}
		while (digitalRead(aux_3_prog)==LOW){ 
			BUTTON_DRAG = aux_3_prog;
			aux_3_prog=(byte)REPROG_AUX_PIN;
			delay_and_press();
			while (digitalRead(aux_3_prog)==LOW) {}
		}
		while (digitalRead(aux_4_prog)==LOW){ 
			BUTTON_DRAG = aux_4_prog;
			aux_4_prog=(byte)REPROG_AUX_PIN;
			delay_and_press();
			while (digitalRead(aux_4_prog)==LOW) {}
		} 
		usbMultiHID.update();
    }while(prog_press == false);
  
    prog_press = false;
	mode_reprog=false;
	mode_mouse=true;
    
    Blink();
        
    EEPROM.write(0, BUTTON_LEFT);
    EEPROM.write(1, BUTTON_2CLICK); 
    EEPROM.write(2, BUTTON_RIGHT);
    EEPROM.write(3, BUTTON_DRAG);  
    EEPROM.write(5, 1);
    
	digitalWrite(MODE_REFFERENCE_LED_1, LOW);
	digitalWrite(MODE_REFFERENCE_LED_2, LOW);
	digitalWrite(MODE_REFFERENCE_LED_3, LOW);
} //End of Key_program

void setup(){
	if(EEPROM.read(5)==1){ 
		BUTTON_LEFT = EEPROM.read(0);
		BUTTON_2CLICK = EEPROM.read(1); 
		BUTTON_RIGHT = EEPROM.read(2); 
		BUTTON_DRAG = EEPROM.read(3); 
		travel = EEPROM.read(4); 
	}
  
	pinMode(mouse_pins[0],INPUT_PULLUP);
	pinMode(mouse_pins[1],INPUT_PULLUP);
	pinMode(mouse_pins[2],INPUT_PULLUP);
	pinMode(mouse_pins[3],INPUT_PULLUP);
	pinMode(mouse_pins[4],INPUT_PULLUP);
	pinMode(mouse_pins[5],INPUT_PULLUP);
	pinMode(mouse_pins[6],INPUT_PULLUP);
	pinMode(mouse_pins[7],INPUT_PULLUP);
	pinMode(RESET,INPUT_PULLUP);
	pinMode(REPROG_AUX_PIN,INPUT_PULLUP);
	
	pinMode(MODE_REFFERENCE_LED_1, OUTPUT);
	digitalWrite(MODE_REFFERENCE_LED_1, LOW);
	pinMode(MODE_REFFERENCE_LED_2, OUTPUT);
	digitalWrite(MODE_REFFERENCE_LED_2, LOW);
	pinMode(MODE_REFFERENCE_LED_3, OUTPUT);
	digitalWrite(MODE_REFFERENCE_LED_3, LOW);
    
	// Definição para uso de alternativa para o Delay
	#if BYPASS_TIMER_ISR
		// disable timer 0 overflow interrupt (used for millis)
		TIMSK0&=!(1<<TOIE0); // ++
	#endif
} //End of setup

void loop(){
  
    boolean left_click_action=false; 
	boolean right_click_action=false;
	boolean double_click_action=false;
	boolean press_key=false;
    short mouse_x = 0;
    short mouse_y = 0;
    byte count = 0;
    byte mouse_buttons = 0;
    byte hx = 0;
    byte hy = 0;
    byte prog_time = 0;
	byte mouse_wheel = 0;

    //digitalWrite(MODE_REFFERENCE_LED_3,led_teste);
    //led_teste=!led_teste;   
    
    #if BYPASS_TIMER_ISR  // check if timer isr fixed.
    delayMs(TIME_DELAY);   // val = TIME_DELAY 
    #else
    delay(TIME_DELAY);
    #endif  
    
	usbMultiHID.update();
	
	verify_mode();
  
	if(mode_mouse){
        if(digitalRead(MV_LEFT)==LOW){
			hx=0;
		}
        if(digitalRead(MV_RIGHT)==LOW){
			hx=20;
		}
        if(digitalRead(MV_UP)==LOW){
			hy=0;
		}
        if(digitalRead(MV_DOWN)==LOW){
			hy=20;
		}
         
        if((digitalRead(MV_LEFT)==HIGH) && (digitalRead(MV_RIGHT)==HIGH)){
			hx=10;
		} 
        if((digitalRead(MV_UP)==HIGH) && (digitalRead(MV_DOWN)==HIGH)){
			hy=10;
		} 
        
        mouse_x = hx;
        mouse_x = constrain(map(mouse_x, 0, 20, -travel, +travel), -travel, +travel);
        mouse_x = (mouse_x/PRECISION)*PRECISION;
        
        mouse_y = hy;
        mouse_y = constrain(map(mouse_y, 20, 0, -travel, +travel), -travel, +travel);
        mouse_y = (mouse_y/PRECISION)*PRECISION;
        
        // *************************************************************
        // Leitura dos botões e montagem do byte para o buffer do mouse
        // *************************************************************
      
        if(digitalRead(BUTTON_LEFT)==LOW){
			if(!left_click_press){
				left_click_action=true;  
				left_click_press=true;
			}
		}else{
			left_click_press=false;
		}
        
        if(digitalRead(BUTTON_2CLICK)==LOW){
			if(!double_click_press){
				double_click_action=true;
				double_click_press=true;  
			}
		}else{
			double_click_press=false;
        }
        
        if(digitalRead(BUTTON_RIGHT)==LOW){
			if(!right_click_press){
				right_click_action=true;
				right_click_press=true;  
			}
		}else{
			right_click_press=false;
        }
        
        if(digitalRead(BUTTON_DRAG)==LOW){
			if(!drag_click_switch){ //different logic applied to the drag_button
				drag_click_action=!drag_click_action;
				drag_click_switch=true;
			}
		}else{
			drag_click_switch=false;
        }
        
		if(left_click_action){
            mouse_buttons |= 0x01;
        }
        
        if(right_click_action){
            mouse_buttons |= 0x01 << 1;
        }
        
        if(drag_click_action){ //usb drag_click communication
            mouse_buttons |= 0x01;
        }
              
        usbMultiHID.send_mouse_report(mouse_buttons, mouse_x, mouse_y, 0);
        
        if(double_click_action){
            usbMultiHID.update();
            mouse_buttons=0;
            usbMultiHID.send_mouse_report(mouse_buttons, mouse_x, mouse_y, 0);
            mouse_buttons |= 0x01;
            delayMs(70);
            usbMultiHID.send_mouse_report(mouse_buttons, mouse_x, mouse_y, 0);
            mouse_buttons=0;
            usbMultiHID.send_mouse_report(mouse_buttons, mouse_x, mouse_y, 0);
            mouse_buttons |= 0x01;
            delayMs(70);
            usbMultiHID.send_mouse_report(mouse_buttons, mouse_x, mouse_y, 0); 
        }
    }//End of mode mouse
  
	if(mode_scroll){
		if(!press_mouse_wheel){
			if(digitalRead(MV_DOWN)==LOW){
				delay(25);
				if(digitalRead(MV_DOWN)==LOW){
					press_up_mouse_wheel=true;
					press_mouse_wheel=true;
				}
			}
			if(digitalRead(MV_UP)==LOW){
				delay(25);
				if(digitalRead(MV_UP)==LOW){
					press_down_mouse_wheel=true;
					press_mouse_wheel=true;
				}
			}
		}
		if(press_mouse_wheel){
			if(press_up_mouse_wheel){
				if(digitalRead(MV_DOWN)==HIGH){
					mouse_wheel=1;
					press_up_mouse_wheel=false;
					press_mouse_wheel=false;
				}
			}
			if(press_down_mouse_wheel){
				if(digitalRead(MV_UP)==HIGH){
					mouse_wheel=-1;
					press_down_mouse_wheel=false;
					press_mouse_wheel=false;
				}
			}
		}
		usbMultiHID.send_mouse_report(mouse_buttons, mouse_x, mouse_y, mouse_wheel);
	}//End of mode scroll
  
	if(mode_keyboard){
		verify_keyboard_buttons();
		keyboard_control();
		keyboard_buttons=0;
	}//End of mode keyboard
  
	if(mode_fliperama){ 
		if(digitalRead(MV_LEFT)==LOW){
			cleanKeyBuffer(1);
			pressKey(KEY_A);
			press_key=true;
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);
		}else{
			press_key=false;
		}
		if(digitalRead(MV_RIGHT)==LOW){
			cleanKeyBuffer(1);
			pressKey(KEY_D);
			press_key=true;
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);
		}else{
			press_key=false;
		}
		if(digitalRead(MV_UP)==LOW){
			cleanKeyBuffer(1);
			pressKey(KEY_S);
			press_key=true;
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);
		}else{
			press_key=false;
		}
		if(digitalRead(MV_DOWN)==LOW){
			cleanKeyBuffer(1);
			pressKey(KEY_W);
			press_key=true;
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);
		}else{
			press_key=false;
		}
		
		if(digitalRead(BUTTON_LEFT)==LOW){
			cleanKeyBuffer(1);
			pressKey(KEY_J);
			press_key=true;
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);
		}else{
			press_key=false;
		}
		if(digitalRead(BUTTON_2CLICK)==LOW){
			cleanKeyBuffer(1);
			pressKey(KEY_O);
			press_key=true;
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);
		}else{
			press_key=false;
		}
		if(digitalRead(BUTTON_RIGHT)==LOW){
			cleanKeyBuffer(1);
			pressKey(KEY_L);
			press_key=true;
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);
		}else{
			press_key=false;
		}
		if(digitalRead(BUTTON_DRAG)==LOW){
			cleanKeyBuffer(1);
			pressKey(KEY_K);
			press_key=true;
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);
		}else{
			press_key=false;
		}
		
		if(!press_key){
			cleanKeyBuffer(1);
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);
		}
    } //End mode fliperama
    
	if(mode_velocidade){
		if(!press_velocity){
			if(digitalRead(MV_DOWN)==LOW){
				delay(25);
				if(digitalRead(MV_DOWN)==LOW){
					press_add_velocity=true;
					press_velocity=true;
				}
			}
			if(digitalRead(MV_UP)==LOW){
				delay(25);
				if(digitalRead(MV_UP)==LOW){
					press_less_velocity=true;
					press_velocity=true;
				}
			}
		}
		if(press_velocity){
			if(press_add_velocity){
				if(digitalRead(MV_DOWN)==HIGH){
					if(travel<10){
						travel++;
						press_add_velocity=false;
						press_velocity=false;
						EEPROM.write(4, travel);
						Blink();
					}
				}
			}
			if(press_less_velocity){
				if(digitalRead(MV_UP)==HIGH){
					if(travel>3){
						travel--;
						press_less_velocity=false;
						press_velocity=false;
						EEPROM.write(4, travel);
						Blink();
					}
				}
			}
		}
	}
  
	if(mode_reprog){
		Key_program();
	}

}//End loop
