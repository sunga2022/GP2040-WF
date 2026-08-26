# 固件目录

**先看 [docs/CHIP_LOCK.md](../docs/CHIP_LOCK.md)。** 芯片未按该文档落地之前，不要在本目录加新方案。

| 路径 | 锁定？ |
|---|---|
| [nuvoton_m487](nuvoton_m487) | 是。手柄主控，有线 8 kHz。UART 对端是 nRF52840。 |
| [../wireless/nrf52840](../wireless/nrf52840) | 是。手柄 BLE+2.4G。接收器量产是 **nRF52820 U 盘**，协议同一套。 |
| [esp32c3_ble](esp32c3_ble) | 否。已废弃。 |
| [nuvoton_m032bt](nuvoton_m032bt) | 否。不采用。 |
| [ch585](ch585) | 否。沁恒，认错厂商。 |
