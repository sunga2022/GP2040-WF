# 新唐 M032BT：蓝牙 / 2.4G（不是 8 kHz）

M487 负责有线 8 kHz。无线要另一颗 **M032BT**（BLE 5.0 + 新唐私有 2.4G）。它的 USB 是 **Full Speed**，有线最高 1 kHz。

不要指望 M032BT 单独做成 8 kHz 有线手柄。

## 怎么接

推荐：

1. 手柄主控仍是 **M487**（USB HS CON1）
2. M032BT 通过 UART 收 `wireless/protocol.h` 里的 `WfFrame`（20 字节，1 Mbps）
3. M032BT 发 BLE HID 或私有 2.4G；电脑侧再做一颗 M032BT 接收器，USB FS 1 kHz 枚举成手柄

NuMaker-M032BT 例程和射频库在新唐 BLE BSP 里，体积大而且和 M480 不是同一套 SDK，所以这里不把 BLE stack 塞进仓库。有线 8 kHz 先在 M487 上打。

帧格式已经固定，见仓库根目录 `wireless/protocol.h`。
