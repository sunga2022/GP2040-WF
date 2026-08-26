# 三模与 8 kHz（已换 MCU）

主控已离开 RP2040。细节以 [firmware/README.md](../firmware/README.md) 为准。

| 问题 | 答案 |
|---|---|
| 蓝牙 + 2.4G + 有线？ | 用 **ESP32-S3-DevKitC-1 × 2**。手柄 USB/BLE/ESP-NOW，接收器 USB+ESP-NOW。 |
| 1k 改 8k？ | **S3 改不了。** USB Full Speed 上限 1 kHz。8k 换 **Teensy 4.1** 有线固件。 |
| 还用 GP2040-CE？ | 旧 Pico 板可以。新板不要再画 RP2040。 |
| 主机认证（PS5 USB 狗）？ | 那是 GP2040 PIO-USB 的能力，ESP32-S3 第一版不做。先打 PC。 |
