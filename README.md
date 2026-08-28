# GP2040-WF

GP2040-CE 跑在 RP2040 上，板型只保留 **Pico19**。有线协议原样；外面加杰里蓝牙和 **XN297L 3×3** 2.4G，都只报 Switch Pro。插着手柄 USB 时无线静音。

| 模式 | 谁干活 |
|------|--------|
| 有线 Xbox 360 | 手柄 USB，默认已认证 |
| 有线 Xbox One / PS5 | 手柄 USB + 认证口 NXP7105（D+ GP3 D− GP2） |
| 有线 PS4 | 手柄 USB，三份授权文件已编进固件 |
| 有线 Switch | 手柄 USB |
| 蓝牙 Switch | UART GP24 → 杰里 AC632N |
| 2.4G Switch | 手柄 XN297L → USB-A 接收器 **CH32X035 + Si24R1** |

开机（A=B1）：**A Switch Pro · B Xbox 360 · X PS3 · Y PS4 · R1 Xbox One · L1 PS5 · L2 P5 General · R2 键盘**

OLED GP0/GP1，右上角 `87%B`（电量 + L/B/G）。RGB GP28，灯只跟线不跟键值。FN 已取消。

## 烧录

就三个文件，在 [`firmware-bin/`](firmware-bin/)：

1. `GP2040-WF.uf2` → 手柄 RP2040
2. `GP2040-WF_AC632N.ufw` → 杰理 AC632N
3. `GP2040-WF_receiver.bin` → 2.4G USB-A 接收器 **CH32X035F8U6**（WCHISP，起始地址 `0x00000000`）

接收器是 **两颗芯片**：USB MCU **CH32X035F8U6 QFN20 3×3**（全速 USB，**1 kHz** 轮询）+ 射频 **Si24R1 QFN20 4×4**（立创 C14436）。板子 USB-A **16.30×11.20 mm**。不要用 nRF52820，也不要用 RP2040 做接收器。

## 嘉立创

标准版 JSON + BOM：[`pcb/lceda/`](pcb/lceda/)

## 编译

```bash
export PICO_SDK_PATH=/path/to/pico-sdk
GP2040_BOARDCONFIG=Pico19 cmake -B build && cmake --build build
make -C firmware/receiver_ch32
```

脚位：[`configs/Pico19/BoardConfig.h`](configs/Pico19/BoardConfig.h)
