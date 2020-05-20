/*
 * Based on Obdev's AVRUSB code and under the same license.
 */
#ifndef __UsbMultiHID_h__
#define __UsbMultiHID_h__

#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <string.h>
#include <util/delay.h>
#include "usbdrv.h"

typedef uint8_t byte;

#define BUFFER_SIZE 8 // Minimum of 2: 1 for modifiers + 1 for keystroke

#define REPORT_ID_KEYBOARD 2
#define REPORT_ID_MOUSE 3
#define REPORT_ID_JOYSTICK 4
#define REPORT_ID_JOYSTICK2 5

#define KEYPRESS_BUFFER_LENGTH 7
#define SIMULTANEOUS_KEYS 6
#define MAX_SINGLE_TRANSMISSION_SIZE 8

#define REPORT_SIZE_MOUSE 4

#define NUM_LOGICAL_AXES 6
#define NUM_LOGICAL_BUTTONS 16


static uchar reportBuffer[12];  /* buffer for HID reports */
static uchar idleRate;          // in 4 ms units


/*
 * USB Report Descriptor com as definições dos 3 dispositivos representados
 * (Teclado, Mouse e Joystick).
 * Caso seja feita alguma alteração que implique mudança no tamanho do array,
 * o valor da constante USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH deve ser corrigido
 * no arquivo usbconfig.h da biblioteca.
 */
PROGMEM const char usbHidReportDescriptor[USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH] = {
  // KEYBOARD - USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH = 37
  0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
  0x09, 0x06,                    // USAGE (Keyboard)
  0xa1, 0x01,                    // COLLECTION (Application)
  0x85, 0x02,                    //   REPORT_ID (2)
  0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
  0x19, 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl)
  0x29, 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI)
  0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
  0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
  0x75, 0x01,                    //   REPORT_SIZE (1)
  0x95, 0x08,                    //   REPORT_COUNT (8)
  0x81, 0x02,                    //   INPUT (Data,Var,Abs)
  0x95, 0x05,                    //   REPORT_COUNT (5)
  0x75, 0x08,                    //   REPORT_SIZE (8)
  0x25, 0x65,                    //   LOGICAL_MAXIMUM (101)
  0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated))
  0x29, 0x65,                    //   USAGE_MAXIMUM (Keyboard Application)
  0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
  0xc0,                          // END_COLLECTION

  // MOUSE - USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH = 54
  0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
  0x09, 0x02,                    // USAGE (Mouse)
  0xa1, 0x01,                    // COLLECTION (Application)
  0x85, 0x03,                    //   REPORT_ID (3)
  0x09, 0x01,                    //   USAGE (Pointer)
  0xa1, 0x00,                    //   COLLECTION (Physical)
  0x05, 0x09,                    //     USAGE_PAGE (Button)
  0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
  0x29, 0x03,                    //     USAGE_MAXIMUM (Button 3)
  0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
  0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
  0x95, 0x03,                    //     REPORT_COUNT (3)
  0x75, 0x01,                    //     REPORT_SIZE (1)
  0x81, 0x02,                    //     INPUT (Data,Var,Abs)
  0x95, 0x01,                    //     REPORT_COUNT (1)
  0x75, 0x05,                    //     REPORT_SIZE (5)
  0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
  0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
  0x09, 0x30,                    //     USAGE (X)
  0x09, 0x31,                    //     USAGE (Y)
  0x09, 0x38,                    //     USAGE (Wheel)
  0x15, 0x81,                    //     LOGICAL_MINIMUM (-127)
  0x25, 0x7f,                    //     LOGICAL_MAXIMUM (127)
  0x75, 0x08,                    //     REPORT_SIZE (8)
  0x95, 0x03,                    //     REPORT_COUNT (3)
  0x81, 0x06,                    //     INPUT (Data,Var,Rel)
  0xc0,                          //   END_COLLECTION
  0xc0,                          // END_COLLECTION

  // JOYSTICK - USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH = 76
  0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
  0x09, 0x04,                    // USAGE (Joystick)
  0xa1, 0x01,                    // COLLECTION (Application)
  0x85, 0x04,                    //   REPORT_ID (4)
  0xa1, 0x00,                    //   COLLECTION (Physical)
  0x09, 0x30,                    //     USAGE (X)
  0x09, 0x31,                    //     USAGE (Y)
  0x09, 0x33,                    //     USAGE (Rx)
  0x09, 0x35,                    //     USAGE (Rz)
  0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
  0x26, 0xff, 0x03,              //     LOGICAL_MAXIMUM (1023)
  0x75, 0x0a,                    //     REPORT_SIZE (10)
  0x95, 0x04,                    //     REPORT_COUNT (4)
  0x81, 0x02,                    //     INPUT (Data,Var,Abs)
  0xc0,                          //   END_COLLECTION
  0x05, 0x09,                    //   USAGE_PAGE (Button)
  0x19, 0x01,                    //   USAGE_MINIMUM (Button 1)
  0x29, 0x10,                    //   USAGE_MAXIMUM (Button 16)
  0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
  0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
  0x75, 0x01,                    //   REPORT_SIZE (1)
  0x95, 0x10,                    //   REPORT_COUNT (16)
  0x55, 0x00,                    //   UNIT_EXPONENT (0)
  0x65, 0x00,                    //   UNIT (None)
  0x81, 0x02,                    //   INPUT (Data,Var,Abs)
  0x85, 0x05,                    //   REPORT_ID (5)
  0x05, 0x01,                    //   USAGE_PAGE (Generic Desktop)
  0x09, 0x32,                    //   USAGE (Z)
  0x09, 0x34,                    //   USAGE (Ry)
  0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
  0x26, 0xff, 0x03,              //   LOGICAL_MAXIMUM (1023)
  0x75, 0x0a,                    //   REPORT_SIZE (10)
  0x95, 0x02,                    //   REPORT_COUNT (2)
  0x81, 0x02,                    //   INPUT (Data,Var,Abs)
  0x75, 0x04,                    //   REPORT_SIZE (4)
  0x95, 0x01,                    //   REPORT_COUNT (1)
  0x81, 0x03,                    //   INPUT (Cnst,Var,Abs)
  0xc0,                          // END_COLLECTION
};



/* Keyboard usage values, see usb.org's HID-usage-tables document, chapter
 * 10 Keyboard/Keypad Page for more codes.
 */
#define MOD_CONTROL_LEFT    (1<<0)
#define MOD_SHIFT_LEFT      (1<<1)
#define MOD_ALT_LEFT        (1<<2)
#define MOD_GUI_LEFT        (1<<3)
#define MOD_CONTROL_RIGHT   (1<<4)
#define MOD_SHIFT_RIGHT     (1<<5)
#define MOD_ALT_RIGHT       (1<<6)
#define MOD_GUI_RIGHT       (1<<7)

#define KEY_A       4
#define KEY_B       5
#define KEY_C       6
#define KEY_D       7
#define KEY_E       8
#define KEY_F       9
#define KEY_G       10
#define KEY_H       11
#define KEY_I       12
#define KEY_J       13
#define KEY_K       14
#define KEY_L       15
#define KEY_M       16
#define KEY_N       17
#define KEY_O       18
#define KEY_P       19
#define KEY_Q       20
#define KEY_R       21
#define KEY_S       22
#define KEY_T       23
#define KEY_U       24
#define KEY_V       25
#define KEY_W       26
#define KEY_X       27
#define KEY_Y       28
#define KEY_Z       29
#define KEY_1       30
#define KEY_2       31
#define KEY_3       32
#define KEY_4       33
#define KEY_5       34
#define KEY_6       35
#define KEY_7       36
#define KEY_8       37
#define KEY_9       38
#define KEY_0       39

#define KEY_ENTER        40
#define KEY_ESCAPE       41
#define KEY_DELETE       42
#define KEY_TAB          43
#define KEY_SPACE        44
#define KEY_MINUS        45
#define KEY_EQUALS       46
#define KEY_LBRACKET     47
#define KEY_RBRACKET     48
#define KEY_BACKSLASH    49
#define KEY_NONUS_NUMBER 50
#define KEY_SEMICOLON    51
#define KEY_QUOTE        52
#define KEY_TILDE        53
#define KEY_COMMA        54
#define KEY_PERIOD       55
#define KEY_SLASH        56
#define KEY_CAPSLOCK     57

#define KEY_F1      58
#define KEY_F2      59
#define KEY_F3      60
#define KEY_F4      61
#define KEY_F5      62
#define KEY_F6      63
#define KEY_F7      64
#define KEY_F8      65
#define KEY_F9      66
#define KEY_F10     67
#define KEY_F11     68
#define KEY_F12     69

#define KEY_ARROW_LEFT 0x50


class UsbMultiHID {
public:
  UsbMultiHID () {
    usbInit();
    sei();
  }

  void update() {
    usbPoll();
  }

  void send_keyboard_report(
      int count,
      byte* buttons ) {

    if (count < MAX_SINGLE_TRANSMISSION_SIZE) {
      memset(reportBuffer, 0, sizeof(reportBuffer));

      reportBuffer[0] = REPORT_ID_KEYBOARD;
      memcpy(reportBuffer+1, buttons, count);

      while (!usbInterruptIsReady()) {
        usbPoll();
      }
      usbSetInterrupt(reportBuffer, SIMULTANEOUS_KEYS+2);
    }
  }

  void send_single_keyboard_report(
      byte keyStroke,
      byte modifiers ) {

    while (!usbInterruptIsReady()) {
      usbPoll();
    }

    memset(reportBuffer, 0, sizeof(reportBuffer));

    reportBuffer[0] = REPORT_ID_KEYBOARD;
    reportBuffer[1] = modifiers;
    reportBuffer[2] = keyStroke;
    reportBuffer[3] = 0;
    reportBuffer[4] = 0;

    usbSetInterrupt(reportBuffer, SIMULTANEOUS_KEYS+2);

    while (!usbInterruptIsReady()) {
      usbPoll();
    }

    // This stops endlessly repeating keystrokes:
    memset(reportBuffer, 0, SIMULTANEOUS_KEYS+2 );
    reportBuffer[0] = REPORT_ID_KEYBOARD;
    usbSetInterrupt(reportBuffer, SIMULTANEOUS_KEYS+2);
  }

  void send_mouse_report(
      unsigned char buttons,
      char x,
      char y,
      char wheel ) {

    while (!usbInterruptIsReady()) {
      usbPoll();
    }

    memset( reportBuffer, 0, sizeof( reportBuffer ) );
    reportBuffer[0] = REPORT_ID_MOUSE;
    reportBuffer[1] = buttons;
    reportBuffer[2] = x;
    reportBuffer[3] = y;
    reportBuffer[4] = wheel;

    while (!usbInterruptIsReady()) {
      usbPoll();
    }
    usbSetInterrupt(reportBuffer, REPORT_SIZE_MOUSE+1);
  }

  void send_joystick_report(
      unsigned short x,
      unsigned short y,
      unsigned short z,
      unsigned short rx,
      unsigned short ry,
      unsigned short rz,
      unsigned short btn ) {

    unsigned char high;
    unsigned char low;
    unsigned char temp;

    reportBuffer[0] = REPORT_ID_JOYSTICK;

    high = x >> 8;
    low = x & 255;
    reportBuffer[1] = low;
    temp = high;

    high = y >> 6;
    low = y & 63;
    reportBuffer[2] = (low << 2) + temp;
    temp = high;

    high = rx >> 4;
    low = rx & 15;
    reportBuffer[3] = (low << 4) + temp;
    temp = high;

    high = rz >> 2;
    low = rz & 3;
    reportBuffer[4] = (low << 6) + temp;
    temp = high;

    high = 0;
    low = 0;
    reportBuffer[5] = temp;
    temp = high;

    reportBuffer[6] = btn & 255;
    reportBuffer[7] = btn >> 8;
    while (!usbInterruptIsReady()) {
      usbPoll();
    }
    usbSetInterrupt(reportBuffer, 8);

    // now set up the second report
    reportBuffer[8] = REPORT_ID_JOYSTICK2;

    high = z >> 8;
    low = z & 255;
    reportBuffer[9] = low;
    temp = high;

    high = ry >> 6;
    low = ry & 63;
    reportBuffer[10] = (low << 2) + temp;
    temp = high;

    high = 0;
    low = 0;
    reportBuffer[11] = (low << 4) + temp;

    while (!usbInterruptIsReady()) {
      usbPoll();
    }
    usbSetInterrupt( reportBuffer+8, 4 );
  }

  uchar    reportBuffer[12];
};

UsbMultiHID usbMultiHID = UsbMultiHID();

#ifdef __cplusplus
extern "C"{
#endif
  // USB_PUBLIC uchar usbFunctionSetup
  uchar usbFunctionSetup(uchar data[8])
  {
    usbRequest_t    *rq = (usbRequest_t *)((void *)data);

    usbMsgPtr = usbMultiHID.reportBuffer; //
    if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){
      if(rq->bRequest == USBRQ_HID_GET_REPORT){
        return 0;
      }
      else if(rq->bRequest == USBRQ_HID_GET_IDLE){
        return 0;
      }
      else if(rq->bRequest == USBRQ_HID_SET_IDLE){
        idleRate = rq->wValue.bytes[1];
      }
    }
    else{
      /* no vendor specific requests implemented */
    }
    return 0;
  }
#ifdef __cplusplus
} // extern "C"
#endif


#endif // __UsbMultiHID_h__

