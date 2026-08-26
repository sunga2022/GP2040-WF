# GP2040-WF

**sunga2022** 的格斗板固件。主控是 **新唐（Nuvoton）M487**，不是树莓派 RP2040，也不是南京沁恒（WCH）。

「新塘」是 **新唐** 的笔误。沁恒 CH585 那套还留在 `firmware/ch585` 里，新板不要画。

## 现在能打的

| 模式 | 芯片 | 说明 |
|---|---|---|
| **有线 8 kHz** | **M487** USB 2.0 High Speed | 主固件，`firmware/nuvoton_m487` |
| 蓝牙 / 2.4G | M032BT | 片内 BLE + 私有 2.4G；USB 只有 FS，**不是 8 kHz** |
| 家用 Wi-Fi | 不是片内 | NuMaker-IoT-M487 上的模组，不能当 8 kHz HID |

新唐没有「USB HS 8k + 蓝牙 + 802.11」的单芯片。8 kHz 只存在于 M487/M484 的 USB HS。手柄圈说的 wifi/2.4G 走 M032BT 的 2.4G 接收器。

## 买什么

一块 [NuMaker-PFM-M487](https://www.nuvoton.com/board/numaker-pfm-m487/)。

```bash
cd firmware/nuvoton_m487
make test
make
```

Nu-Link ICE 口烧 `build/controller.hex`。打游戏插板上的 **CON1（USB HS）**。设备名 `GP2040-WF`。

按键脚：`firmware/nuvoton_m487/include/wf_pins.h`。

## 授权

新固件 MIT。`firmware/nuvoton_m487/bsp` 是新唐 M480BSP 子集，Apache-2.0。根目录 GP2040-CE / Pico 部分版权仍归 OpenStickCommunity、Jason Skuby 与 sunga2022。
