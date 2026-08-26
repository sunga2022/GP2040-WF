# 杰里 AC632N：蓝牙从机

GP2040-CE 已在 UART 上 1 Mbps 送 `WfFrame`。不要自己写按键矩阵。

1. 克隆官方 SDK：https://github.com/Jieli-Tech/fw-AC63_BT_SDK
2. 打开 `apps/hid/board/bd19/AC632N_hid.cbp`
3. 打开 `TCFG_USER_BLE_ENABLE` 和 `TCFG_USER_EDR_ENABLE`
4. 把 `wireless/` 加进 include path（`protocol.h` / `uart_rx.h` / `hid_pack.h`）
5. 把 `gp2040wf_uart_hid.c` 加进工程
6. UART RX 中断里对每个字节调用 `wf_jieli_on_byte(byte)`；初始化时调一次 `wf_jieli_reset()`

接线：Pico16 **GP0** 或 G2 **GP24** → 杰里 UART RX，共地，3.3 V。
