# GP2040-WF

**sunga2022** 的三模格斗板：**有线、蓝牙、2.4G 接收器**。不要家用 Wi‑Fi。

芯片已经锁定，细节以 [docs/CHIP_LOCK.md](docs/CHIP_LOCK.md) 为准。**先按锁定 BOM 买板，再写程序。**

| 角色 | 芯片 | 板 |
|---|---|---|
| 手柄主控 | 新唐 **M487JIDAE** | NuMaker-PFM-M487，插 **CON1** |
| 手柄无线 | Nordic **nRF52840-QIAA** | Seeed XIAO nRF52840 |
| **2.4G 接收器** | Nordic **nRF52840-QIAA** | 再一块 XIAO nRF52840（只插电脑 USB） |

| 模式 | 谁干活 | 电脑看到的轮询 |
|---|---|---|
| 有线 | M487 USB High Speed | **8 kHz** |
| 蓝牙 | 手柄 nRF52840 BLE HID，名字 `GP2040-WF` | 约 133 Hz |
| 2.4G | 手柄 nRF 发私有 2.4G → 接收器 nRF USB HID | 约 1 kHz |

M487 没有片内射频。CON1 插上后 PA.11 拉高，蓝牙和 2.4G 都停。2.4G 是接收器，不连路由器。

仓库里的 ESP32-C3/S3、沁恒 CH585 是选型过程，**新板不要用。**

## 授权

新固件 MIT。`firmware/nuvoton_m487/bsp` 是新唐 M480BSP 子集，Apache-2.0。根目录 GP2040-CE / Pico 部分版权仍归 OpenStickCommunity、Jason Skuby 与 sunga2022。
