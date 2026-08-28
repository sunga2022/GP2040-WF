# One-chip 2.4G USB-A dongle: nRF52820 QFN40 5x5
# USB + nRF24-compatible radio on the same die. Stick keeps XN297L/Si24R1.

ROOT    := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
REPO    := $(abspath $(ROOT)/../..)
TUSB    := $(REPO)/lib/tinyusb
NRFX    := $(ROOT)/third_party/nrfx
CMSIS   := $(ROOT)/third_party/CMSIS/Core/Include

PREFIX  ?= arm-none-eabi-
CC      := $(PREFIX)gcc
OBJCOPY := $(PREFIX)objcopy
SIZE    := $(PREFIX)size

MCU     := nrf52820
CFLAGS  := -mcpu=cortex-m4 -mthumb -mfloat-abi=soft
CFLAGS  += -Os -ffunction-sections -fdata-sections -fno-common
CFLAGS  += -Wall -Wextra -Wno-unused-parameter
CFLAGS  += -DNRF52820_XXAA -DCFG_TUSB_MCU=OPT_MCU_NRF5X
CFLAGS  += -DCONFIG_GPIO_AS_PINRESET -D__STARTUP_CLEAR_BSS -D__START=main
CFLAGS  += -I$(ROOT) -I$(REPO)/wireless
CFLAGS  += -I$(TUSB)/src
CFLAGS  += -I$(NRFX) -I$(NRFX)/mdk -I$(NRFX)/hal -I$(NRFX)/drivers/include
CFLAGS  += -I$(NRFX)/drivers/src -I$(NRFX)/helpers -I$(NRFX)/soc -I$(NRFX)/templates
CFLAGS  += -I$(CMSIS)

LDFLAGS := $(CFLAGS) -nostartfiles --specs=nosys.specs --specs=nano.specs
LDFLAGS += -Wl,--gc-sections -Wl,-Map=$(ROOT)/build/gp2040wf_receiver.map
LDFLAGS += -L$(NRFX)/mdk -T $(NRFX)/mdk/nrf52820_xxaa.ld
LDFLAGS += -Wl,--print-memory-usage

SRC := \
	$(ROOT)/main.c \
	$(ROOT)/usb_descriptors.c \
	$(ROOT)/nrf24_radio.c \
	$(REPO)/wireless/switch_pro.c \
	$(TUSB)/src/tusb.c \
	$(TUSB)/src/common/tusb_fifo.c \
	$(TUSB)/src/device/usbd.c \
	$(TUSB)/src/device/usbd_control.c \
	$(TUSB)/src/class/hid/hid_device.c \
	$(TUSB)/src/portable/nordic/nrf5x/dcd_nrf5x.c \
	$(NRFX)/mdk/system_nrf52820.c \
	$(NRFX)/drivers/src/nrfx_power.c \
	$(NRFX)/soc/nrfx_atomic.c \
	$(NRFX)/helpers/nrfx_flag32_allocator.c

ASM := $(NRFX)/mdk/gcc_startup_nrf52820.S

BUILD := $(ROOT)/build
TARGET := $(BUILD)/gp2040wf_receiver

.PHONY: all clean
all: $(TARGET).hex

$(BUILD):
	mkdir -p $(BUILD)

$(TARGET).elf: $(SRC) $(ASM) | $(BUILD)
	$(CC) $(CFLAGS) -o $@ $(SRC) $(ASM) $(LDFLAGS)
	$(SIZE) $@

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex $< $@
	$(OBJCOPY) -O binary $< $(TARGET).bin
	cp -f $@ $(REPO)/firmware-bin/GP2040-WF_receiver.hex

clean:
	rm -rf $(BUILD)
