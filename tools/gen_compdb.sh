#!/bin/sh
set -eu

ROOT=$(pwd)
FLAGS="-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -O2 -g3 -std=gnu11 -ffunction-sections -fdata-sections -I$ROOT/bsp -I$ROOT/app"

{
    echo '['
    first=1
    for f in main.c app/app.c app/ui.c bsp/sys.c bsp/lcd.c bsp/adc.c bsp/mag.c bsp/led.c bsp/key.c bsp/beep.c bsp/storage.c; do
        if [ "$first" -eq 1 ]; then
            first=0
        else
            echo ','
        fi
        printf '{"directory":"%s","file":"%s/%s","command":"arm-none-eabi-gcc %s -c %s"}' \
            "$ROOT" "$ROOT" "$f" "$FLAGS" "$f"
    done
    echo
    echo ']'
} > "$ROOT/compile_commands.json"

echo "compile_commands.json generated at $ROOT"
