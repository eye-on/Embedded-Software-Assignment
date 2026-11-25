#ifndef __BEEP_H
#define __BEEP_H

#include "sys.h"

extern volatile uint8_t beep_phase;
extern volatile uint8_t beep_enable;

void beep_init(void);
void beep_set_enable(uint8_t enable);

#endif
