#ifndef __ADC_H
#define __ADC_H

#include "sys.h"

void     adc_init(void);
uint32_t adc_read_mv(void);

#endif
