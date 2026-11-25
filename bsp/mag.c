#include "sys.h"
#include "mag.h"

uint32_t g_scl   = (1u << 8);
uint32_t g_sda   = (1u << 9);
uint32_t g_scl_sh = 8 * 2;
uint32_t g_sda_sh = 9 * 2;

#define MSCL_H()  (GPIOB_ODR |=  g_scl)
#define MSCL_L()  (GPIOB_ODR &= ~g_scl)

#define MSDA_H()  (GPIOB_ODR |=  g_sda)
#define MSDA_L()  (GPIOB_ODR &= ~g_sda)
#define MSDA_RD() ((GPIOB_IDR & g_sda) ? 1 : 0)

void sda_out(void)
{
    GPIOB_MODER = (GPIOB_MODER & ~(3u << g_sda_sh)) | (1u << g_sda_sh);
}

void sda_in(void)
{
    GPIOB_MODER &= ~(3u << g_sda_sh);
}

float gx_bias = 0.0f, gy_bias = 0.0f, gz_bias = 0.0f;
volatile uint32_t g_bus_idle = 0;

void md(void)
{
    for (volatile int i = 0; i < 100; i++) { __asm volatile ("nop"); }
}

void mpu_gpio_init(uint32_t scl, uint32_t sda)
{
    g_scl = scl;
    g_sda = sda;

    RCC_AHB1ENR |= (1u << 1);

    uint32_t ps = 0, pd = 0;
    while ((1u << ps) != scl) ps++;
    while ((1u << pd) != sda) pd++;

    uint32_t mask = (3u << (ps * 2)) | (3u << (pd * 2));

    g_scl_sh = ps * 2;
    g_sda_sh = pd * 2;

    GPIOB_MODER = (GPIOB_MODER & ~mask) |
                  ((1u << (ps * 2)) | (1u << (pd * 2)));
    GPIOB_OTYPER &= ~(scl | sda);
    GPIOB_PUPDR = (GPIOB_PUPDR & ~mask) |
                  ((1u << (ps * 2)) | (1u << (pd * 2)));
    GPIOB_ODR |= scl | sda;

    delay_ms(2);
    g_bus_idle = ((GPIOB_IDR & scl) ? 2u : 0u) |
                 ((GPIOB_IDR & sda) ? 1u : 0u);
}

void m_start(void) { MSDA_H(); MSCL_H(); md(); MSDA_L(); md(); MSCL_L(); md(); }
void m_stop(void)  { MSDA_L(); md(); MSCL_H(); md(); MSDA_H(); md(); }

int m_ack(void)
{
    int a;

    sda_in();
    md();
    MSCL_H(); md();
    a = MSDA_RD();
    MSCL_L(); md();
    sda_out();

    return a;
}

void m_wr(uint8_t b)
{
    for (int i = 0; i < 8; i++) {
        if (b & 0x80) MSDA_H(); else MSDA_L();
        md(); MSCL_H(); md(); MSCL_L();
        b <<= 1;
    }
}

uint8_t m_rd(int ack)
{
    uint8_t v = 0;

    sda_in();
    for (int i = 0; i < 8; i++) {
        md(); MSCL_H(); md();
        v = (uint8_t)((v << 1) | (MSDA_RD() ? 1 : 0));
        MSCL_L();
    }

    sda_out();
    if (ack) MSDA_L(); else MSDA_H();
    md(); MSCL_H(); md(); MSCL_L(); md();

    return v;
}

uint8_t mpu_addr = 0x68;
volatile uint32_t g_scan_addr = 0;
volatile uint32_t g_probe_bits = 0;

int i2c_probe(uint8_t a7)
{
    int nack;
    m_start();
    m_wr((uint8_t)(a7 << 1));
    nack = m_ack();
    m_stop();
    return nack;
}

uint8_t i2c_scan(void)
{
    for (uint8_t a = 0x08; a < 0x78; a++) {
        if (i2c_probe(a) == 0) return a;
    }
    return 0;
}

void probe_all(void)
{
    uint32_t bits = 0;

    if (i2c_probe(0x50) == 0) bits |= (1u << 0);
    if (i2c_probe(0x50) == 0) bits |= (1u << 1);
    if (i2c_probe(0x68) == 0) bits |= (1u << 2);
    if (i2c_probe(0x69) == 0) bits |= (1u << 3);

    g_probe_bits = bits;
}

int mpu_wr(uint8_t reg, uint8_t val)
{
    m_start();
    m_wr((uint8_t)(mpu_addr << 1));
    if (m_ack()) { m_stop(); return 1; }
    m_wr(reg); m_ack();
    m_wr(val); m_ack();
    m_stop();
    return 0;
}

int mpu_rd(uint8_t reg, uint8_t *buf, int len)
{
    m_start();
    m_wr((uint8_t)(mpu_addr << 1));
    if (m_ack()) { m_stop(); return 1; }
    m_wr(reg); m_ack();

    m_start();
    m_wr((uint8_t)((mpu_addr << 1) | 1));
    if (m_ack()) { m_stop(); return 1; }

    for (int i = 0; i < len; i++) {
        buf[i] = m_rd(i < (len - 1));
    }
    m_stop();
    return 0;
}

volatile uint8_t  g_found[16];
volatile uint32_t g_found_n = 0;
volatile uint32_t g_who[3] = {0, 0, 0};

volatile uint32_t g_bus_cnt[3]  = {0, 0, 0};
volatile uint32_t g_bus_addr[3] = {0, 0, 0};

#define IMU_ADDR   0x0C

volatile uint32_t g_mag_ok = 0;
volatile uint32_t g_mag_err = 0;

int mag_init(void)
{
    mpu_gpio_init(1u << 8, 1u << 9);
    delay_ms(50);

    mpu_addr = IMU_ADDR;

    uint8_t st = 0;

    for (int i = 0; i < 10; i++) {
        if (mpu_rd(0xF0, &st, 1) == 0) {
            g_mag_ok = 1;
            delay_ms(20);
            return 0;
        }
        delay_ms(20);
    }

    g_mag_ok = 0;
    return 1;
}

int mag_read(int16_t *x, int16_t *y, int16_t *z)
{
    uint8_t b[7] = {0};

    if (mpu_rd(0x3E, b, 1) != 0) { g_mag_err++; return 1; }
    delay_ms(15);
    if (mpu_rd(0x4E, b, 7) != 0) { g_mag_err++; return 2; }

    if (b[0] & 0x10) { g_mag_err++; return 3; }

    *x = (int16_t)((b[1] << 8) | b[2]);
    *y = (int16_t)((b[3] << 8) | b[4]);
    *z = (int16_t)((b[5] << 8) | b[6]);
    return 0;
}

float mag_heading_deg(void)
{
    int16_t x;
    int16_t y;
    int16_t z;

    if (mag_read(&x, &y, &z) != 0) {
        return -1.0f;
    }

    float h = atan2f((float)y, (float)x) * 57.2957795f;

    if (h < 0.0f) {
        h += 360.0f;
    }

    return h;
}
