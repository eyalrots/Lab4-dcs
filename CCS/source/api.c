#include "../header/api.h"
#include <stdlib.h>
#include <string.h>

extern volatile FSM_state_t state;
extern volatile SYS_mode_t lpm_mode;

extern volatile unsigned int delay_ifg;

volatile int X, PB1_pressed, count;
volatile char x_string[5];

void count_on_LCD() {
    timer_enable();
    lcd_clear();
    while (state==state1) {
        print_num(count, 5, 5, 0x30);
    }
}

void start_buzzing() {
    lcd_clear();
    lcd_puts("Buzzing...");
    timer_enable();
    while (state==state2) {
        continue;
    }
}

void x_to_int() {
    lcd_clear();
    lcd_puts("state-3");
    while (state==state3) {
        if (delay_ifg) {
            X = atoi((const char *)x_string);
            X = X>=486 ? 485 : X;
            set_timer_value();
            return;
        }
    }
}

void disp_pot_val() {
    lcd_clear();
    lcd_puts("POT value:  .");
    int pot_val;
    adc_start_conversion();
    while (state==state4) {
        pot_val = pot_read() * 330 / 1023;
        print_num(pot_val/100, 12, 1, 0x30);
        print_num(pot_val%100, 15, 2, 0x30);
    }
    adc_stop_conversion();
    lcd_clear();
}

void my_negev() {
    lcd_clear();
    lcd_puts("state-5");
    while (state==state5) {
        enterLPM(mode0);
        if (PB1_pressed) {
            uart_tx_enable();
            enterLPM(mode0);
        }
        lcd_clear();
    }
    uart_tx_disable();
}

void zero_all() {
    lcd_clear();
    timer_disable();
    count = 0;
}
