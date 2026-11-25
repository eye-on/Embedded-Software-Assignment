#ifndef __KEY_H
#define __KEY_H

#include "sys.h"

#define KEY_EVENT_NONE   0u
#define KEY_EVENT_SHORT  1u
#define KEY_EVENT_LONG   2u

void    key_init(void);
uint8_t key_scan(void);

#endif
