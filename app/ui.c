#include "sys.h"
#include "lcd.h"
#include "ui.h"

#define COLOR_LABEL   0x7BEFu
#define COLOR_VALUE   0xFFFFu
#define COLOR_ANGLE   0x07E0u
#define COLOR_ON      0xF800u
#define COLOR_OFF     0x7BEFu
#define COLOR_BEEP    0xFFE0u

static char shown_volt[16] = "";
static char shown_angle[16] = "";
static char shown_fan[8] = "";
static char shown_beep[8] = "";

static void format_voltage(char *buf, uint32_t mv)
{
    sprintf(buf, "%lu.%02luV",
            (unsigned long)(mv / 1000u),
            (unsigned long)((mv % 1000u) / 10u));
}

static void format_angle(char *buf, int32_t tenths)
{
    sprintf(buf, "%ld.%ld",
            (long)(tenths / 10),
            (long)(tenths % 10));
}

static void draw_label(uint16_t y, const char *text)
{
    lcd_draw_text(20, y, text, 2, COLOR_LABEL);
}

static void draw_field(uint16_t y, const char *text, uint16_t color)
{
    lcd_fill_rect(10, y, 330, (uint16_t)(y + 28u), 0x0000);
    lcd_draw_text(20, y, text, 3, color);
}

void ui_frame(void)
{
    lcd_fill_rect(0, 0, LCD_W - 1, LCD_H - 1, 0x0000);

    draw_label(24, "VOLT");
    draw_label(160, "ANG");
    draw_label(296, "FAN");
    draw_label(432, "BEEP");

    lcd_draw_text(20, 700, "KEY0", 2, COLOR_LABEL);

    shown_volt[0] = 0;
    shown_angle[0] = 0;
    shown_fan[0] = 0;
    shown_beep[0] = 0;
}

void ui_realtime(uint32_t voltage_mv, int32_t angle_x10, uint8_t fan_on, uint8_t beep_on)
{
    char buf[20];

    format_voltage(buf, voltage_mv);
    if (strcmp(buf, shown_volt) != 0) {
        strcpy(shown_volt, buf);
        draw_field(60, buf, COLOR_VALUE);
    }

    format_angle(buf, angle_x10);
    if (strcmp(buf, shown_angle) != 0) {
        strcpy(shown_angle, buf);
        draw_field(196, buf, COLOR_ANGLE);
    }

    strcpy(buf, fan_on ? "ON" : "OFF");
    if (strcmp(buf, shown_fan) != 0) {
        strcpy(shown_fan, buf);
        draw_field(332, buf, fan_on ? COLOR_ON : COLOR_OFF);
    }

    strcpy(buf, beep_on ? "ON" : "OFF");
    if (strcmp(buf, shown_beep) != 0) {
        strcpy(shown_beep, buf);
        draw_field(468, buf, beep_on ? COLOR_BEEP : COLOR_OFF);
    }
}

void ui_history(const sample_t *data, uint32_t count)
{
    char buf[24];

    lcd_fill_rect(0, 0, LCD_W - 1, LCD_H - 1, 0x0000);

    lcd_draw_text(20, 20, "HIST", 3, COLOR_VALUE);

    for (uint32_t i = 0; i < count && i < STORAGE_SIZE; i++) {
        uint16_t y = (uint16_t)(100u + i * 55u);

        sprintf(buf, "%lu", (unsigned long)(i + 1u));
        lcd_draw_text(20, y, buf, 2, COLOR_LABEL);

        sprintf(buf, "%lu.%02luV",
                (unsigned long)(data[i].mv / 1000u),
                (unsigned long)((data[i].mv % 1000u) / 10u));
        lcd_draw_text(90, y, buf, 2, COLOR_VALUE);

        sprintf(buf, "%ld.%ld",
                (long)(data[i].ang_x10 / 10),
                (long)(data[i].ang_x10 % 10));
        lcd_draw_text(280, y, buf, 2, COLOR_ANGLE);
    }
}

void ui_clear_hint(void)
{
    lcd_draw_text(20, 640, "CLR", 3, COLOR_ON);
}
