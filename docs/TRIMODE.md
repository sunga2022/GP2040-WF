# 三模与 8 kHz

芯片锁定见 [CHIP_LOCK.md](CHIP_LOCK.md)。不要家用 Wi‑Fi。

| 问题 | 答案 |
|---|---|
| 主控是哪家？ | **新唐 Nuvoton M487**，不是树莓派，也不是沁恒。 |
| 主板蓝牙 / 2.4G？ | **nRF52840**（Raytac MDBT50Q 一类认证模组），BLE HID 兼容手机和电脑。 |
| **2.4G 接收器芯片？** | 小小 MCU：**nRF52820-QDAA**（5×5 mm），做成 **USB-A U 盘**。不要 SuperMini / XIAO / PA 模块。 |
| 是家里那种 Wi‑Fi 吗？ | 不是。空中是 Nordic ESB，52840 和 52820 同家族对频。 |
| 蓝牙能 8 kHz 吗？ | 不能。BLE 大约一百多 Hz。 |
| 2.4G U 盘能 8 kHz 吗？ | 不能。USB Full Speed，大约 1 kHz。 |
| 8 kHz 走哪？ | 只有 M487 **CON1 USB High Speed**。 |
| 还用 Pico / GP2040-CE？ | 旧板可以。新板不要再画 RP2040。 |
