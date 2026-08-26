# 沁恒 CH585（旧实验，不是新唐）

这段是把「新塘」理解成南京沁恒时留下的。**正确厂商是新唐 Nuvoton M487**，见 [../nuvoton_m487](../nuvoton_m487)。

CH585 也能做 USB HS 8 kHz + 片内 BLE/2.4G，但不是这个项目要的料。新板不要画 CH585。

下面内容仅给这颗评估板作参考。

## 编译（有线 8 kHz HID）

```bash
cd firmware/ch585
make
```

`build/controller.hex` / `build/dongle.hex`。WCHISPTool 从 USB FS 下载，打游戏走 USB HS。

射频默认关（`WF_ENABLE_RF=0`）。BLE/2.4G 要 MounRiver + 沁恒 ROM：`mrs/README.md`。
