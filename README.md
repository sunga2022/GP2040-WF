# GP2040-WF

**sunga2022** 的三模格斗板：**有线、蓝牙、2.4G U 盘接收器**。不要家用 Wi‑Fi。

芯片锁定见 [docs/CHIP_LOCK.md](docs/CHIP_LOCK.md)。**先买板，再写程序。**

| 角色 | 芯片 | 形态 |
|---|---|---|
| 手柄主控 | 新唐 **M487JIDAE** | NuMaker-PFM-M487，**CON1** |
| 手柄无线 | Nordic **nRF52840**（Raytac MDBT50Q） | 手柄内小模组；评估用 XIAO |
| **2.4G 接收器** | **CH32V305GBU6** + **Si24R1** | USB-A U 盘；评估用 **CH32V307V-EVT** |

| 模式 | 谁干活 | 轮询 |
|---|---|---|
| 有线 | M487 USB High Speed | **8 kHz** |
| 蓝牙 | nRF52840 BLE HID `GP2040-WF` | 约 133 Hz |
| 2.4G | nRF52840 → Si24R1 → CH32V305 **USB HS** HID | **4 kHz** |

nRF52 的 USB 是 Full Speed，做不到 4 kHz，接收器不用 nRF52820。CH32V305 有官方/淘宝评估板，比 Nordic 便宜。

## 授权

新固件 MIT。`firmware/nuvoton_m487/bsp` 是新唐 M480BSP 子集，Apache-2.0。根目录 GP2040-CE / Pico 部分版权仍归 OpenStickCommunity、Jason Skuby 与 sunga2022。
