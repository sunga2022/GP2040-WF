# 2.4G USB-A 接收器：CH32X035 + Si24R1

产品接收器就这两颗：

| 芯片 | 封装 | 立创 | 干什么 |
|------|------|------|--------|
| **CH32X035F8U6** | QFN20 **3×3** | C42442062 | USB FS HID，`bInterval=1` → **1 kHz** |
| **Si24R1** | QFN20 **4×4** P0.5 | C14436 | 2.4G RX，寄存器对齐手柄 XN297L |

板子 USB-A **16.30×11.20 mm**。VBUS 经 SOT-23 LDO 到 3.3 V（Si24R1 要 1.9–3.6 V）。CH32 用内部 48 MHz HSI，晶振只给 Si24R1。

不要用 nRF52820 当产品。不要用 CH582 / CH585 / PT3220 听 XN297L，射频对不上。手柄蓝牙仍是杰里，2.4G 仍是 XN297L，都不用改。

| CH32 脚 | 功能 |
|---------|------|
| PA0 / PA1 / PA2 / PA3 / PA4 | Si24R1 CE / CSN / SCK / MOSI / MISO |
| PC16 / PC17 | USB D− / D+ |
| PC18 / PC19 | SWDIO / SWCLK（调试用，量产走 WCHISP） |

烧录：上电时把 ISP 焊盘短到 3V3（D+ 经 4.7 kΩ），WCHISPStudio 选 `CH32X035F8U6`，烧 `firmware-bin/GP2040-WF_receiver.hex`。

```bash
make -C firmware/receiver_ch32
make -C firmware/receiver_ch32 ship
```
