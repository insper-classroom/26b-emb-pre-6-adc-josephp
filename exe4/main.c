/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


 #include <stdio.h>
 #include "pico/stdlib.h"
 #include "hardware/gpio.h"
 #include "hardware/timer.h"
 #include "hardware/adc.h"
 
 const int PIN_LED_B = 4;
 repeating_timer_t timer;
volatile int flg_timer = 0;
 bool timer_callback(repeating_timer_t *rt) {
    flg_timer=1;
    return true;
}
 const float conversion_factor = 3.3f / (1 << 12);
 
 /**
  * 0..1.0V: Desligado
  * 1..2.0V: 150 ms
  * 2..3.3V: 400 ms
  */
 int main() {
    stdio_init_all();
    adc_init();
    gpio_init(PIN_LED_B);
    gpio_set_dir(PIN_LED_B, GPIO_OUT);
    gpio_put(PIN_LED_B,0);
    adc_gpio_init(26);
    adc_select_input(0);
    int alarme_ligado1=0;
    int alarme_ligado2=0;
    int estado=0;

     while (1) {
        uint16_t valor = adc_read();
        float tensao = valor * 3.3f / 4096.0f;
        if(tensao<=1){
            if (alarme_ligado1 == 1 || alarme_ligado2 == 1) {
                cancel_repeating_timer(&timer);
            }
            alarme_ligado1=0;
            alarme_ligado2=0;
            gpio_put(PIN_LED_B, 0);
            estado = 0;
            flg_timer = 0;
        }
        else if(tensao<=2 && tensao>=1){
            if(alarme_ligado2==1){
                cancel_repeating_timer(&timer);
                alarme_ligado2=0;
            }
            if(alarme_ligado1==0){
                add_repeating_timer_ms(300,timer_callback,NULL,&timer);
                alarme_ligado1=1;
            }
        }
        else if(tensao<=3.3 && tensao>=2){
    
            if(alarme_ligado1==1){
                cancel_repeating_timer(&timer);
                alarme_ligado1=0;
            }
            if(alarme_ligado2==0){
                add_repeating_timer_ms(500,timer_callback,NULL,&timer);
                alarme_ligado2=1;
            }
        }
        if(flg_timer){
            flg_timer=0;
            estado=!estado;
            gpio_put(PIN_LED_B,estado);

        }









    }
}
