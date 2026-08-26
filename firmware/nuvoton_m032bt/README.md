# 新唐 M032BT（备选，不是现在这套模块）

现在的蓝牙方案是 **M487 UART + ESP32-C3 模块**，见 [../esp32c3_ble](../esp32c3_ble)。

M032BT 是新唐自己的 BLE+2.4G 芯片，USB 只有 Full Speed。若以后改用 NuMaker-M032BT 当模块，同样收 `wireless/protocol.h` 的 `WfFrame`（1 Mbps）。不要用 M032BT 做有线 8 kHz。
