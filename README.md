# GP2040-WF

GP2040-CE 跑在 RP2040 上，板型只保留 **Pico19**。有线协议原样；外面加杰里蓝牙和 **XN297L 3×3** 2.4G，都只报 Switch Pro。插着手柄 USB 时无线静音。

| 模式 | 谁干活 |
|------|--------|
| 有线 Xbox 360 | 手柄 USB，默认已认证 |
| 有线 Xbox One / PS5 | 手柄 USB + 认证口 NXP7105（D+ GP3 D− GP2） |
| 有线 PS4 | 手柄 USB，三份授权文件已编进固件 |
| 有线 Switch | 手柄 USB |
| 蓝牙 Switch | UART GP24 → 杰里 AC632N |
| 2.4G Switch | 手柄 XN297L → USB-A 接收器 **nRF52820**（USB+2.4G 一只芯片） |

开机（A=B1）：**A Switch Pro · B Xbox 360 · X PS3 · Y PS4 · R1 Xbox One · L1 PS5 · L2 P5 General · R2 键盘**

OLED GP0/GP1，右上角 `87%B`（电量 + L/B/G）。RGB GP28，灯只跟线不跟键值。FN 已取消。

## 烧录

就三个文件，在 [`firmware-bin/`](firmware-bin/)：

1. `GP2040-WF.uf2` → 手柄 RP2040
2. `GP2040-WF_AC632N.ufw` → 杰理 AC632N
3. `GP2040-WF_receiver.hex` → 2.4G USB-A 接收器 **nRF52820**（SWD：J-Link / WCH-Link）

接收器是键盘接收器那种 **一只 MCU**，USB-A **22×12 mm**，芯片 **nRF52820-QDAA QFN40 5×5**（立创 C3015612）。不要用两颗 3×3，也不要用 RP2040 做接收器。

## 嘉立创

标准版 JSON + BOM：[`pcb/lceda/`](pcb/lceda/)

## 编译

```bash
export PICO_SDK_PATH=/path/to/pico-sdk
GP2040_BOARDCONFIG=Pico19 cmake -B build && cmake --build build
make -C firmware/receiver_nrf52820
```

脚位：[`configs/Pico19/BoardConfig.h`](configs/Pico19/BoardConfig.h)
