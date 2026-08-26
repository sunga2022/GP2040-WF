# GP2040-WF

**sunga2022** 的三模格斗板固件。主控 **不再使用 RP2040**。

- **三模（蓝牙 / 2.4G / 有线）主控：ESP32-S3-DevKitC-1**
- **有线 8 kHz：Teensy 4.1**（USB High Speed，没有射频）
- 仓库根目录里的 GP2040-CE / Pico 代码只给旧 RP2040 板参考

新固件说明：**[firmware/README.md](firmware/README.md)**

## 为什么换掉 RP2040

RP2040 没有射频，USB 只有 Full Speed（最高 1 kHz）。继续改 GP2040-CE 加不出蓝牙/2.4G，也加不出 8 kHz。

ESP32-S3 自带 USB、BLE、Wi-Fi/ESP-NOW，GPIO 够排一整套摇杆按键。2.4G 用第二块 S3 做接收器。

8 kHz 必须 USB High Speed。S3 没有 HS PHY，所以 8k 走 Teensy 4.1，和三模不是同一块板。

## 最快上手（三模 1k）

买两块 **ESP32-S3-DevKitC-1**。按键脚见 `firmware/esp32s3/include/wf_pins.h`。

```bash
pip install platformio
cd firmware/esp32s3/controller && pio run -t upload
cd ../dongle && pio run -t upload
```

## 授权

新固件 MIT。根目录 GP2040-CE 部分版权仍归 OpenStickCommunity、Jason Skuby 与 sunga2022。
