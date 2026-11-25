#include "sys.h"
#include "adc.h"

#define ADC1_SR      REG32(0x40012000u)
#define ADC1_CR1     REG32(0x40012004u)
#define ADC1_CR2     REG32(0x40012008u)
#define ADC1_SMPR2   REG32(0x40012010u)
#define ADC1_SQR1    REG32(0x4001202Cu)
#define ADC1_SQR3    REG32(0x40012034u)
#define ADC1_DR      REG32(0x4001204Cu)
#define ADC_CCR      REG32(0x40012304u)

void adc_init(void)
{
    RCC_APB2ENR |= (1u << 8);
    RCC_AHB1ENR |= (1u << 0);

    GPIOA_MODER |= (3u << 10);

    ADC_CCR = (ADC_CCR & ~(3u << 16)) | (1u << 16);

    ADC1_CR1 = 0;
    ADC1_SMPR2 = (ADC1_SMPR2 & ~(7u << 15)) | (4u << 15);
    ADC1_SQR1 = 0;
    ADC1_SQR3 = 5;

    ADC1_CR2 = (1u << 0);

    delay_ms(2);
}

uint32_t adc_read_mv(void)
{
    uint32_t raw;
    uint32_t guard = 200000u;

    ADC1_CR2 |= (1u << 30);

    while ((ADC1_SR & (1u << 1)) == 0) {
        if (guard-- == 0u) {
            return 0;
        }
    }

    raw = ADC1_DR & 0x0FFFu;

    return (raw * 3300u) / 4095u;
}
