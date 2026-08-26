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

## 编译

```bash
export PICO_SDK_PATH=/path/to/pico-sdk
GP2040_BOARDCONFIG=Pico16 cmake -B build && cmake --build build
cmake -S firmware/receiver_pico -B firmware/receiver_pico/build && cmake --build firmware/receiver_pico/build
make -C wireless/host_test
```

脚位见 [`docs/CHIP_LOCK.md`](docs/CHIP_LOCK.md)。

## 授权

GP2040-CE 版权归 OpenStickCommunity、Jason Skuby 与 sunga2022。三模附加代码 MIT。
