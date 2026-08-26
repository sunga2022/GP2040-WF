# 杰里 AC632N：Switch Pro 蓝牙从机

手柄 USB 拔掉后，GP2040-CE 在 UART 上送 `WfFrame`。开机模式是 Switch / Switch Pro 时，把帧转成 **Switch Pro HID**（报告 0x30，子命令 0x01）。没有加密 IC。

Xbox / PS 不要用这条蓝牙，插手柄 USB 走 CE 引导认证。

1. 克隆 https://github.com/Jieli-Tech/fw-AC63_BT_SDK
2. `apps/hid/board/bd19/AC632N_hid.cbp`
3. `TCFG_USER_BLE_ENABLE` + `TCFG_USER_EDR_ENABLE`
4. 蓝牙名称改成 **Pro Controller**，VID `057E` PID `2009`
5. HID 报告描述符换成 `wireless/switch_pro_desc.h` 里那份
6. include `wireless/`，编译 `gp2040wf_uart_hid.c` + `wireless/switch_pro.c`
7. UART ISR：`wf_jieli_on_byte`；HID 主机下行：`wf_jieli_on_hid_output`；主循环：`wf_jieli_poll`

接线：Pico16 **GP0** 或 G2 **GP24** → 杰里 UART RX，共地，3.3 V。
