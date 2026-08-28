# 2.4G USB-A 接收器（键盘接收器大小）

2.4G 用 **XN297L QFN20 3×3 mm**（和 nRF24 / Si24R1 同 SPI 协议）。  
USB 用 **CH32X035F8U6 QFN20 3×3 mm**（自带全速 USB，WCHISP 能烧）。

两颗都是 3×3，板子 USB-A **22×12 mm**。不要用 RP2040（7×7 + Flash）。A7105 是 4×4 且没有 USB。

| XN297L | CH32X035F8U6 |
|--------|----------------|
| CE | PA0（脚 2） |
| CSN | PA1（脚 3） |
| SCK | PA2（脚 4） |
| MOSI | PA3（脚 5） |
| MISO | PA4（脚 6） |
| 16 MHz 晶振 | 芯片用内部 48 MHz HSI，不用晶振 |
| USB D− / D+ | PC16 / PC17（脚 17 / 18） |

烧录：`firmware-bin/GP2040-WF_receiver.hex`  
WCHISPStudio 选 **CH32X035F8U6**，USB。进 ISP：上电时短接板上 **ISP** 两焊盘（D+ 经 4.7k 到 3V3）。

```bash
make -C firmware/receiver_ch32
```
