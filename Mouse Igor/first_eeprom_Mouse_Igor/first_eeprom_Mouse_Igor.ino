/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  | Configuração inicial de pinos referentes a cliques salvos permanentemente no Software Mouse Inclusivo Igor - Assistiva | Tecnologia para a Inclusão Social - Versão 2018/2      |                                  
  | Contém 8 botões                                                                                                                                                                 |
  | Primeiro botão pré-definido: LEFT                                                                                                                                               |
  | Segundo botão pré-definido: 2CLICK                                                                                                                                              |
  | Terceiro botão pré-definido: RIGHT                                                                                                                                              |
  | Quarto botão pré-definido: DRAG                                                                                                                                                 |
  | Todos os botões podem ser reconfigarado da forma que o usuário quiser                                                                                                           |
  -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#include <EEPROM.h>   

void setup() {
  // put your setup code here, to run once:
  byte BUTTON_LEFT =   7;
  byte BUTTON_2CLICK = 8;
  byte BUTTON_RIGHT =  6;
  byte BUTTON_DRAG =   5;

  EEPROM.write(0, BUTTON_LEFT);
  EEPROM.write(1, BUTTON_2CLICK); 
  EEPROM.write(2, BUTTON_RIGHT);
  EEPROM.write(3, BUTTON_DRAG);  
  EEPROM.write(4, 5);  
  EEPROM.write(5, 1);
}

void loop(){
  
}

