# GP2040-WF

GP2040-CE **原样**跑在 RP2040 上：按键、SOCD、热键、网页配置、有线 XInput / Switch / PS 全部不动。外面只加两条无线。

| 模式 | 谁干活 |
|------|--------|
| **有线 Xbox One / PS5** | 手柄 USB。认证走 GP2040-CE 的 **USB 引导认证**（认证口插官方/兼容 dongle） |
| **有线 PS4** | 手柄 USB。认证密钥已编进固件，不用网页上传 `serial.txt` / `sig.bin` / `key.pem` |
| **有线 Switch** | 手柄 USB。CE 的 Switch / Switch Pro 描述符 |
| **蓝牙 Switch** | UART → 杰里 **AC632N**，按 CE 的 Switch Pro 协议走 **经典蓝牙 HID**（无加密芯片） |
| **2.4G Switch** | Si24R1 → 第二块 Pico 接收器，USB 枚举成 **Switch Pro**（VID `057E` PID `2009`） |

插着手柄 USB 时无线静音。Xbox / PS **不要**走蓝牙或 2.4G，插手柄 USB。

```
按键 ──► RP2040 GP2040-CE ── USB ──► 电脑 / 主机（含 Xbox/PS 引导认证）
                 ├── UART ──► AC632N ── Switch Pro EDR ──► Switch
                 └── Si24R1 ──► Pico 接收器 USB Switch Pro ──► Switch / Steam
```

开机按键和 GP2040-CE 一样（A=B1）：**A Switch Pro · B Xbox 360 · X PS3 · Y PS4 · R1 Xbox One · L1 PS5 · L2 P5 General · R2 键盘**。无线只在 Switch Pro 模式发。PS4 密钥已内置。

## 烧录（现成 UF2）

现成文件在 [`firmware-bin/`](firmware-bin/)：

1. 按住板上 **BOOT**，插 USB，出现 `RPI-RP2` 磁盘
2. 手柄拖 `firmware-bin/GP2040-WF_Pico16.uf2`
3. 2.4G 接收器拖 `firmware-bin/GP2040-WF_receiver.uf2`

详细步骤、开机组合键见 [`firmware-bin/README.md`](firmware-bin/README.md) 和 [`pcb/FLASH.md`](pcb/FLASH.md)。

**杰里 AC632N 必须另烧固件**（UF2 进不去那颗芯片）。现成 `firmware-bin/GP2040-WF_AC632N.ufw`，GitHub Actions 工作流 `Jieli AC632N HID` 也会编。步骤见 [`firmware/jieli_ac632n/README.md`](firmware/jieli_ac632n/README.md)。只用有线 / 2.4G 可以不焊它。

开机：**A Switch Pro · B Xbox 360 · X PS3 · Y PS4 · R1 Xbox One · L1 PS5 · L2 P5 General · R2 键盘**。Xbox One / PS5 走手柄 USB + 认证口（默认打开）；PS4 用内置密钥；无线只在 Switch Pro 模式发。

OLED 右上角只显示链路字母 **L** 插线 / **B** 蓝牙 / **G** 2.4G。不做百分比：RP2040 模拟采样只能用 GP26–29 里的 **一脚**（不是四脚），Pico16 这四脚都占了。

电量改成双色灯：按键 WS2812 串后面再多一颗（还是 GP28 那根线）。正常绿灯，低压红灯。插 USB 一定绿灯。低压检测用数字脚接电压检测芯片（开漏、低电平=亏电），不占 26–29。Pico16 暂时没有空闲数字脚，`BATTERY_LOW_PIN` 先留空；有空脚后在 `BoardConfig.h` 里填上即可。也可以用两脚红绿双色灯：`BATTERY_LED_RED_PIN` / `BATTERY_LED_GREEN_PIN`。

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
