# 在 MounRiver Studio 里打开 CH585 三模

Linux 上的 `make` 只编 **USB HS HID 8 kHz**。蓝牙和 2.4G 要用沁恒官方 ROM。

## 准备

1. 安装 [MounRiver Studio](http://www.mounriver.com/)
2. 克隆 [openwch/ch585](https://github.com/openwch/ch585)
3. 两块 CH585M-EVT

## 有线 8 kHz

把本目录上一级的 `src/`、`include/` 加进任意 USBHS Device 工程（可从 `EVT/EXAM/USB/USBHS/DEVICE/CompositeKM` 改）。编译器若是 WCH 的 `riscv-none-embed-gcc`，去掉 `INT_SOFT`，改用默认的 `WCH-Interrupt-fast`。

下载后把 USB **HS** 口（不是 ISP 用的 FS 口）插电脑。设备名 `GP2040-WF`。

## 2.4G 8 kHz

从 `EVT/EXAM/BLE/RF_Basic` 起步：

- 手柄：`TEST_MODE = MODE_TX`，定时器里不要发测试字节，改成 `Gamepad_Read` + `rf_link_send`
- 接收器：`TEST_MODE = MODE_RX`，在 `RF_STATE_RX` 回调里取出 WfFrame，再 `USBHS_Endp_DataUp`
- 打开 `WF_ENABLE_RF=1`，并加入 BLE/RF 库、`CH585BLE_ROMx.hex`

接入地址、频点两边必须一致。默认频点 16，帧格式见仓库根目录 `wireless/protocol.h`。

## 蓝牙 HID

从 `EVT/EXAM/BLE/HID_Mouse` 起步，把鼠标报告换成 `GamepadRepDesc`（16 键 + hat + 四轴）。BLE 通知间隔受连接参数限制，做不到 8 kHz，通常一百多 Hz。手机/平板搜 `GP2040-WF`。

## 模式优先级

手柄插着 USB HS 时只走有线 8k，不要同时发 2.4G。拔掉 USB 再发射频。接收器始终插在电脑上。
