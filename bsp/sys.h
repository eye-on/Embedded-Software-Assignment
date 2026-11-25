#ifndef __SYS_H
#define __SYS_H

#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define REG32(a) (*(volatile uint32_t *)(a))

#define RCC_CR        REG32(0x40023800u)
#define RCC_PLLCFGR   REG32(0x40023804u)
#define RCC_CFGR      REG32(0x40023808u)
#define RCC_AHB1ENR   REG32(0x40023830u)
#define RCC_APB1ENR   REG32(0x40023840u)
#define RCC_APB2ENR   REG32(0x40023844u)
#define RCC_AHB3ENR   REG32(0x40023838u)

#define FLASH_ACR     REG32(0x40023C00u)

#define GPIOA_MODER   REG32(0x40020000u)
#define GPIOA_PUPDR   REG32(0x4002000Cu)
#define GPIOA_IDR     REG32(0x40020010u)
#define GPIOA_AFRH    REG32(0x40020024u)

#define GPIOB_MODER   REG32(0x40020400u)
#define GPIOB_OTYPER  REG32(0x40020404u)
#define GPIOB_PUPDR   REG32(0x4002040Cu)
#define GPIOB_IDR     REG32(0x40020410u)
#define GPIOB_ODR     REG32(0x40020414u)

#define GPIOC_MODER   REG32(0x40020800u)
#define GPIOC_OTYPER  REG32(0x40020804u)
#define GPIOC_OSPEEDR REG32(0x40020808u)
#define GPIOC_PUPDR   REG32(0x4002080Cu)
#define GPIOC_ODR     REG32(0x40020814u)

#define GPIOD_MODER   REG32(0x40020C00u)
#define GPIOD_OSPEEDR REG32(0x40020C08u)
#define GPIOD_AFRL    REG32(0x40020C20u)
#define GPIOD_AFRH    REG32(0x40020C24u)

#define GPIOE_MODER   REG32(0x40021000u)
#define GPIOE_OSPEEDR REG32(0x40021008u)
#define GPIOE_PUPDR   REG32(0x4002100Cu)
#define GPIOE_IDR     REG32(0x40021010u)
#define GPIOE_ODR     REG32(0x40021014u)
#define GPIOE_AFRL    REG32(0x40021020u)
#define GPIOE_AFRH    REG32(0x40021024u)

#define GPIOF_MODER   REG32(0x40021400u)
#define GPIOF_OSPEEDR REG32(0x40021408u)
#define GPIOF_OTYPER  REG32(0x40021404u)
#define GPIOF_PUPDR   REG32(0x4002140Cu)
#define GPIOF_IDR     REG32(0x40021410u)
#define GPIOF_ODR     REG32(0x40021414u)
#define GPIOF_AFRL    REG32(0x40021420u)
#define GPIOF_AFRH    REG32(0x40021424u)

#define GPIOG_MODER   REG32(0x40021800u)
#define GPIOG_OSPEEDR REG32(0x40021808u)
#define GPIOG_AFRL    REG32(0x40021820u)
#define GPIOG_AFRH    REG32(0x40021824u)

#define USART1_SR     REG32(0x40011000u)
#define USART1_DR     REG32(0x40011004u)
#define USART1_BRR    REG32(0x40011008u)
#define USART1_CR1    REG32(0x4001100Cu)

#define SYST_CSR      REG32(0xE000E010u)
#define SYST_RVR      REG32(0xE000E014u)
#define SYST_CVR      REG32(0xE000E018u)
#define SCB_CPACR     REG32(0xE000ED88u)

#define FSMC_BCR4     REG32(0xA0000018u)
#define FSMC_BTR4     REG32(0xA000001Cu)

#define LCD_REG       (*(volatile uint16_t *)0x6C000000u)
#define LCD_RAM       (*(volatile uint16_t *)0x6C000080u)

#define LCD_W         480u
#define LCD_H         800u

#define LED0_PIN      (1u << 9)
#define LED1_PIN      (1u << 10)

extern volatile uint32_t g_ms;
extern uint32_t SystemCoreClock;

void fpu_enable(void);
void clock_168m(void);
void systick_init(void);
void delay_ms(uint32_t ms);

void uart_init(void);
void uart_putc(char c);
void uart_puts(const char *s);
void uprintf(const char *fmt, ...);

#endif
