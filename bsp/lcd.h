#ifndef __LCD_H
#define __LCD_H

#include "sys.h"

void     lcd_backlight(int on);
void     fsmc_init(void);
uint16_t lcd_read_id(void);
void     lcd_init(void);
void     lcd_gamma_fix(void);

void lcd_fill_rect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void lcd_draw_text(uint16_t x, uint16_t y, const char *s, uint16_t scale, uint16_t color);
void lcd_draw_uint(uint16_t x, uint16_t y, uint32_t v, uint16_t scale, uint16_t color);
void lcd_draw_int(uint16_t x, uint16_t y, int32_t v, uint16_t scale, uint16_t color);
void lcd_draw_fixed(uint16_t x, uint16_t y, uint32_t centi, uint16_t scale, uint16_t color);

#endif
