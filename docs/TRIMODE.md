# 三模与 8 kHz

芯片锁定见 [CHIP_LOCK.md](CHIP_LOCK.md)。

| 问题 | 答案 |
|---|---|
| 主控？ | **新唐 M487**，CON1 **8 kHz** |
| 主板蓝牙 / 2.4G？ | **nRF52840**（兼容手机 BLE HID，并能按 nRF24 发给接收器） |
| **2.4G 接收器？** | 便宜 USB HS MCU：**CH32V305GBU6** + 贴片 **Si24R1**。开发板：**CH32V307V-EVT** |
| 为什么不用 nRF52820？ | 贵，而且 USB FS 只有 1 kHz。要 **4 kHz** 必须 USB High Speed |
| 是家用 Wi‑Fi 吗？ | 不是 |
| 蓝牙 8 kHz？ | 不能 |
| 2.4G 几 k？ | **4 kHz**（USB HS `bInterval = 2`） |
| Pico？ | 旧板可以，新板不要 RP2040 |
