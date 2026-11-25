#ifndef __MAG_H
#define __MAG_H

#include "sys.h"

int   mag_init(void);
int   mag_read(int16_t *x, int16_t *y, int16_t *z);
float mag_heading_deg(void);

#endif
