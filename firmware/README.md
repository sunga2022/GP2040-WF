# 手柄主板 + 2.4G 接收器固件

手柄主板 **不要** 在这个目录里自己写游戏垫协议。主板就是仓库根目录的 **GP2040-CE**（RP2040）。

这个目录只放两块 **外挂**：

| 目录 | 芯片 | 干什么 |
|------|------|--------|
| [`jieli_ac632n/`](jieli_ac632n/) | 杰里 **AC632N** | Switch Pro 经典蓝牙（无加密） |
| [`receiver_nrf52820/`](receiver_nrf52820/) | **nRF52820** QFN40 5×5 | 2.4G USB-A 接收器，USB+射频一只芯片 |

`receiver_ch32/`、`receiver_pico/` 是旧接收器，不要当产品。`nuvoton_*` / `ch585` / `esp32*` / `teensy41` 也是旧实验。
