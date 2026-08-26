# 新唐 M032BT（不采用）

锁定方案见 [docs/CHIP_LOCK.md](../../docs/CHIP_LOCK.md)：无线是 **nRF52840**，不是 M032BT。

M032BT 是新唐自己的 BLE+2.4G 芯片，USB 只有 Full Speed。若以后改用 NuMaker-M032BT 当模块，同样收 `wireless/protocol.h` 的 `WfFrame`（1 Mbps）。不要用 M032BT 做有线 8 kHz。
