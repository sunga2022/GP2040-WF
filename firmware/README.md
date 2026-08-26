# 固件目录

**先看 [docs/CHIP_LOCK.md](../docs/CHIP_LOCK.md)。** 芯片未按该文档落地之前，不要在本目录加新方案。

| 路径 | 锁定？ |
|---|---|
| [nuvoton_m487](nuvoton_m487) | 是。手柄主控，有线 8 kHz。UART 对端改为 nRF52840，不是 ESP32。 |
| [../wireless/nrf52840](../wireless/nrf52840) | 是。手柄 BLE+2.4G 发射，以及 **2.4G 接收器**。草稿，锁定后才继续写。 |
| [esp32c3_ble](esp32c3_ble) | 否。ESP-NOW / Wi‑Fi PHY，已废弃。 |
| [nuvoton_m032bt](nuvoton_m032bt) | 否。不采用。 |
| [ch585](ch585) | 否。沁恒，认错厂商。 |
