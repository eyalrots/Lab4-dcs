#ifndef _bsp_H_
#define _bsp_H_

#include  <msp430g2553.h>  // MSP430x2xx

#define   debounceVal      250
#define   SMCLK            1095870

// POT abstraction
#define POTPortIn           P1IN
#define POTPortSel          P1SEL
#define POTPortDir          P1DIR

// Buzzer abstraction
#define BuzzPortDir         P2DIR
#define BuzzPortSel         P2SEL

// LCDs abstraction
#define LCD_DATA_WRITE      P1OUT
#define LCD_DATA_DIR        P1DIR
#define LCD_DATA_READ       P1IN
#define LCD_DATA_SEL        P1SEL
#define LCD_CTL_SEL         P2SEL

// UART abstraction
#define TXD BIT2
#define RXD BIT1

// PushButtons abstraction
#define PBsArrPort         P1IN
#define PBsArrIntPend      P1IFG
#define PBsArrIntEn        P1IE
#define PBsArrIntEdgeSel   P1IES
#define PBsArrPortSel      P1SEL
#define PBsArrPortDir      P1DIR
#define PB0                0x01
#define PB1                0x02
#define PB2                0x04
#define PB3                0x08

void __GPIO_config();
void __timerA0_config();
void __timerA1_config();
void __adc_config();
void __UART_config();

typedef enum {
    state0,
    state1,
    state2,
    state3,
    state4,
    state5,
    state6,
    state7,
    state8,
}FSM_state_t;

typedef enum {
    mode0,
    mode1,
    mode2,
    mode3,
    mode4
}SYS_mode_t;

#endif
