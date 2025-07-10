#include  "../header/halGPIO.h"     // private library - HAL layer

const int buzz_freq[7] = {1024, 819, 682, 585, 512, 455, 410};
const char quote[16] = "I love my Negev\0";

extern volatile FSM_state_t state;
extern volatile SYS_mode_t lpm_mode;

extern volatile char x_string[];
extern volatile int X, PB1_pressed, count;

volatile unsigned int buzz_idx, j, delay_ifg, Nadc;

//--------------------------------------------------------------------
//             System Configuration
//--------------------------------------------------------------------
void system_config() {
    delay_ifg=1;
    __GPIO_config();
    __timerA0_config();
    __timerA1_config();
    __adc_config();
    __UART_config();
}
//---------------------------------------------------------------------
//            Polling based Delay function
//---------------------------------------------------------------------
void delay(unsigned int t){  //
    for(;t > 0; t--);
}
//---------------------------------------------------------------------
//            Enter from LPM0 mode
//---------------------------------------------------------------------
void enterLPM(unsigned char LPM_level){
    if (LPM_level == 0x00)
      _BIS_SR(LPM0_bits);     /* Enter Low Power Mode 0 */
        else if(LPM_level == 0x01)
      _BIS_SR(LPM1_bits);     /* Enter Low Power Mode 1 */
        else if(LPM_level == 0x02)
      _BIS_SR(LPM2_bits);     /* Enter Low Power Mode 2 */
    else if(LPM_level == 0x03)
      _BIS_SR(LPM3_bits);     /* Enter Low Power Mode 3 */
        else if(LPM_level == 0x04)
      _BIS_SR(LPM4_bits);     /* Enter Low Power Mode 4 */
}
//---------------------------------------------------------------------
//            Enable interrupts
//---------------------------------------------------------------------
void enable_interrupts(){
  _BIS_SR(GIE);
}
//---------------------------------------------------------------------
//            Disable interrupts
//---------------------------------------------------------------------
void disable_interrupts(){
  _BIC_SR(GIE);
}
//---------------------------------------------------------------------
//            LCD
//---------------------------------------------------------------------
//******************************************************************
// send a command to the LCD -> copied
//******************************************************************
void lcd_cmd(unsigned char c){

    LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h

    if (LCD_MODE == FOURBIT_MODE)
    {
        LCD_DATA_WRITE &= ~OUTPUT_DATA;// clear bits before new write
        LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;
        lcd_strobe();
        LCD_DATA_WRITE &= ~OUTPUT_DATA;
        LCD_DATA_WRITE |= (c & (0x0F)) << LCD_DATA_OFFSET;
        lcd_strobe();
    }
    else
    {
        LCD_DATA_WRITE = c;
        lcd_strobe();
    }
}
//******************************************************************
// send data to the LCD -> copied
//******************************************************************
void lcd_data(unsigned char c){

    LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h

    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_RS(1);
    if (LCD_MODE == FOURBIT_MODE)
    {
            LCD_DATA_WRITE &= ~OUTPUT_DATA;
            LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;
            lcd_strobe();
            LCD_DATA_WRITE &= (0xF0 << LCD_DATA_OFFSET) | (0xF0 >> 8 - LCD_DATA_OFFSET);
            LCD_DATA_WRITE &= ~OUTPUT_DATA;
            LCD_DATA_WRITE |= (c & 0x0F) << LCD_DATA_OFFSET;
            lcd_strobe();
    }
    else
    {
            LCD_DATA_WRITE = c;
            lcd_strobe();
    }

    LCD_RS(0);
}
//******************************************************************
// write a string of chars to the LCD -> copied
//******************************************************************
void lcd_puts(const char * s){

    while(*s)
        lcd_data(*s++);
}
//******************************************************************
// initialize the LCD -> copied
//******************************************************************
void lcd_init(){

    char init_value;

    if (LCD_MODE == FOURBIT_MODE) init_value = 0x03 << LCD_DATA_OFFSET;
    else init_value = 0x3F;

    LCD_RS_DIR(OUTPUT_PIN);
    LCD_EN_DIR(OUTPUT_PIN);
    LCD_RW_DIR(OUTPUT_PIN);
    LCD_DATA_DIR |= OUTPUT_DATA;
    LCD_RS(0);
    LCD_EN(0);
    LCD_RW(0);

    // sanity check - need mental health hospital?
    P2DIR |= 0xe0;
    P2OUT |= 0xe0;
    P2OUT &= ~0xe0;

    DelayMs(15);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();
    DelayMs(5);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();
    DelayUs(200);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();

    if (LCD_MODE == FOURBIT_MODE){
        LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h
        LCD_DATA_WRITE &= ~OUTPUT_DATA;
        LCD_DATA_WRITE |= 0x2 << LCD_DATA_OFFSET; // Set 4-bit mode
        lcd_strobe();
        lcd_cmd(0x28); // Function Set
    }
    else lcd_cmd(0x3C); // 8bit,two lines,5x10 dots

    lcd_cmd(0xF); //Display On, Cursor On, Cursor Blink
    lcd_cmd(0x1); //Display Clear
    lcd_cmd(0x6); //Entry Mode
    lcd_cmd(0x80); //Initialize DDRAM address to zero
}
//******************************************************************
// lcd strobe functions -> copied
//******************************************************************
void lcd_strobe(){
  LCD_EN(1);
  asm("NOP");
 // asm("NOP");
  LCD_EN(0);
}
//******************************************************************
// Delay usec functions -> copied
//******************************************************************
void DelayUs(unsigned int cnt){

    unsigned char i;
    for(i=cnt ; i>0 ; i--) asm("nop"); // tha command asm("nop") takes raphly 1usec

}
//******************************************************************
// Delay msec functions -> copied
//******************************************************************
void DelayMs(unsigned int cnt){

    unsigned char i;
    for(i=cnt ; i>0 ; i--) DelayUs(1000); // tha command asm("nop") takes raphly 1usec

}
//-------------------------------------------------------------
//                      LCD
//-------------------------------------------------------------
// print letter (b) at location (start) on LCD
void print_b(char b, int start) {
    for (;start > 0; start--) {
        lcd_cursor_right();
    }

    lcd_data(b);
    cursor_off;
}
// print a number (num) of length (len) at location (start) with a fill character (fill)
void print_num(unsigned int num, int start, int len, char fill) {
    lcd_home();
    unsigned int i;
    char digit;
    for (i=start; i>0; i--) lcd_cursor_right();
    i=0;
    while (num != 0 && i < len) {
        digit  = num % 10 + 0x30;
        num /= 10;
        lcd_cursor_left();
        lcd_data(digit);
        lcd_cursor_left();
        i++;
    }

    while (i < len) {
        lcd_cursor_left();
        lcd_data(fill);
        lcd_cursor_left();
        i++;
    }
}
//-------------------------------------------------------------
//                      Timers
//-------------------------------------------------------------
void timer_enable() {
    switch(state) {
    case state1:
        TA0CTL |= MC_1;
    case state2:
        TA0CTL |= MC_1;
        TA1CTL |= MC_1;
    }
}

void timer_disable() {
    switch(state) {
    case state1:
        TA0CTL &= ~MC_1;
    case state2:
        TA0CTL &= ~MC_1;
        TA1CTL &= ~MC_1;
    }
}

void set_timer_value() {
    TA0CCR0 = X * 135;
    print_num(TA0CCR0, 16, 7, 0x30);
}
//---------------------------------------------------------------------
//                       ADC
//---------------------------------------------------------------------
// Begin sample
void adc_start_conversion(void) {
    ADC10CTL0 |= ADC10ON;         //turn on the ADC10
    ADC10CTL0 |= ENC | ADC10SC;   // enable conversion, start conversion

}
// Halt sample
void adc_stop_conversion(void) {
    ADC10CTL0 &= ~ (ENC | ADC10SC);
}
// Get potentiometer value
int pot_read(){  //
    return Nadc;
}
//---------------------------------------------------------------------
//                       UART
//---------------------------------------------------------------------
// Enable Transmission to PC

void uart_tx_enable(void) {
    IE2 |= UCA0TXIE;
}
// Disable Transmission to PC
void uart_tx_disable(void) {
    IE2 &= ~UCA0TXIE;
}
// Enable Receiving from PC
void uart_rx_enable(void) {
    IE2 |= UCA0RXIE;
}
// Disable Receiving from PC
void uart_rx_disable(void) {
    IE2 &= ~UCA0RXIE;
}
//---------------------------------------------------------------------
//                       ISRs
//---------------------------------------------------------------------
//*********************************************************************
//           ADC10 Interrupt Service Routine
//*********************************************************************
#pragma vector=ADC10_VECTOR
__interrupt void ADC_handler() {
    Nadc = ADC10MEM;
}
//*********************************************************************
//           Timer A0 Interrupt Service Routine
//*********************************************************************
#pragma vector=TIMER0_A1_VECTOR
__interrupt void timerA0_handler() {

    switch(state) {
    case state1:
        count = count==0xffff ? 0 : count+1;
        break;
    case state2:
        buzz_idx = buzz_idx==6 ? 0 : buzz_idx+1;
        TA1CCR0 = *(buzz_freq+buzz_idx);
        TA1CCR2 = TA1CCR0 >> 1; // 50% duty cycle
        break;
    }

    TA0CTL &= ~TAIFG;
}
//*********************************************************************
//            Port1 Interrupt Service Routine
//*********************************************************************
#pragma vector=PORT1_VECTOR
__interrupt void PBs_handler() {
    delay(debounceVal);

    if(PBsArrIntPend & BIT0){
      PB1_pressed = 1;
      PBsArrIntPend &= ~BIT0;
    }

    // Exit from a given LPM
    switch(lpm_mode){
        case mode0:
            LPM0_EXIT; // must be called from ISR only
            break;

        case mode1:
            LPM1_EXIT; // must be called from ISR only
            break;

        case mode2:
            LPM2_EXIT; // must be called from ISR only
            break;

        case mode3:
            LPM3_EXIT; // must be called from ISR only
            break;

        case mode4:
            LPM4_EXIT; // must be called from ISR only
            break;
    }
}
//*********************************************************************
//                           TX ISR
//*********************************************************************
#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR() {
    static unsigned int i = 0;

    if (quote[i] != '\0') {
        UCA0TXBUF = quote[i++];
    }
    else if (quote[i] == '\0') {
        UCA0TXBUF = '\n';
        PB1_pressed = 0;
        i = 0;
        IE2 &= ~UCA0TXIE;
        LPM0_EXIT;
    }
}
//*********************************************************************
//                         RX ISR
//*********************************************************************
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR() {

    if (!delay_ifg) {
        x_string[j++] = UCA0RXBUF;
//        if (j == 3) {
//            delay_ifg = !delay_ifg;
//            j = 0;
//        }
        if (UCA0RXBUF=='\n' || j==5) {
            delay_ifg = !delay_ifg;
            j=0;
        }
    }
    else {
        switch(UCA0RXBUF) {
            case '1':
                state = state1;
                break;
            case '2':
                state = state2;
                break;
            case '3':
                state = state3;
                delay_ifg = 0;
                break;
            case '4':
                state = state4;
                break;
            case '5':
                state = state5;
                break;
            case '6':
                state = state6;
                break;
            case '7':
                state = state7;
                break;
            case '8':
                state = state0;
                break;
        }
    }

  switch(lpm_mode){
  case mode0:
      LPM0_EXIT; // must be called from ISR only
      break;
  case mode1:
      LPM1_EXIT; // must be called from ISR only
      break;
  case mode2:
      LPM2_EXIT; // must be called from ISR only
      break;
  case mode3:
      LPM3_EXIT; // must be called from ISR only
      break;
  case mode4:
      LPM4_EXIT; // must be called from ISR only
      break;
  }
}





