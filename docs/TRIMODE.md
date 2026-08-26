# 三模与 8 kHz

芯片锁定见 [CHIP_LOCK.md](CHIP_LOCK.md)。不要家用 Wi‑Fi。

| 问题 | 答案 |
|---|---|
| 主控是哪家？ | **新唐 Nuvoton M487**，不是树莓派，也不是沁恒。 |
| 蓝牙和 2.4G 用哪颗？ | 手柄上一颗 **nRF52840**。 |
| **2.4G 接收器芯片？** | 再一颗 **nRF52840-QIAA**（USB HID + 私有 2.4G 接收）。 |
| 是家里那种 Wi‑Fi 吗？ | 不是。空中是 Nordic ESB，不连路由器。 |
| 蓝牙能 8 kHz 吗？ | 不能。BLE 大约一百多 Hz。 |
| 2.4G 接收器能 8 kHz 吗？ | 不能。nRF52840 USB 是 Full Speed，大约 1 kHz。 |
| 8 kHz 走哪？ | 只有 M487 **CON1 USB High Speed**。 |
| 还用 Pico / GP2040-CE？ | 旧板可以。新板不要再画 RP2040。 |
