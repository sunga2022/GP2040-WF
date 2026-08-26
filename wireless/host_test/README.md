# Host tests (no Pico SDK)

```bash
make -C wireless/host_test
```

Covers `WfFrame` packing, UART reassembly, HID hat mapping, nRF24 stub, and the Jieli byte glue.
