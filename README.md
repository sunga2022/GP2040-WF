# GP2040-WF

**sunga2022** 的三模格斗板固件。主控 **不再使用 RP2040**。

- **主控：沁恒 CH585**（USB 2.0 High Speed + BLE 5.4 + 2.4G）
- 有线 **8 kHz**，2.4G 官方最高也是 8 kHz；蓝牙仍受 BLE 连接间隔限制
- 仓库根目录里的 GP2040-CE / Pico 代码只给旧 RP2040 板参考

新固件说明：**[firmware/README.md](firmware/README.md)** · CH585 细节：**[firmware/ch585/README.md](firmware/ch585/README.md)**

## 为什么换掉 RP2040

RP2040 没有射频，USB 只有 Full Speed（最高 1 kHz）。继续改 GP2040-CE 加不出蓝牙/2.4G，也加不出 8 kHz。

CH585 片内 USB HS PHY，有线 `bInterval=1` 就是 125 µs = 8 kHz。同一颗芯片还有 BLE 和私有 2.4G，三模不用再叠一块 ESP32。

## 最快上手（有线 8 kHz）

买两块 **CH585M-EVT**。按键脚见 `firmware/ch585/include/wf_pins.h`。

```bash
cd firmware/ch585
make
# 用 WCHISPTool / wchisp 把 build/controller.hex 下到手柄
```

插评估板的 **USB HS** 口（不要插错成 ISP 用的 FS 口）。设备名 `GP2040-WF`。

2.4G / 蓝牙要在 MounRiver Studio 里链沁恒 RF/BLE ROM，步骤见 `firmware/ch585/mrs/README.md`。

## 授权

新固件 MIT。CH585 SDK 子集仍归南京沁恒，只能用于其 MCU。根目录 GP2040-CE 部分版权仍归 OpenStickCommunity、Jason Skuby 与 sunga2022。
