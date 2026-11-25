TARGET   := app
BUILD    := build
LDSCRIPT := stm32f407zg.ld

SRCS     := main.c \
            app/app.c \
            app/ui.c \
            bsp/sys.c \
            bsp/lcd.c \
            bsp/adc.c \
            bsp/mag.c \
            bsp/led.c \
            bsp/key.c \
            bsp/beep.c \
            bsp/storage.c

OBJS     := $(BUILD)/startup_stm32f407xx.o $(SRCS:%.c=$(BUILD)/%.o)

PREFIX   := arm-none-eabi-
CC       := $(PREFIX)gcc
OBJCOPY  := $(PREFIX)objcopy
SIZE     := $(PREFIX)size

CPU      := -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
CFLAGS   := $(CPU) -O2 -g3 -Wall -Wextra -std=gnu11 \
            -ffunction-sections -fdata-sections -Ibsp
ASFLAGS  := $(CPU) -g3
LDFLAGS  := $(CPU) -T$(LDSCRIPT) -Wl,--gc-sections \
            -Wl,-Map=$(BUILD)/$(TARGET).map -Wl,--print-memory-usage \
            --specs=nosys.specs

OCD      := openocd -f interface/stlink.cfg -f target/stm32f4x.cfg

all: $(BUILD)/$(TARGET).elf $(BUILD)/$(TARGET).bin $(BUILD)/$(TARGET).hex

$(BUILD):
	mkdir -p $@

$(BUILD)/%.o: %.c | $(BUILD)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: %.s | $(BUILD)
	@mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) -c $< -o $@

$(BUILD)/$(TARGET).elf: $(OBJS) $(LDSCRIPT)
	$(CC) $(LDFLAGS) $(OBJS) -lm -o $@
	$(SIZE) $@

$(BUILD)/$(TARGET).bin: $(BUILD)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

$(BUILD)/$(TARGET).hex: $(BUILD)/$(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

flash: $(BUILD)/$(TARGET).elf
	$(OCD) -c "adapter speed 1000" \
	       -c "program $< verify reset exit"

erase:
	$(OCD) -c "adapter speed 1000" -c "init" -c "reset halt" \
	       -c "stm32f4x mass_erase 0" -c "reset run" -c "shutdown"

clean:
	rm -rf $(BUILD)

.PHONY: all flash erase clean
