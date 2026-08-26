# 沁恒 CH585 手柄 / 接收器

主控就是 **一颗** 新塘 CH585，不用树莓派 RP2040。

- USB 2.0 **High Speed** → 有线 **8 kHz**
- 片内 **BLE 5.4**
- 片内私有 **2.4G**（官方最高 8 kHz）。这就是无线/常说的 wifi 接收器通道，不是 802.11 Wi-Fi。

沁恒没有「8k USB + 蓝牙 + 家用 Wi-Fi」的单芯片。要 8k 又要蓝牙无线，用 CH585。

## 买什么板

两块 **CH585M-EVT**（沁恒官方评估板）：

1. 手柄：烧 `build/controller.hex`，USB HS 口插电脑 = 有线 8k
2. 接收器：烧 `build/dongle.hex`，插电脑收 2.4G（要在 MounRiver 里打开射频库）

USB HS 必须外接 32 MHz 晶振。评估板自带。ISP 下载走板上的 USB FS 口（BOOT 键）。

按键脚见 `include/wf_pins.h`。方向和 14 个键在 PB0–PB15，Home/Capture 在 PA8/PA9，LED 在 PA0。

## 编译（有线 8 kHz HID）

Linux 用 xpack RISC-V GCC。没有 `riscv-none-elf-gcc` 时，把工具链放到 `firmware/ch585/.toolchain/`。

```bash
cd firmware/ch585
make
```

得到 `build/controller.hex` 和 `build/dongle.hex`。用 [WCHISPTool](https://www.wch.cn/downloads/WCHISPTool_Setup_exe.html) 或 `wchisp` 从 USB FS 下载。

`make` 默认 **关射频**（`WF_ENABLE_RF=0`）。手柄插 USB 就是 8 kHz 有线 HID。接收器同样会枚举成手柄，但没有 2.4G 数据。

## 三模（蓝牙 / 2.4G / 有线）

有线 8k 这份就能打。蓝牙和 2.4G 依赖沁恒闭源 BLE/RF ROM（`EVT/EXAM/BLE/LIB`），要用 [MounRiver Studio](http://www.mounriver.com/)：

1. 打开官方 `RF_Basic` 例程，把 `src/hid_gamepad.c`、`src/usb_desc.c`、`src/ch585_usbhs_device.c` 加进去
2. `WF_ENABLE_RF=1` 编译 `rf_link.c`
3. 蓝牙用官方 `HID_Mouse` / `HID_Keyboard` 例程，把报告描述符换成 `GamepadRepDesc`

更细的步骤见 `mrs/README.md`。

不要把手柄 USB 和接收器 USB 同时插同一台电脑。

## 和 CH32V307 的关系

CH32V307 也能做有线 8k，但没有 BLE/2.4G。要三模就用 CH585，不要 307。
