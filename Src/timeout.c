/*
 ============================================================================
 Name        : timeout.c
 Author      : Krystian Heberlein
 Email       : krystianheberlein@gmail.com
 Description : Timeout timer on TIM2
 ============================================================================
 */


#include "timeout.h"

volatile int timer_cnt;

static void clock_init()
{
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
}

/*
 * Upcounting mode In upcounting mode, the counter counts from 0 to the auto-reload value
 * (content of the TIMx_ARR register), then restarts from 0 and generates a counter overflow event.
 */

static void timer_init(uint8_t _timeout)
{
  timer_cnt = _timeout;
  // TIM2 CLK 7.5 MHz
  TIM2->ARR = 0x7270E0;
  TIM2->DIER |= TIM_DIER_UIE;
  TIM2->CR1 |= TIM_CR1_CEN;
}

static void timer_deinit()
{
  TIM2->DIER &= ~(TIM_DIER_UIE);
  TIM2->CR1 &= ~(TIM_CR1_CEN);
}


void TIM2_IRQHandler()
{
  if (TIM2->SR & TIM_SR_UIF_Msk){
    timer_cnt--;
    if (timer_cnt < 0) {
      timer_deinit();
    } else {
      TIM2->SR &= ~(TIM_SR_UIF);
    }
  }
}

void configure_timout_timer()
{
  uint32_t prioritygroup = NVIC_GetPriorityGrouping();
  uint32_t priority = NVIC_EncodePriority(prioritygroup, 0, 1 );
  NVIC_SetPriority(TIM2_IRQn, priority);
  NVIC_EnableIRQ(TIM2_IRQn);
}


void set_timeout (uint8_t _timeout)
{
  clock_init();
  timer_init(_timeout);
}

void reset_timeout()
{
  timer_deinit();
}

bool has_timed_out()
{
  return (timer_cnt > 0) ? false : true;
}
