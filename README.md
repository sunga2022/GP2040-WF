# GP2040-WF

**sunga2022** 的三模格斗板。主控是 **一颗沁恒（新塘）CH585**，不用树莓派 RP2040。

| 模式 | 这颗 CH585 上怎么做 |
|---|---|
| 有线 | USB 2.0 High Speed → **8 kHz** |
| 蓝牙 | 片内 BLE 5.4 |
| 无线（常说的 wifi / 2.4G） | 片内私有 2.4G，官方最高 **8 kHz** |

沁恒没有「USB HS 8k + 蓝牙 + 家用 Wi-Fi（802.11）」的单芯片。带 8k USB 和蓝牙的新塘料就是 CH585，无线走 2.4G，不是连路由器那种 Wi-Fi。

新板说明：[firmware/README.md](firmware/README.md) · [firmware/ch585/README.md](firmware/ch585/README.md)

根目录里的 GP2040-CE / Pico 代码只给旧 RP2040 板参考，新板不要再画 Pico。

## 买什么

两块 **CH585M-EVT**。按键脚见 `firmware/ch585/include/wf_pins.h`。

```bash
cd firmware/ch585
make
# WCHISPTool / wchisp 烧 build/controller.hex
```

打游戏插评估板的 **USB HS** 口。设备名 `GP2040-WF`。蓝牙 / 2.4G 要在 MounRiver 里链沁恒 ROM，见 `firmware/ch585/mrs/README.md`。

## 授权

新固件 MIT。CH585 SDK 子集仍归南京沁恒。根目录 GP2040-CE 部分版权仍归 OpenStickCommunity、Jason Skuby 与 sunga2022。
