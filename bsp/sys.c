#include "sys.h"

uint32_t SystemCoreClock = 16000000u;
volatile uint32_t g_ms = 0;

void SysTick_Handler(void) { g_ms++; }

void fpu_enable(void)
{
    SCB_CPACR |= (0xFu << 20);
    __asm volatile ("dsb");
    __asm volatile ("isb");
}

void clock_168m(void)
{
    RCC_CR |= (1u << 16);
    while ((RCC_CR & (1u << 17)) == 0) { }

    FLASH_ACR = (FLASH_ACR & ~0x7u) | 5u;
    FLASH_ACR |= (1u << 8) | (1u << 9) | (1u << 10);

    RCC_PLLCFGR = 8u | (336u << 6) | (0u << 16) | (1u << 22) | (7u << 24);

    RCC_CR |= (1u << 24);
    while ((RCC_CR & (1u << 25)) == 0) { }

    uint32_t cfgr = RCC_CFGR;
    cfgr &= ~(0xFu << 4);
    cfgr &= ~(0x7u << 10); cfgr |= (5u << 10);
    cfgr &= ~(0x7u << 13); cfgr |= (4u << 13);
    cfgr &= ~0x3u;         cfgr |= 2u;
    RCC_CFGR = cfgr;

    while (((RCC_CFGR >> 2) & 0x3u) != 2u) { }
    SystemCoreClock = 168000000u;
}

void systick_init(void)
{
    SYST_RVR = (SystemCoreClock / 1000u) - 1u;
    SYST_CVR = 0;
    SYST_CSR = 7u;
}

void delay_ms(uint32_t ms)
{
    uint32_t s = g_ms;
    while ((g_ms - s) < ms) { }
}

void uart_init(void)
{
    RCC_AHB1ENR |= (1u << 0);
    RCC_APB2ENR |= (1u << 4);

    GPIOA_MODER &= ~((3u << 18) | (3u << 20));
    GPIOA_MODER |=  ((2u << 18) | (2u << 20));
    GPIOA_AFRH &= ~((0xFu << 4) | (0xFu << 8));
    GPIOA_AFRH |=  ((7u << 4) | (7u << 8));

    USART1_BRR = (SystemCoreClock / 2u) / 115200u;
    USART1_CR1 = (1u << 13) | (1u << 3) | (1u << 2);
}

void uart_putc(char c)
{
    while ((USART1_SR & (1u << 7)) == 0) { }
    USART1_DR = (uint8_t)c;
}

void uart_puts(const char *s) { while (*s) uart_putc(*s++); }

int _write(int fd, char *ptr, int len)
{
    (void)fd;
    for (int i = 0; i < len; i++) uart_putc(ptr[i]);
    return len;
}

void uprintf(const char *fmt, ...)
{
    char buf[128];
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    if (n > 0) {
        if (n > (int)sizeof(buf) - 1) n = (int)sizeof(buf) - 1;
        for (int i = 0; i < n; i++) uart_putc(buf[i]);
    }
}

