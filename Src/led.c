/*
 ============================================================================
 Name        : led.c
 Author      : Krystian Heberlein
 Email       : krystianheberlein@gmail.com
 Description : LED operations
 ============================================================================
 */

#include "led.h"


void led_green_init()
{
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;
  GPIOG->MODER |= GPIO_MODER_MODE13_0;
  GPIOG->MODER &= ~(GPIO_MODER_MODE13_1);
  GPIOG->OTYPER &= ~(GPIO_OTYPER_OT13);
  GPIOG->PUPDR |= GPIO_PUPDR_PUPD13_0;
  GPIOG->PUPDR &= ~(GPIO_PUPDR_PUPD13_1);
}


void led_red_init()
{
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;
  GPIOG->MODER |= GPIO_MODER_MODE14_0;
  GPIOG->MODER &= ~(GPIO_MODER_MODE14_1);
  GPIOG->OTYPER &= ~(GPIO_OTYPER_OT14);
  GPIOG->PUPDR |= GPIO_PUPDR_PUPD14_0;
  GPIOG->PUPDR &= ~(GPIO_PUPDR_PUPD14_1);
}

void led_init()
{
  led_green_init();
  led_red_init();
  led_green_off();
  led_red_off();
}

void led_green_on() { GPIOG->BSRR |= GPIO_BSRR_BS13; }
void led_green_off() { GPIOG->BSRR |= GPIO_BSRR_BR13; }
void led_red_on() { GPIOG->BSRR |= GPIO_BSRR_BS14; }
void led_red_off() { GPIOG->BSRR |= GPIO_BSRR_BR14; }
