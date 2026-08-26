# 手柄主板 + 2.4G 接收器固件

手柄主板 **不要** 在这个目录里自己写游戏垫协议。主板就是仓库根目录的 **GP2040-CE**（RP2040）。

这个目录只放两块 **外挂**：

| 目录 | 芯片 | 干什么 |
|------|------|--------|
| [`jieli_ac632n/`](jieli_ac632n/) | 杰里 **AC632N** | 手柄上的蓝牙模组：UART 收 `WfFrame`，转 BLE / EDR HID |
| [`receiver_pico/`](receiver_pico/) | **RP2040 Pico** + Si24R1 | **2.4G 接收器**：空中收包 → USB HID |

`nuvoton_*` / `ch585` / `esp32*` / `teensy41` 是旧实验，不要当产品路径。
