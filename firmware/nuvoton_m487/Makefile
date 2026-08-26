# GP2040-WF — Nuvoton M487 USB High Speed HID gamepad (8 kHz)
#
# Toolchain: arm-none-eabi-gcc (Ubuntu gcc-arm-none-eabi or GNU Arm Embedded).
# Flash with Nu-Link on the NuMaker-PFM-M487 ICE USB, then play on CON1 (USB HS).

ROOT     := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
BUILD    := $(ROOT)/build
BSP      := $(ROOT)/bsp

PREFIX   ?= arm-none-eabi-
CC       := $(PREFIX)gcc
OBJCOPY  := $(PREFIX)objcopy
SIZE     := $(PREFIX)size
CC_HOST  ?= gcc

CPU      := -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
CFLAGS   := $(CPU) -std=gnu11 -Os -ffunction-sections -fdata-sections
CFLAGS   += -fno-builtin -fshort-enums -funsigned-char
CFLAGS   += -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable
CFLAGS   += -I$(BSP)/CMSIS/Include
CFLAGS   += -I$(BSP)/Device/Include
CFLAGS   += -I$(BSP)/StdDriver/inc
CFLAGS   += -I$(ROOT)/include
CFLAGS   += -I$(ROOT)/../../wireless

LDFLAGS  := $(CPU) --specs=nano.specs --specs=nosys.specs
LDFLAGS  += -T $(BSP)/Device/Source/GCC/gcc_arm_160k.ld
LDFLAGS  += -Wl,--gc-sections -Wl,-Map=$(BUILD)/controller.map

BSP_SRC  := \
	$(BSP)/Device/Source/system_M480.c \
	$(BSP)/StdDriver/src/clk.c \
	$(BSP)/StdDriver/src/sys.c \
	$(BSP)/StdDriver/src/gpio.c \
	$(BSP)/StdDriver/src/hsusbd.c

APP_SRC  := \
	$(ROOT)/src/main.c \
	$(ROOT)/src/hid_gamepad.c \
	$(ROOT)/src/hid_usb.c \
	$(ROOT)/src/descriptors.c \
	$(ROOT)/src/hardfault.c \
	$(ROOT)/src/bt_uart.c

STARTUP  := $(BSP)/Device/Source/GCC/startup_M480.S
SRCS     := $(STARTUP) $(BSP_SRC) $(APP_SRC)

.PHONY: all controller clean test

all: controller

controller: $(BUILD)/controller.hex

test: | $(BUILD)
	python3 $(ROOT)/host_test/test_hid_pack.py
	python3 $(ROOT)/host_test/test_interval.py
	python3 $(ROOT)/../../wireless/host_test/test_protocol.py
	$(CC_HOST) -I$(ROOT)/../../wireless -o $(BUILD)/test_uart_rx $(ROOT)/../../wireless/host_test/test_uart_rx.c
	$(BUILD)/test_uart_rx

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/controller.elf: $(SRCS) | $(BUILD)
	$(CC) $(CFLAGS) -o $@ $(SRCS) $(LDFLAGS)
	$(SIZE) $@

$(BUILD)/controller.hex: $(BUILD)/controller.elf
	$(OBJCOPY) -O ihex $< $@
	$(OBJCOPY) -O binary $< $(BUILD)/controller.bin

clean:
	rm -rf $(BUILD)
