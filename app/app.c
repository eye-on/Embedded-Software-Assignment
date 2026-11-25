#include "sys.h"
#include "led.h"
#include "key.h"
#include "adc.h"
#include "mag.h"
#include "beep.h"
#include "storage.h"
#include "app.h"
#include "ui.h"

#define VOLT_THRESHOLD_MV     1500u
#define ANGLE_THRESHOLD_X10   500
#define ADC_PERIOD_MS         100u
#define MAG_PERIOD_MS         200u
#define DISPLAY_PERIOD_MS     250u
#define SAMPLE_PERIOD_MS      1000u
#define SAVE_EVERY_SAMPLES    10u
#define BLINK_PERIOD_MS       50u

static sample_t hist_data[STORAGE_SIZE];
static uint32_t hist_count = 0;
static uint32_t save_tick = 0;

static uint32_t voltage_mv = 0;
static int32_t  angle_x10 = 0;
static uint8_t  fan_on = 0;
static uint8_t  angle_alarm = 0;
static uint8_t  view_mode = 0;

static uint32_t adc_last_ms = 0;
static uint32_t mag_last_ms = 0;
static uint32_t display_last_ms = 0;
static uint32_t sample_last_ms = 0;

static void push_sample(void)
{
    sample_t s;

    s.mv = (uint16_t)voltage_mv;
    s.ang_x10 = (int16_t)angle_x10;

    if (hist_count < STORAGE_SIZE) {
        hist_data[hist_count] = s;
        hist_count++;
    } else {
        for (uint32_t i = 1; i < STORAGE_SIZE; i++) {
            hist_data[i - 1] = hist_data[i];
        }
        hist_data[STORAGE_SIZE - 1] = s;
    }

    save_tick++;

    if (save_tick >= SAVE_EVERY_SAMPLES) {
        save_tick = 0;
        storage_save(hist_data, hist_count);
    }
}

static void report_uart(void)
{
    char vb[16];
    char ab[16];

    sprintf(vb, "%lu.%02luV",
            (unsigned long)(voltage_mv / 1000u),
            (unsigned long)((voltage_mv % 1000u) / 10u));

    sprintf(ab, "%ld.%ld",
            (long)(angle_x10 / 10),
            (long)(angle_x10 % 10));

    uprintf("V=%s ANG=%s FAN=%s BEEP=%s\r\n",
            vb, ab,
            fan_on ? "ON" : "OFF",
            beep_phase ? "ON" : "OFF");
}

static void handle_key(void)
{
    uint8_t ev = key_scan();

    if (ev == KEY_EVENT_SHORT) {
        view_mode = (uint8_t)!view_mode;

        if (view_mode) {
            ui_history(hist_data, hist_count);
        } else {
            ui_frame();
            ui_realtime(voltage_mv, angle_x10, fan_on, beep_phase);
        }
    } else if (ev == KEY_EVENT_LONG) {
        storage_clear();
        hist_count = 0;
        save_tick = 0;

        if (view_mode) {
            ui_history(hist_data, hist_count);
        }

        ui_clear_hint();
    }
}

void app_init(void)
{
    storage_load(hist_data, &hist_count);

    if (hist_count > STORAGE_SIZE) {
        hist_count = STORAGE_SIZE;
    }

    view_mode = 0;

    ui_frame();
    ui_realtime(voltage_mv, angle_x10, fan_on, beep_phase);
}

void app_run(void)
{
    for (;;) {
        uint32_t now = g_ms;

        if ((now - adc_last_ms) >= ADC_PERIOD_MS) {
            adc_last_ms = now;

            voltage_mv = adc_read_mv();
            fan_on = (voltage_mv > VOLT_THRESHOLD_MV) ? 1u : 0u;
            beep_set_enable(fan_on);

            fan_set(fan_on);
        }

        if ((now - mag_last_ms) >= MAG_PERIOD_MS) {
            mag_last_ms = now;

            float h = mag_heading_deg();

            if (h >= 0.0f) {
                angle_x10 = (int32_t)(h * 10.0f);
                angle_alarm = (angle_x10 > ANGLE_THRESHOLD_X10) ? 1u : 0u;
            }
        }

        if (angle_alarm) {
            led1_set((int)((now / BLINK_PERIOD_MS) & 1u));
        } else {
            led1_set(0);
        }

        if ((now - sample_last_ms) >= SAMPLE_PERIOD_MS) {
            sample_last_ms = now;

            push_sample();
            report_uart();
        }

        if ((now - display_last_ms) >= DISPLAY_PERIOD_MS) {
            display_last_ms = now;

            if (view_mode == 0u) {
                ui_realtime(voltage_mv, angle_x10, fan_on, beep_phase);
            }
        }

        handle_key();

        delay_ms(10);
    }
}
