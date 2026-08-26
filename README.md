# GP2040-WF

**sunga2022** 的无线格斗板固件（Wireless Firmware）。

从 [sunga2022/GP2040-CE](https://github.com/sunga2022/GP2040-CE) 分出，目标是在现有有线 GP2040 能力之上做 **蓝牙 / 2.4G / 有线三模**。当前主路径仍是 RP2040 有线 1000 Hz，和上游 GP2040-CE 一样。

基于 [OpenStickCommunity/GP2040-CE](https://github.com/OpenStickCommunity/GP2040-CE)（MIT）。

## 现在能做什么

- PC / PS3 / PS4 / PS5 / Switch / Xbox One 等有线模式
- 自定义板：G2、G4F、G5F、RUYI、doio、Pico16、hnu、d26 等
- PS5 / Xbox USB 认证（板载 PIO-USB 从口）
- 内置 Web 配置器

## 还做不到的

RP2040 没有射频，USB 也只有 Full Speed：

- 不能靠改 `bInterval` 变成 8 kHz
- 不能在这颗芯片里直接加蓝牙或 2.4G

三模计划是 **双 MCU**：RP2040 继续跑本固件做有线和主机认证，nRF52840 负责 BLE / 2.4G。那部分还没合进主循环。

## 编译

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

生成的 UF2 在 `build/`，文件名形如 `GP2040-WF_<version>_G2.uf2`。

`GP2040_BOARDCONFIG` 可选：`14PNEW` `17P` `17PNT` `G2` `G2G` `G4F` `G5F` `QF` `QFG` `RUYI` `Pico16` `Pico16N` `Pico19` `HaM` `hnu` `d26` `doio`。

## 授权

MIT。版权归 OpenStickCommunity、Jason Skuby 与 sunga2022。
