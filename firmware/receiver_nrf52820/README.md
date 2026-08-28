# 2.4G USB-A 接收器：一颗 MCU

键鼠接收器那种 **一只芯片**：**nRF52820** QFN40 **5×5 mm**。

片上自带全速 USB + 2.4G 射频，空中和 nRF24 / XN297L / Si24R1 兼容。手柄不用改射频。

不要用两颗 3×3（CH32 + XN297L）。CH582 更好买，但射频对不上 nRF24，手柄还得再焊一颗。

| 脚 | 功能 |
|----|------|
| 12 / 13 | USB D− / D+ |
| 10 | VBUS（5V，芯片自己降到 3.3） |
| 24 | ANT |
| 28 / 29 | 32 MHz 晶振 |
| 19 / 20 | SWDIO / SWDCLK |

烧录：`firmware-bin/GP2040-WF_receiver.hex`  
J-Link / WCH-Link（SWD）烧 nRF52820。板子 USB-A **22×12 mm**。

```bash
make -C firmware/receiver_nrf52820
```
