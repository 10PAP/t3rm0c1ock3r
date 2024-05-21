#include "adc.h"
#include "buttons.h"
#include "display.h"
#include "leds.h"
#include "rtc.h"
#include "spi.h"
#include "stm32f0xx.h"
#include "systick.h"
#include "displayHD.h"

#include <stdio.h>

int main() {
  init_leds();
  init_btns();
  init_systick();
  init_spi();
  init_display();
  init_adc();
  display[0] = 0x1;
  init_rtc();
  init_displayHD();
  static uint8_t last_read_btn[2][2] = {{0, 0}, {0, 0}};
  static int8_t dx_btn[2][2] = {{-1, 1}, {0, 0}};
  static int8_t dy_btn[2][2] = {{0, 0}, {1, -1}};

  static uint8_t last_read_user_btn = 0;

  static uint8_t setTimeMode = 0;
  //display_draw_player(&player);
  while (1) {
    // if (ADC_accumulated && ADC_half != -1) {
    //   uint32_t level = 0;
    //   if (ADC_half == 0) {
    //     for (int i = 0; i < ADC_DMA_BUFFSIZE / 2; ++i)
    //       level += (uint32_t)ADC_array[i];
    //   } else {
    //     for (int i = ADC_DMA_BUFFSIZE / 2; i < ADC_DMA_BUFFSIZE; ++i)
    //       level += (uint32_t)ADC_array[i];
    //   }
    //   level /= (ADC_DMA_BUFFSIZE / 2);
    //   ADC_half = -1;
    //   ADC_accumulated = 0;
    //   draw_level(level);
    // }

    uint8_t user_btn_pressed = !user_btn_state && last_read_user_btn;
    last_read_user_btn = user_btn_state;
    if (user_btn_pressed) {
      setTimeMode = setTimeMode == 0 ? 1 : 0;
      displayHDToggleBlink(setTimeMode);
    }
    
    if (RTC->ISR & RTC_ISR_RSF) {
      sTime currTime;
      rtcDecode(RTC->TR, &currTime);
      display[0] = currTime.seconds;
      display[1] = currTime.minutes;
      if (!setTimeMode) {
        char buffer[256];
        sprintf(buffer, "%02d:%02d", currTime.minutes, currTime.seconds);
        displayHDSendString(buffer);
      }
    }


    for (int i = 0; i < 2; ++i) {
     for (int j = 0; j < 2; ++j) {
       uint8_t btn_pressed = !btn_mat_state[i][j] && last_read_btn[i][j];
       last_read_btn[i][j] = btn_mat_state[i][j];
       if (!btn_pressed)
         continue;
       // led_state[i][j] = led_state[i][j] ? 0 : 1;
       display_draw_player(&player);
       player.x = (player.x + dx_btn[i][j]) & 0x7;
       player.y = (player.y + dy_btn[i][j]) & 0x7;
       display_draw_player(&player);
     }
    }

    leds_refresh();
  }
}
