# GP2040-WF

**sunga2022** 的三模格斗板：**有线、蓝牙、2.4G U 盘**。不要家用 Wi‑Fi。

芯片锁定见 [docs/CHIP_LOCK.md](docs/CHIP_LOCK.md)。**手柄无线是杰里 AC632N，不是 nRF52840。**

| 角色 | 芯片 | 形态 |
|---|---|---|
| 手柄主控 | 新唐 **M487JIDAE** | NuMaker-PFM-M487，**CON1** |
| 手柄蓝牙 | 杰里 **AC632N** | 评估：AC632N 开发板。BLE + 经典蓝牙 |
| 手柄 2.4G 发射 / U 盘射频 | **Si24R1** ×2 | 4×4 mm 贴片 |
| **2.4G 接收器** | **CH32V305GBU6** | USB-A U 盘；评估 **CH32V307V-EVT** |

| 模式 | 谁干活 | 轮询 |
|---|---|---|
| 有线 | M487 USB High Speed | **8 kHz** |
| 蓝牙 | AC632N BLE / EDR HID `GP2040-WF` | BLE 约 133 Hz |
| 2.4G | M487→Si24R1 → CH32V305 **USB HS** | **4 kHz** |

Xbox / Switch / PS 主机无线不做在这颗杰里上。电脑和手机蓝牙可以。

## 授权

新固件 MIT。`firmware/nuvoton_m487/bsp` 是新唐 M480BSP 子集，Apache-2.0。根目录 GP2040-CE / Pico 部分版权仍归 OpenStickCommunity、Jason Skuby 与 sunga2022。
