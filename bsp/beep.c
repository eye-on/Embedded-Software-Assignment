#include "sys.h"
#include "beep.h"

#define TIM3_CR1     REG32(0x40000400u)
#define TIM3_DIER    REG32(0x4000040Cu)
#define TIM3_SR      REG32(0x40000410u)
#define TIM3_EGR     REG32(0x40000414u)
#define TIM3_PSC     REG32(0x40000428u)
#define TIM3_ARR     REG32(0x4000042Cu)
#define NVIC_ISER0   REG32(0xE000E100u)

volatile uint8_t beep_phase = 0;
volatile uint8_t beep_enable = 0;

void beep_init(void)
{
    RCC_APB1ENR |= (1u << 1);

    TIM3_PSC = 8399u;
    TIM3_ARR = 9999u;

    TIM3_EGR = 1;
    TIM3_SR = 0;
    TIM3_DIER |= 1;

    NVIC_ISER0 |= (1u << 29);

    TIM3_CR1 |= 1;
}

void TIM3_IRQHandler(void)
{
    if (TIM3_SR & 1u) {
        TIM3_SR = 0;

        if (beep_enable) {
            beep_phase = (uint8_t)!beep_phase;
        } else {
            beep_phase = 0;
        }
    }
}
