# nRF52：手柄无线 + U 盘接收器

锁定方案见 [docs/CHIP_LOCK.md](../../docs/CHIP_LOCK.md)。**芯片已定，程序先停。**

| 角色 | 芯片 | 形态 | 草稿 |
|---|---|---|---|
| 手柄无线 | **nRF52840-QIAA**（兼容性：BLE HID + 同家族 2.4G） | 量产 Raytac MDBT50Q；评估 XIAO | `GP2040WF_Controller` |
| **2.4G 接收器** | **nRF52820-QDAA** 5×5 mm | **USB-A U 盘**；评估 PCA10059 | `GP2040WF_Dongle`（现草稿按 52840 dongle，量产改 52820） |

不要用 XIAO / SuperMini 当产品接收器。不要 nRF24 PA 大模块。
