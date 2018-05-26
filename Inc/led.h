/*
 ============================================================================
 Name        : led.h
 Author      : Krystian Heberlein
 Email       : krystianheberlein@gmail.com
 Description : LED operations interface
 ============================================================================
 */

#ifndef LED_H_
#define LED_H_

#include "stm32f429xx.h"

void led_init();

void led_green_on();
void led_green_off();
void led_red_on();
void led_red_off();


#endif /* LED_H_ */
