#include  "../header/bsp.h"    // private library - BSP layer

#define LCD_DATA 0xE0

void __GPIO_config() {
    WDTCTL = WDTHOLD | WDTPW;           // Stop WDT

    // LCD configuration
    LCD_DATA_WRITE &= ~LCD_DATA;            // Bit clear P1.5-P1.7
    LCD_DATA_DIR   |= LCD_DATA;             // P1.5-P1.7 -> output('1')
    LCD_DATA_SEL   &= ~LCD_DATA;              // GPIO capabilities
    LCD_CTL_SEL    &= ~LCD_DATA;            // Bit clear P2.5-P2.7

    // PushButtons Setup
    PBsArrPortSel  &= ~BIT0;            // GPIO capability
    PBsArrPortDir  &= ~BIT0;            // input direction
    PBsArrIntEdgeSel |= BIT0;           // pull-up mode
    // PBsArrIntEdgeSel &= ~0x0C;          // pull-down mode
    PBsArrIntEn    |= BIT0;
    PBsArrIntPend  &= ~BIT0;            // clear pending interrupts

    // POT setup
    POTPortDir     &= ~BIT3;
    POTPortSel     &= ~BIT3;

    // Buzz configuration
    BuzzPortDir |= BIT4;
    BuzzPortSel |= BIT4;

    _BIS_SR(GIE);                     // enable interrupts globally
}

// TIMERS
void __timerA0_config() {
    TA0CTL |= TACLR; //reset the counter
    TA0CTL = TASSEL_2 + ID_3 + TAIE; // set counter to (=2^17Hz), enable interrupts
    TA0CTL &= ~TAIFG; //clear pending interrupts
    TA0CCR0 = 0xffff; // in up mode, will request an interrupt each 500msec
}

void __timerA1_config() {
    TA1CTL = TASSEL_2;
    TA1CCTL2 |= OUTMOD_7;
}

// ADC
void __adc_config() {
    ADC10CTL0 = SREF_0 + ADC10SHT_2 + MSC + ADC10IE;
    ADC10CTL0 &= ~ADC10IFG;

    ADC10CTL1 = INCH_3 + SHS_0 + ADC10SSEL_3 + CONSEQ_2;
    ADC10AE0 = BIT3; // enable P1.3 as analog input
}

// UART
void __UART_config() {

    P1SEL |= 0x06;       // P1.1 = RXD, P1.2=TXD
    P1SEL2 |= 0x06;      // P1.1 = RXD, P1.2=TXD

    UCA0CTL1 |= UCSWRST; // hold in reset whilst configurating

    UCA0CTL1 |= UCSSEL_2; // SMCLK source
    UCA0MCTL = UCBRS0;
    UCA0BR0 = 109; //Baud Rate: 9600
    UCA0BR1 = 0;

    UCA0CTL1 &= ~UCSWRST;  // Release USCI from reset

    IE2 |= UCA0RXIE; //enable rx

}

