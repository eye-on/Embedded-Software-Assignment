#include "sys.h"
#include "key.h"

#define KEY0_PIN        (1u << 4)
#define KEY_LONG_MS     800u

static uint8_t  key_down_flag = 0;
static uint8_t  key_long_fired = 0;
static uint32_t key_down_ms = 0;

void key_init(void)
{
    RCC_AHB1ENR |= (1u << 4);

    GPIOE_MODER &= ~(3u << 8);
    GPIOE_PUPDR = (GPIOE_PUPDR & ~(3u << 8)) | (1u << 8);
}

uint8_t key_scan(void)
{
    uint8_t pressed = (GPIOE_IDR & KEY0_PIN) ? 0u : 1u;

    if (pressed && !key_down_flag) {
        key_down_flag = 1;
        key_long_fired = 0;
        key_down_ms = g_ms;
        return KEY_EVENT_NONE;
    }

    if (pressed && key_down_flag && !key_long_fired &&
        (g_ms - key_down_ms) >= KEY_LONG_MS) {
        key_long_fired = 1;
        return KEY_EVENT_LONG;
    }

    if (!pressed && key_down_flag) {
        key_down_flag = 0;

        if (!key_long_fired) {
            return KEY_EVENT_SHORT;
        }
    }

    return KEY_EVENT_NONE;
}
