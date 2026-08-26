# 三模与 8 kHz（已换 MCU）

主控已离开 RP2040。细节以 [firmware/README.md](../firmware/README.md) 为准。

| 问题 | 答案 |
|---|---|
| 蓝牙 + 2.4G + 有线？ | 用 **CH585M-EVT × 2**。手柄 USB HS / BLE / 2.4G，接收器 USB HS + 2.4G。 |
| 1k 改 8k？ | 有线和 2.4G 在 CH585 上可以 8 kHz（USB HS `bInterval=1`，官方 2.4G 最高 8k）。蓝牙做不到 8k。 |
| 还用 ESP32-S3？ | 可以，但只有 1 kHz。新板用 CH585。 |
| 还用 GP2040-CE？ | 旧 Pico 板可以。新板不要再画 RP2040。 |
| 主机认证（PS5 USB 狗）？ | 第一版只打 PC。CH585 USB host 以后再说。 |
