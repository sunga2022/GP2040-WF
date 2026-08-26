# 已废弃：ESP32-C3 / S3

**不要用这套做新板。** 锁定方案是 M487 + 手柄 nRF52840 + **CH32V305/Si24R1 U 盘（4 kHz）**，见 [docs/CHIP_LOCK.md](../../docs/CHIP_LOCK.md)。

下面只是选型过程留下的 ESP-NOW 路径（Wi‑Fi PHY），和「只要 2.4G 接收器、不要家用 Wi‑Fi」不一致。

---

# 旧说明：无线模块：蓝牙 + 2.4G

| 板 | 芯片 | 固件 |
|---|---|---|
| 手柄模组 | **ESP32-C3 SuperMini** | `stick`：BLE + ESP-NOW 发射 |
| 2.4G 接收器 | **ESP32-S3 SuperMini / DevKit** | `dongle`：ESP-NOW 接收 + USB HID |

C3 的 USB 做不了 HID，所以接收器要用 S3。

## 手柄接线（C3 stick）

| NuMaker-PFM-M487 | C3 | 作用 |
|---|---|---|
| D1 PB.3 UART1_TXD | GPIO4 RX | 1 Mbps 按键帧 |
| D0 PB.2 UART1_RXD | GPIO5 TX | 可空 |
| NU7.7 PA.11 | GPIO2 | CON1 插上后蓝牙和 2.4G 都停 |
| 3V3 / GND | 3V3 / GND | |

接收器只插电脑 USB。

## 怎么打

| 模式 | 做法 |
|---|---|
| 有线 8 kHz | 手柄 CON1 插电脑 |
| 蓝牙 | 拔 CON1，搜 `GP2040-WF` |
| 2.4G | 拔 CON1，插 S3 接收器。不要和蓝牙同时连同一台电脑 |

BLE ≈ 133 Hz。2.4G 接收器是 USB FS ≈ 1 kHz。8 kHz 只有 M487 CON1。

## 编译

```bash
pip install platformio
cd firmware/esp32c3_ble
pio run -e stick
pio run -e dongle
pio run -e stick -t upload
pio run -e dongle -t upload
```
