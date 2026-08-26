# 2.4G 接收器（第二块 Pico）

主板 GP2040-CE 用 Si24R1 / nRF24 发 20 字节 `WfFrame`。这块板只收射频、枚举成 USB 手柄（VID `1d50` PID `57f6`）。

接线（和 Pico16 主板发射脚对齐，接收器多一根 MISO）：

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
# 刷 gp2040wf_receiver.uf2
```

插电脑 USB。主板拔掉 USB 后才发 2.4G（插着有线时无线静音）。空中 50 ms 没包就松开按键。
