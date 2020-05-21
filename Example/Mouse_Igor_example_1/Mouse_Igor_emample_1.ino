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
//botoes do joystick - digitais
#define MV_LEFT 13
#define MV_RIGHT 11
#define MV_UP 10
#define MV_DOWN 12

// botao reset
#define RESET 9

// definicaio dos leds
#define MODE_REFFERENCE_LED_1 3
#define MODE_REFFERENCE_LED_2 A5
#define MODE_REFFERENCE_LED_3 A4

#define REPROG_AUX_PIN A3

// definicaio de parametros globais
#define PRECISION 2
#define TIME_DELAY 20
#define PAUSE 500
#define HALF_PAUSE 250
#define LITTLE_PAUSE 125
#define BYPASS_TIMER_ISR 1

//definicao da tabela de caracteres

//definicoes retiradas de usbhid...

//existem duas formas de trabalhar com essa tabela, colocar todas as combinacoes e verificar os valores de ponteiros validos
// ou colocar todos os valores como 0 e depois para valores de ponteiros validos colocar o valor correspondente, a segunda abordagem sera tomada

#define nada 0
const char tabela[]={ 
 nada,//        0
 nada,//        1
 nada,//        2
 nada,//        3
 KEY_A,//       4
 KEY_B,//       5
 KEY_C,//       6
 KEY_D,//       7
 KEY_E,//       8
 KEY_F,//       9
 KEY_G,//       10
 KEY_H,//       11
 KEY_I,//       12
 KEY_J,//       13
 KEY_K,//       14
 KEY_L,//       15
 KEY_M,//       16
 KEY_N,//       17
 KEY_O,//       18
 KEY_P,//       19
 KEY_Q,//       20
 KEY_R,//       21
 KEY_S,//       22
 KEY_T,//       23
 KEY_U,//       24
 KEY_V,//       25
 KEY_W,//       26
 KEY_X,//       27
 KEY_Y,//       28
 KEY_Z,//       29
 KEY_1,//       30
 KEY_2,//       31
 KEY_3,//       32
 KEY_4,//       33
 KEY_5,//       34
 KEY_6,//       35
 KEY_7,//       36
 KEY_8,//       37
 KEY_9,//       38
 KEY_0,//       39
 KEY_ENTER,//        40
 KEY_ESCAPE,//       41
 KEY_DELETE,//       42
 KEY_TAB,//          43
 KEY_SPACE,//        44
 KEY_MINUS,//        45
 KEY_EQUALS,//       46
 KEY_LBRACKET,//     47
 KEY_RBRACKET,//     48
 KEY_BACKSLASH,//    49
 KEY_NONUS_NUMBER,// 50
 KEY_SEMICOLON,//    51
 KEY_QUOTE,//        52
 KEY_TILDE,//        53
 KEY_COMMA,//        54
 KEY_PERIOD,//       55
 KEY_SLASH,//        56
 KEY_CAPSLOCK,//     57
 KEY_F1,//      58
 KEY_F2,//      59
 KEY_F3,//      60
 KEY_F4,//      61
 KEY_F5,//      62
 KEY_F6,//      63
 KEY_F7,//      64
 KEY_F8,//      65
 KEY_F9,//      66
 KEY_F10,//     67
 KEY_F11,//     68
 KEY_F12 //     69
}
char poteiro_tabela=0; // inicia uma variavel que ira indicar a posicao da tabela em que iremos pegar o caracter

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

// definicao dos botoes reprogramaveis
byte BUTTON_LEFT =   7;
byte BUTTON_2CLICK = 8;
byte BUTTON_RIGHT =  6;
byte BUTTON_DRAG =   5;

int mouse_pins[8]={7,8,11,10,12,13,6,5}; // pinagem dos botoes do mouse - mais uma definicao de botoes?
int numero_botoes=8; // numero de botoes que o mouse possui e serao checados
int botoes_pressionados[]={0,0};
uint16_t keyboard_buttons=0; //variael que ira armazenar o botao selecionado? 
byte count_verify_buttons=0;
byte bit_0_8_keyboard_buttons=0;
boolean bit_0_8_keyboard_buttons_flag=true; //true to bit 0 and false to bit 8
byte first_pressed_button=0;
byte second_pressed_button=0;
boolean pressed_button_flag=false;
boolean pressed_second_combination_condition=false;
boolean second_combination=false;

boolean mode_flag = true; // original = true
boolean bt_drag = false; // original = false
boolean prog_press = false; // original = false
  
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
	int ia=0
	for(ia=0;ia<numero_botoes;i++){
		if(mouse_pins[i]==LOW){ 
			while (mouse_pins[i]==LOW) {
				delay_ms(15);
			};
			break;
		} 
	}
	//se nenhum botao foi pressionado os dois valores da variavel botoes_pressionados serao 0
	if (botoes_pressionados[0]==0){
		botoes_pressionados[0]=ia;
	}
	else if(botoes_pressionados[0]!=ia){  // dois numeros iguais sao proibidos apenas para facilitar a programacao
		botoes_pressionados[1]=ia
	}
}

void keyboard_control(){
	// para usar esta funcao, os botoes deverao ser enumerados, de 1 a 8, 
	// e o keyboard_buttons devera ter o valor de 10*Primeiro_botao+segundo_botao
	// o resultado disso sera a posicao do elemento na tabela, ajustando a sequencia dos elemetnos 
	// na variavel tabela pode-se reorganizar todo o teclado.	
	ponteiro_tabela=10*botoes_pressionados[1]+botoes_pressionados[0]; // verificar se esta funcionando
	botoes_pressionados[1]=0;
	botoes_pressionados[0]=0;
	cleanKeyBuffer(1);
		if(shift){
			pressKey2(tabela[ponteiro_tabela],MOD_SHIFT_LEFT);
		}else{
			pressKey(tabela[ponteiro_tabela]);
		}
		usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses); // precisa fazer algo mais aqui??

//<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//verificar o que esta abaixo , nesta funcao
	switch(keyboard_buttons){
		case 0:						// o que e este zero?? nao esta definido
			cleanKeyBuffer(1);
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);
		break;
		
		case 0x8000:
			cleanKeyBuffer(1);    // o que e este valor?? nao definido
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
				
		default:
			cleanKeyBuffer(1);
			usbMultiHID.send_keyboard_report(KEYPRESS_BUFFER_LENGTH, keypresses);
		break;
	}
}
//varios A s repetidos no final


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
         
        if((digitalRead(MV_LEFT)==HIGH) && (digitalRead(MV_RIGHT)==HIGH)){   //nunca sera verdade???
			hx=10;
		} 
        if((digitalRead(MV_UP)==HIGH) && (digitalRead(MV_DOWN)==HIGH)){		//nunca sera verdade???
			hy=10;
		} 
        
		
		// verificar!!!
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
        // elif ou ja setar a variavel nos testes que esta acima
		if(left_click_action){
            mouse_buttons |= 0x01;
        }
        
        if(right_click_action){
            mouse_buttons |= 0x01 << 1;
        }
        
        if(drag_click_action){ //usb drag_click communication
            mouse_buttons |= 0x01;
        }
              
        usbMultiHID.send_mouse_report(mouse_buttons, mouse_x, mouse_y, 0);//envia dado do mouse, ok
        
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
		// quando o mouse_whell for pressionado o passo do mouse poderia ser diminuido
		usbMultiHID.send_mouse_report(mouse_buttons, mouse_x, mouse_y, mouse_wheel);
	}//End of mode scroll
  
	if(mode_keyboard){
		verify_keyboard_buttons();
		if ((botoes_pressionados[0]!=0)&&(botoes_pressionados[1]!=0)){
			keyboard_control();
		}
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
