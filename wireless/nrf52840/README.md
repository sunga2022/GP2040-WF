# nRF52840：手柄无线 + 2.4G 接收器

锁定方案见 [docs/CHIP_LOCK.md](../../docs/CHIP_LOCK.md)。**芯片已定，程序先停。**

| 板 | 芯片 | 草稿 |
|---|---|---|
| 手柄模组 | nRF52840-QIAA（XIAO） | `GP2040WF_Controller`：UART → BLE HID + ESB 发射 |
| **2.4G 接收器** | nRF52840-QIAA（再一块 XIAO） | `GP2040WF_Dongle`：ESB 接收 → USB HID |

这是锁定的射频方向。现有 `.ino` 只是草稿，确认 BOM 后再改。
