#ifndef __STORAGE_H
#define __STORAGE_H

#include "sys.h"

#define STORAGE_SIZE   10u
#define STORAGE_MAGIC  0x53544F52u

typedef struct {
    uint16_t mv;
    int16_t  ang_x10;
} sample_t;

void storage_load(sample_t *buf, uint32_t *count);
int  storage_save(const sample_t *buf, uint32_t count);
void storage_clear(void);

#endif
