# 烧录

就三个文件，都在 `firmware-bin/`。

1. 手柄：`GP2040-WF.uf2` → RP2040（BOOT + 设备口 USB-C）
2. 杰理：`GP2040-WF_AC632N.ufw` → AC632N
3. 2.4G：`GP2040-WF_receiver.hex` → USB-A 接收器 **nRF52820**（J-Link / WCH-Link SWD）

细节见 [`firmware-bin/README.md`](../firmware-bin/README.md)。
