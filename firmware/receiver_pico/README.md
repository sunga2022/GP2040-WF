# 2.4G 接收器（Switch Pro USB）

主板发 `WfFrame`。这块 Pico 收射频，USB 枚举成和 GP2040-CE 一样的 **Switch Pro**（VID `057E` PID `2009`），处理 USB 0x80 握手和 0x01 子命令。无加密。

插 Switch 底座或 PC（Steam）。Xbox / PS 请插手柄 USB，不要插这块板。

| nRF24 / Si24R1 | Pico |
|----------------|------|
| CE | GP1 |
| CSN | GP21 |
| SCK | GP25 |
| MOSI | GP29 |
| MISO | GP16 |
| 3V3 / GND | 3V3 / GND |

```bash
export PICO_SDK_PATH=/path/to/pico-sdk
cmake -S firmware/receiver_pico -B firmware/receiver_pico/build
cmake --build firmware/receiver_pico/build
```
