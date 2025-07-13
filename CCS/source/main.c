#include <msp430g2553.h>
#include "../header/api.h"

volatile FSM_state_t state;
volatile SYS_mode_t lpm_mode;

extern volatile unsigned int delay_ifg;

int main(void)
{
  volatile unsigned int i;
  WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer

  state = state0;
  lpm_mode = mode0;

  system_config();
  lcd_init();
  lcd_clear();

  while(1) {
    switch (state) {
      case state0:
        timer_disable();
        enterLPM(mode0);
        break;
      case state1:
          count_on_LCD();
          timer_disable();
          break;
      case state2:
          start_buzzing();
          timer_disable();
          break;
      case state3:
          x_to_int();
          state=state0;
          break;
      case state4:
          disp_pot_val();
          break;
      case state5:
          my_negev();
          break;
      case state6:
          zero_all();
          state=state0;
          break;
      case state8:
          print_on_lcd();
      default:
        state=state0;
        break;
    }
  }
}

