# GP2040-WF

GP2040-CE **原样**跑在 RP2040 上：按键、SOCD、热键、网页配置、有线 XInput / Switch / PS 全部不动。外面只加两条无线。

| 模式 | 谁干活 |
|------|--------|
| **有线 Xbox / PS** | 手柄 USB。认证走 GP2040-CE 原来的 **USB 引导认证**（PS4/PS5/Xbox 插官方/兼容 dongle） |
| **有线 Switch** | 手柄 USB。CE 的 Switch / Switch Pro 描述符 |
| **蓝牙 Switch** | UART → 杰里 **AC632N**，按 CE 的 Switch Pro 协议走 **经典蓝牙 HID**（无加密芯片） |
| **2.4G Switch** | Si24R1 → 第二块 Pico 接收器，USB 枚举成 **Switch Pro**（VID `057E` PID `2009`） |

插着手柄 USB 时无线静音。Xbox / PS **不要**走蓝牙或 2.4G，插手柄 USB。

```
按键 ──► RP2040 GP2040-CE ── USB ──► 电脑 / 主机（含 Xbox/PS 引导认证）
                 ├── UART ──► AC632N ── Switch Pro EDR ──► Switch
                 └── Si24R1 ──► Pico 接收器 USB Switch Pro ──► Switch / Steam
```

开机按键选模式跟 CE 一样：L2 = Switch Pro 时，无线帧带 `WF_MODE_SWITCH`，杰里才发 0x30 报告。

## 烧录（现成 UF2）

现成文件在 [`firmware-bin/`](firmware-bin/)：

1. 按住板上 **BOOT**，插 USB，出现 `RPI-RP2` 磁盘
2. 手柄拖 `firmware-bin/GP2040-WF_Pico16.uf2`
3. 2.4G 接收器拖 `firmware-bin/GP2040-WF_receiver.uf2`

详细步骤、开机组合键见 [`firmware-bin/README.md`](firmware-bin/README.md) 和 [`pcb/FLASH.md`](pcb/FLASH.md)。

**杰里 AC632N 必须另烧固件**（UF2 进不去那颗芯片）。Windows 上用官方 SDK 编，步骤见 [`firmware/jieli_ac632n/README.md`](firmware/jieli_ac632n/README.md)。只用有线 / 2.4G 可以不焊它。

开机：**R1 Xbox · L1 PS5 · B4 PS4 · L2 Switch Pro**。Xbox/PS 走手柄 USB + 认证口；无线只在 Switch Pro 模式发。

## PCB / 嘉立创

| 板 | 尺寸 | 图 | Gerber |
|----|------|----|--------|
| 手柄主板 | 120×82 mm | [`pcb/stick/GP2040-WF-stick.png`](pcb/stick/GP2040-WF-stick.png) | [`pcb/stick/GP2040-WF-stick-gerber.zip`](pcb/stick/GP2040-WF-stick-gerber.zip) |
| 2.4G 接收器 | 42×18 mm | [`pcb/receiver/GP2040-WF-receiver.png`](pcb/receiver/GP2040-WF-receiver.png) | [`pcb/receiver/GP2040-WF-receiver-gerber.zip`](pcb/receiver/GP2040-WF-receiver-gerber.zip) |

原理图：[`pcb/GP2040-WF-schematic.png`](pcb/GP2040-WF-schematic.png) · KiCad 8：`pcb/stick/*.kicad_pcb`、`pcb/receiver/*.kicad_pcb`。

投产前在 KiCad 里铺 GND 铜皮、补齐 USB-C 封装、跑 DRC。QFN-56 焊盘来自 RP2040 数据手册。

## 编译

```bash
git submodule update --init --recursive
export PICO_SDK_PATH=/path/to/pico-sdk
GP2040_BOARDCONFIG=Pico16 cmake -B build && cmake --build build
cmake -S firmware/receiver_pico -B firmware/receiver_pico/build && cmake --build firmware/receiver_pico/build
make -C wireless/host_test
```

没有 picotool 时可用 `tools/elf2uf2.py build/*.elf firmware-bin/out.uf2`。

脚位见 [`docs/CHIP_LOCK.md`](docs/CHIP_LOCK.md)。

## 授权

GP2040-CE 版权归 OpenStickCommunity、Jason Skuby 与 sunga2022。三模附加代码 MIT。
