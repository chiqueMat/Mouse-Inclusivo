/*------------------------------------------------------------------------------------------------------
  | Software Mouse Inclusivo Bernardo - Assistiva | Tecnologia para a Inclusão Social - Versão 2018/1  |                                  
  | Contém 4 botões, 1 led e 1 potenciometro para ajustar a sensibilidade do mouse                     |
  | Primeiro botão pré-definido: LEFT                                                                  |
  | Segundo botão pré-definido: 2CLICK                                                                 |
  | Terceiro botão pré-definido: RIGHT                                                                 |
  | Quarto botão pré-definido: DRAG                                                                    |
  | Todos os botões podem ser reconfigarado da forma que o usuário quiser                              |
  | Pode-se jogar com dois jogadores                                                                   |
  | Dois modos de jogo: Mouse e fliperama                                                              |
  |  Led apagado: Modo mouse                                                                           |
  |  Led piscando: Modo fliperama player 1                                                             |
  |  Led aceso: Modo fliperama player 2                                                                |
  ------------------------------------------------------------------------------------------------------ 
*/

#include <EEPROM.h>   
#include "UsbMultiHID.h" // Biblioteca USB
                
// Definições de botões e controles
 
#define MV_LEFT 5
#define MV_RIGHT 7
#define MV_UP 8
#define MV_DOWN 9

#define RESET 6

#define LED_PROG 3

#define PRECISION 2
#define TIME_DELAY 20
#define PAUSE 500
#define HALF_PAUSE 250
#define LITTLE_PAUSE 125

#define BYPASS_TIMER_ISR 1

byte BUTTON_LEFT =   10;
byte BUTTON_2CLICK = 11;
byte BUTTON_RIGHT =  12;
byte BUTTON_DRAG =   13;

boolean mode_flag = true;boolean led_flag = true;

boolean bt_drag = false;

boolean mode_mouse = true;

boolean prog_press = false; 
  
byte keypresses[KEYPRESS_BUFFER_LENGTH];

boolean left_click_press=false;//flag that indicate that capacitive button is pressed in
boolean right_click_press=false;//flag that indicate that capacitive button is pressed in
boolean drag_click_switch=false;//auxiliar flag for drag button, drag button logic is like to a light switch
boolean drag_click_action=false;//auxiliar flag for drag button, drag button logic is like to a light switch
boolean double_click_press=false;//flag that indicate that capacitive button is pressed in

int oldX=0;
int oldY=0;

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
  for (int i=0; i <= 5; i++)
  {
      digitalWrite(LED_PROG, LOW);
      delayMs(LITTLE_PAUSE);
      digitalWrite(LED_PROG, HIGH);
      delayMs(50);
   } 
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

void BTS_and_RESET_ON(){
byte inputs[9] = {5,6,7,8,9,10,11,12,13};
    
    for(int i=0;i<=9;i++){ 
      pinMode(inputs[i], INPUT);
      digitalWrite(inputs[i], HIGH);
    }
}

 void delay_and_press(){
  delayMs(PAUSE);
  prog_press = true;
 }


void Key_program(){
	// configura botão LEFT
	byte aux_1_prog=10;
	byte aux_2_prog=11;
	byte aux_3_prog=12;
	byte aux_4_prog=13;
  
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
    } 
    while(prog_press == false);

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

    prog_press == false;
    
    Blink();
        
    EEPROM.write(0, BUTTON_LEFT);
    EEPROM.write(1, BUTTON_2CLICK); 
    EEPROM.write(2, BUTTON_RIGHT);
    EEPROM.write(3, BUTTON_DRAG);  
    EEPROM.write(5, 1);
} //End of Key_program

void readJoystick(){ 
	int joystick_x,joystick_y;
	
	joystick_x = map(analogRead(A0), 0, 1023, 1, 20);  
	joystick_y = map(analogRead(A1), 0, 1023, 1, 20); 
  
	if((oldX != joystick_x) || (oldY != joystick_y)){  
		oldX = joystick_x;  
		oldY = joystick_y;   
	}  
	delayMs(10);  
}

void setup() {
  
 if (EEPROM.read(5)==1)
 { 
    BUTTON_LEFT = EEPROM.read(0);
    BUTTON_2CLICK = EEPROM.read(1); 
    BUTTON_RIGHT = EEPROM.read(2); 
    BUTTON_DRAG = EEPROM.read(3); 
 }

  BTS_and_RESET_ON();

  pinMode(LED_PROG, OUTPUT);
  digitalWrite(LED_PROG, LOW);

  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, OUTPUT);
  digitalWrite(A5, HIGH);

  mode_mouse = true;
  
// Definição para uso de alternativa para o Delay
#if BYPASS_TIMER_ISR
  // disable timer 0 overflow interrupt (used for millis)
  TIMSK0&=!(1<<TOIE0); // ++
#endif
} //End of setup

void loop(){
  
    boolean left_click_action=false; boolean right_click_action=false;boolean double_click_action=false;
    
    short mouse_x, mouse_y;
    byte count = 0;
    byte travel = 10; 
    byte mouse_buttons = 0;     
    byte prog_time = 0;
    
    usbMultiHID.update();
    
    if(mode_flag==true && (digitalRead(RESET)==LOW)){
        mode_flag=false;
    }
    
    while(mode_mouse==true && (digitalRead(RESET)==LOW)){
        ++prog_time;
        delayMs(1000);
        if(prog_time > 5){
            Key_program();
            mode_mouse=true;
            mode_flag=true;
        }
    }
   
    travel = map(analogRead(A4), 0, 1023, 2, 10);
    
    #if BYPASS_TIMER_ISR  // check if timer isr fixed.
      delayMs(TIME_DELAY);   // val = TIME_DELAY 
    #else
      delay(TIME_DELAY);
    #endif  

    if(mode_mouse==true){
      digitalWrite(LED_PROG, LOW);
    }
		
		readJoystick();
        
        mouse_x = oldX;
        mouse_x = constrain(map(mouse_x, 0, 20, -travel, +travel), -travel, +travel);
        mouse_x = (mouse_x/PRECISION)*PRECISION;
        
        mouse_y = oldY;
        mouse_y = constrain(map(mouse_y, 20, 0, -travel, +travel), -travel, +travel);
        mouse_y = (mouse_y/PRECISION)*PRECISION;
        
        // *************************************************************
        // Leitura dos botões e montagem do byte para o buffer do mouse
        // *************************************************************
      
        if (digitalRead(BUTTON_LEFT)==LOW){
          if(!left_click_press){
            left_click_action=true;  
            left_click_press=true;
          }
        }else{
          left_click_press=false;
        }
        
        if (digitalRead(BUTTON_2CLICK)==LOW){
          if(!double_click_press){
             double_click_action=true;
             double_click_press=true;  
          }
        }else{
          double_click_press=false;
        }
        
        if (digitalRead(BUTTON_RIGHT)==LOW){
          if(!right_click_press){
            right_click_action=true;
            right_click_press=true;  
          }
        }else{
          right_click_press=false;
        }
        
        if (digitalRead(BUTTON_DRAG)==LOW){
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
} //End loop

