# nRF52840：只做手柄无线

锁定见 [docs/CHIP_LOCK.md](../../docs/CHIP_LOCK.md)。程序先停。

手柄：**nRF52840** BLE HID + ESB/nRF24 发射（喂 4 kHz 够）。

2.4G 接收器 **不是** nRF52820。改成便宜的 **CH32V305 USB HS（4 kHz）+ Si24R1**。评估板：**CH32V307V-EVT**。

`GP2040WF_Dongle` 草稿作废，对不上新 BOM。
