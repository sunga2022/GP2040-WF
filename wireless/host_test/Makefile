CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -I..

.PHONY: all clean
all: test_uart_rx test_hid_pack test_nrf24_stub test_jieli test_switch_pro
	./test_uart_rx
	./test_hid_pack
	./test_nrf24_stub
	./test_switch_pro
	./test_jieli
	python3 test_protocol.py

test_uart_rx: test_uart_rx.c ../uart_rx.h ../protocol.h
	$(CC) $(CFLAGS) -o $@ test_uart_rx.c

test_hid_pack: test_hid_pack.c ../hid_pack.h ../protocol.h
	$(CC) $(CFLAGS) -o $@ test_hid_pack.c

test_nrf24_stub: test_nrf24_stub.c ../nrf24.c ../nrf24.h ../protocol.h
	$(CC) $(CFLAGS) -o $@ test_nrf24_stub.c ../nrf24.c

test_switch_pro: test_switch_pro.c ../switch_pro.c ../switch_pro.h ../protocol.h
	$(CC) $(CFLAGS) -o $@ test_switch_pro.c ../switch_pro.c

test_jieli: test_jieli.c ../../firmware/jieli_ac632n/gp2040wf_uart_hid.c ../../firmware/jieli_ac632n/gp2040wf_uart_hid.h ../switch_pro.c ../uart_rx.h ../hid_pack.h ../protocol.h
	$(CC) $(CFLAGS) -I../../firmware/jieli_ac632n -o $@ test_jieli.c ../../firmware/jieli_ac632n/gp2040wf_uart_hid.c ../switch_pro.c

clean:
	rm -f test_uart_rx test_hid_pack test_nrf24_stub test_jieli test_switch_pro
