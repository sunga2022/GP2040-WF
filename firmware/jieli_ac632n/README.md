# 杰里 AC632N：必须单独烧固件

AC632N **一定要烧我们这套蓝牙固件**。手柄上的 `GP2040-WF_Pico16.uf2` 只进 RP2040，**烧不到杰里里**。

模块上若还是厂家 HID 演示程序：不会叫 `Pro Controller`，不会报 VID `057E` / PID `2009`，也解析不了 Pico GP0 送来的 `WfFrame`。不烧的话 **Switch 蓝牙这条路不通**。只用有线或 2.4G 接收器可以不焊、不烧这颗芯片。

这边编不出杰里的 `.bin`：官方工具链是 Windows 上的 CodeBlocks + `fw-AC63_BT_SDK`。包里给的是要贴进 SDK 的源码，你在 Windows 上编完再用杰理下载器烧掉。

## 编

1. 克隆 https://github.com/Jieli-Tech/fw-AC63_BT_SDK
2. 用杰理 CodeBlocks 打开 `apps/hid/board/bd19/AC632N_hid.cbp`（板级目录按你买的模块改，常见是 bd19 / AC632N）
3. `app_config.h`：打开 `TCFG_USER_BLE_ENABLE`、`TCFG_USER_EDR_ENABLE`
4. 蓝牙名改成 **Pro Controller**，VID `0x057E`，PID `0x2009`
5. HID 报告描述符换成仓库 `wireless/switch_pro_desc.h`
6. 工程里加入：
   - `firmware/jieli_ac632n/gp2040wf_uart_hid.c`
   - `wireless/switch_pro.c`
   - include 路径加上 `firmware/jieli_ac632n/` 和 `wireless/`
7. 定义 `JL_AC63_SDK`
8. UART 中断里调 `wf_jieli_on_byte(byte)`（1 Mbps 8N1，Pico **GP0 TX → 杰里 RX**）
9. HID 主机下行调 `wf_jieli_on_hid_output`
10. 主循环调 `wf_jieli_poll()`
11. 编译出 `.bin` / `.ufw`

Xbox / PS **不要**走这颗蓝牙，插手柄 USB 做 CE 引导认证。

## 烧

1. 装杰理官方下载工具（常见名：`isd_download` / 杰理烧录助手），选 **AC632N**
2. 模块进下载模式（多数 AC632N 模块用 USB D+/D-；有的要按住下载键再插电）
3. 选上一步编出的固件，烧写 + 校验
4. 烧完断电，按正常接线：3.3 V、共地、GP0→UART RX

接线：Pico16 **GP0**（G2 用 **GP24**）→ 杰里 UART RX，共地，3.3 V。不要 5 V。
