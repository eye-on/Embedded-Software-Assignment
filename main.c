#include "bsp/sys.h"
#include "bsp/lcd.h"
#include "bsp/led.h"
#include "bsp/key.h"
#include "bsp/adc.h"
#include "bsp/mag.h"
#include "bsp/beep.h"
#include "bsp/storage.h"
#include "app/app.h"

int main(void)
{
    fpu_enable();
    clock_168m();
    systick_init();
    uart_init();

    lcd_backlight(1);
    fsmc_init();
    lcd_init();
    lcd_gamma_fix();

    led_init();
    key_init();
    adc_init();
    beep_init();

    mag_init();

    app_init();
    app_run();

    return 0;
}
