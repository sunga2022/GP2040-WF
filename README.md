# GP2040-WF

**sunga2022** 的无线格斗板固件（Wireless Firmware）。

从 [sunga2022/GP2040-CE](https://github.com/sunga2022/GP2040-CE) 分出。有线仍是 GP2040 USB 1 kHz；蓝牙 / 2.4G 走外挂 nRF52840。

基于 [OpenStickCommunity/GP2040-CE](https://github.com/OpenStickCommunity/GP2040-CE)（MIT）。

## 结论先看

- **三模：可以做，但不是改 RP2040 就能加射频。** 本仓库已接上 UART 插件 + nRF 手柄/接收器固件。
- **8 kHz：RP2040 做不到。** USB Full Speed 最快 1000 Hz。要 8k 必须换 USB High Speed 芯片，那是另一套固件。
- 板卡和接线见 [docs/TRIMODE.md](docs/TRIMODE.md)。

## 现在能做什么

- PC / PS3 / PS4 / PS5 / Switch / Xbox One 等有线模式
- 自定义板：G2、G4F、G5F、RUYI、doio、Pico16、hnu、d26 等
- PS5 / Xbox USB 认证（板载 PIO-USB 从口）
- 内置 Web 配置器
- G2：GP24 以 1000 Hz 把按键帧发给 nRF52840（1 Mbps UART）

## 编译 RP2040

需要 [Pico SDK 2.1.1](https://github.com/raspberrypi/pico-sdk)、CMake、arm-none-eabi-gcc。

```bash
git clone --recurse-submodules https://github.com/sunga2022/GP2040-WF.git
cd GP2040-WF
git submodule update --init --recursive

export PICO_SDK_PATH=/path/to/pico-sdk
export GP2040_BOARDCONFIG=G2
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

UF2 在 `build/`，文件名形如 `GP2040-WF_<version>_G2.uf2`。

`GP2040_BOARDCONFIG` 可选：`14PNEW` `17P` `17PNT` `G2` `G2G` `G4F` `G5F` `QF` `QFG` `RUYI` `Pico16` `Pico16N` `Pico19` `HaM` `hnu` `d26` `doio`。

## 授权

MIT。版权归 OpenStickCommunity、Jason Skuby 与 sunga2022。
