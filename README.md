# GP2040-WF

GP2040-CE **原样**跑在 RP2040 上：按键、SOCD、热键、网页配置、有线 XInput / Switch / PS 全部不动。外面只加两条无线。

| 模式 | 谁干活 |
|------|--------|
| **有线** | 手柄 USB。就是原来的 GP2040-CE |
| **蓝牙** | UART 把按键帧送给杰里 **AC632N**（BLE + 经典 EDR HID） |
| **2.4G** | 同一帧用 Si24R1 / nRF24 发出去；**另一块 Pico** 当接收器，插电脑 USB |

插着手柄 USB 时无线静音，避免和有线抢同一台电脑。

```
按键 ──► RP2040 GP2040-CE ── USB ──► 电脑 / 主机
                 ├── UART 1 Mbps ──► AC632N ── BLE / EDR ──► 手机 / 电脑
                 └── Si24R1/nRF24 ──► 空中 ──► Pico 接收器 USB ──► 电脑
```

Xbox / PS / Switch **主机蓝牙伪装不在范围内**。主机有线模式仍走 GP2040-CE USB。

## 买什么、接哪根线

完整脚位见 [`docs/CHIP_LOCK.md`](docs/CHIP_LOCK.md)。

- 手柄主板：现有 GP2040 板。**Pico16** 开了 UART + 2.4G；**G2** 只开了 UART（蓝牙 + 有线）
- 蓝牙模组：杰里 **AC632N**
- 2.4G：两颗 Si24R1 或 nRF24L01+；接收器再一块 **树莓派 Pico**

## 编译

```bash
# 手柄（Pico16 = 完整三模）
export PICO_SDK_PATH=/path/to/pico-sdk
GP2040_BOARDCONFIG=Pico16 cmake -B build && cmake --build build

# 2.4G 接收器
cmake -S firmware/receiver_pico -B firmware/receiver_pico/build
cmake --build firmware/receiver_pico/build

# 协议 / 胶水（不需要 Pico SDK）
make -C wireless/host_test
```

杰里侧：官方 HID 工程 + [`firmware/jieli_ac632n/`](firmware/jieli_ac632n/)。

## 授权

GP2040-CE 版权归 OpenStickCommunity、Jason Skuby 与 sunga2022。三模附加代码 MIT。
