#include "sys.h"
#include "led.h"

#define FAN_PIN   (1u << 5)

void led_init(void)
{
    RCC_AHB1ENR |= (1u << 5) | (1u << 1);

    GPIOF_MODER = (GPIOF_MODER & ~((3u << 18) | (3u << 20))) |
                  ((1u << 18) | (1u << 20));

    GPIOB_MODER = (GPIOB_MODER & ~(3u << 10)) | (1u << 10);

    GPIOF_ODR |= (LED0_PIN | LED1_PIN);
    GPIOB_ODR &= ~FAN_PIN;
}

void led0_set(int on)
{
    if (on) {
        GPIOF_ODR &= ~LED0_PIN;
    } else {
        GPIOF_ODR |= LED0_PIN;
    }
}

void led1_set(int on)
{
    if (on) {
        GPIOF_ODR &= ~LED1_PIN;
    } else {
        GPIOF_ODR |= LED1_PIN;
    }
}

void fan_set(int on)
{
    if (on) {
        GPIOB_ODR |= FAN_PIN;
    } else {
        GPIOB_ODR &= ~FAN_PIN;
    }
}
