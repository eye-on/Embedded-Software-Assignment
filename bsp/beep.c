#include "sys.h"
#include "beep.h"

#define TIM3_CR1     REG32(0x40000400u)
#define TIM3_DIER    REG32(0x4000040Cu)
#define TIM3_SR      REG32(0x40000410u)
#define TIM3_EGR     REG32(0x40000414u)
#define TIM3_PSC     REG32(0x40000428u)
#define TIM3_ARR     REG32(0x4000042Cu)
#define NVIC_ISER0   REG32(0xE000E100u)

#define BEEP_PIN     (1u << 8)

volatile uint8_t beep_phase = 0;
volatile uint8_t beep_enable = 0;

void beep_init(void)
{
    RCC_AHB1ENR |= (1u << 5);

    GPIOF_MODER = (GPIOF_MODER & ~(3u << 16)) | (1u << 16);
    GPIOF_ODR &= ~BEEP_PIN;

    RCC_APB1ENR |= (1u << 1);

    TIM3_PSC = 8399u;
    TIM3_ARR = 9999u;

    TIM3_EGR = 1;
    TIM3_SR = 0;
    TIM3_DIER |= 1;

    NVIC_ISER0 |= (1u << 29);

    TIM3_CR1 |= 1;
}

void beep_set_enable(uint8_t enable)
{
    beep_enable = enable;

    if (!enable) {
        beep_phase = 0;
        GPIOF_ODR &= ~BEEP_PIN;
    }
}

void TIM3_IRQHandler(void)
{
    if (TIM3_SR & 1u) {
        TIM3_SR = 0;

        if (beep_enable) {
            beep_phase = (uint8_t)!beep_phase;

            if (beep_phase) {
                GPIOF_ODR |= BEEP_PIN;
            } else {
                GPIOF_ODR &= ~BEEP_PIN;
            }
        } else {
            beep_phase = 0;
            GPIOF_ODR &= ~BEEP_PIN;
        }
    }
}
