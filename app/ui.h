#ifndef __UI_H
#define __UI_H

#include "sys.h"
#include "storage.h"

void ui_frame(void);
void ui_realtime(uint32_t voltage_mv, int32_t angle_x10, uint8_t fan_on, uint8_t beep_on);
void ui_history(const sample_t *data, uint32_t count);
void ui_clear_hint(void);

#endif
