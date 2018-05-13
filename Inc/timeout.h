/*
 ============================================================================
 Name        : timeout.h
 Author      : Krystian Heberlein
 Email       : krystianheberlein@gmail.com
 Description : Timeout timer on TIM2
 ============================================================================
 */

#ifndef TIMEOUT_H_
#define TIMEOUT_H_

#include "stm32f429xx.h"
#include "defs.h"
#include "stdbool.h"

void configure_timout_timer();
void set_timeout (uint8_t _timeout);
void reset_timeout();
bool has_timed_out();


#endif /* TIMEOUT_H_ */
