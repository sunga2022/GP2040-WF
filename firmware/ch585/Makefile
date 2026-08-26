# GP2040-WF CH585 firmware
# USB high-speed HID gamepad at 8 kHz (bInterval=1).
#
# Toolchain: xpack riscv-none-elf-gcc (or WCH/MounRiver riscv-none-embed-gcc).
# Flash with WCHISPTool / wchisp over USB FS bootloader, or WCH-Link.

ROOT     := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
BUILD    := $(ROOT)/build
SDK      := $(ROOT)/sdk

PREFIX   ?= riscv64-unknown-elf-
CC       := $(PREFIX)gcc
OBJCOPY  := $(PREFIX)objcopy
SIZE     := $(PREFIX)size

ifeq ($(shell command -v $(CC) >/dev/null 2>&1; echo $$?),1)
  PREFIX := riscv-none-elf-
  CC     := $(PREFIX)gcc
  OBJCOPY:= $(PREFIX)objcopy
  SIZE   := $(PREFIX)size
endif

ARCH     := -march=rv32imac_zicsr_zifencei -mabi=ilp32
CFLAGS   := $(ARCH) -msmall-data-limit=8 -mno-save-restore
CFLAGS   += -Os -ffunction-sections -fdata-sections -fno-common -fsigned-char
CFLAGS   += -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable
CFLAGS   += -DINT_SOFT
CFLAGS   += -I$(SDK)/StdPeriphDriver/inc -I$(SDK)/RVMSIS -I$(ROOT)/include
CFLAGS   += -I$(ROOT)/include/libc -I$(ROOT)/../../wireless
LDFLAGS  := $(ARCH) -nostartfiles -nostdlib -T $(SDK)/Ld/Link.ld
LDFLAGS  += -Wl,--gc-sections -lgcc

SDK_SRC  := \
	$(SDK)/StdPeriphDriver/CH58x_gpio.c \
	$(SDK)/StdPeriphDriver/CH58x_clk.c \
	$(SDK)/StdPeriphDriver/CH58x_sys.c

COMMON_SRC := \
	$(ROOT)/src/ch585_usbhs_device.c \
	$(ROOT)/src/usb_desc.c \
	$(ROOT)/src/hid_gamepad.c \
	$(ROOT)/src/rf_link.c \
	$(ROOT)/src/minilib.c

STARTUP  := $(SDK)/Startup/startup_CH585.S

.PHONY: all controller dongle clean test

all: controller dongle

controller: $(BUILD)/controller.hex
dongle:     $(BUILD)/dongle.hex

test:
	python3 $(ROOT)/host_test/test_hid_pack.py
	python3 $(ROOT)/../../wireless/host_test/test_protocol.py

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/controller.elf: $(STARTUP) $(SDK_SRC) $(COMMON_SRC) $(ROOT)/src/controller.c | $(BUILD)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	$(SIZE) $@

$(BUILD)/dongle.elf: $(STARTUP) $(SDK_SRC) $(COMMON_SRC) $(ROOT)/src/dongle.c | $(BUILD)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	$(SIZE) $@

$(BUILD)/%.hex: $(BUILD)/%.elf
	$(OBJCOPY) -O ihex $< $@
	$(OBJCOPY) -O binary $< $(BUILD)/$*.bin

clean:
	rm -rf $(BUILD)
