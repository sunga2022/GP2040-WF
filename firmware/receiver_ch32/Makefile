all: build

TARGET:=gp2040wf_receiver
TARGET_MCU:=CH32X035
TARGET_MCU_PACKAGE:=CH32X035F8U6
CH32FUN:=$(CURDIR)/third_party/ch32fun
MINICHLINK:=$(CURDIR)/.minichlink-unused
ADDITIONAL_C_FILES:=third_party/extralibs/fsusb.c ../../wireless/nrf24.c ../../wireless/switch_pro.c
EXTRA_CFLAGS:=-I../../wireless -Ithird_party/extralibs

include $(CH32FUN)/ch32fun.mk

flash: cv_flash
clean: cv_clean

.PHONY: ship
ship: all
	cp $(TARGET).hex ../../firmware-bin/GP2040-WF_receiver.hex
