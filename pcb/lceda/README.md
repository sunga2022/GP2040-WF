# 嘉立创EDA 标准版 JSON

这是 **立创EDA / EasyEDA 标准版** 的源文件（`head.docType`：原理图 `1`，PCB `3`），不是专业版 `.epro` / `.epcb`。

| 文件 | 内容 |
|------|------|
| [GP2040-WF-stick.json](GP2040-WF-stick.json) | 手柄主板 PCB，120×82 mm |
| [GP2040-WF-receiver.json](GP2040-WF-receiver.json) | 2.4G 接收器 PCB，42×18 mm |
| [GP2040-WF-schematic.json](GP2040-WF-schematic.json) | 原理框图 |

## 怎么打开

1. 打开 [立创EDA 标准版](https://lceda.cn/editor)（网页或客户端都可以，选 **标准版**，不要用专业版去开这些 JSON）。
2. **文件 → 打开**（或把 JSON 拖进编辑器）。
3. 一次只开一个文件：先开原理图，再分别开两块 PCB。

专业版打不开这套格式。若你已经在用专业版，用标准版另存后再考虑升级，不要直接把 JSON 当 Gerber 下单。

## 打开之后必须做的

这些文件是按 Pico16 脚位排好的**可编辑源**，还不是嘉立创可直接投板的成品。

1. **USB-C**：`USB-C-16P-PLACEHOLDER` 只是 16 个占位焊盘。在封装库换成嘉立创/LCSC 的 USB-C（带外壳焊盘和 CC 电阻）。
2. **RP2040**：QFN-56 焊盘来自官方 7×7 mm P0.4 + 3.2 mm 散热焊盘。建议再对一次嘉立创库里的 RP2040 封装。
3. **铺铜**：底层已放了一块 GND 铺铜区，顶层请自己补，并打过孔。
4. **布线**：只拉了示意线。按网络把 USB、QSPI、晶振、Si24R1、按键补完。
5. **DRC** 通过后再导出 Gerber 下单。

## 脚位（与固件一致）

- 摇杆 **L2 = GP5**（旧 FN 焊盘），**无 FN**。
- 摇杆 Si24R1 **MOSI = GP15**，MISO 走模块第 6 脚（丝印 `GP16_MISO`）。
- 摇杆 **GP29 = VBAT ADC**，必须经 **100k / 100k** 分压，禁止 4.2 V 直灌。
- 接收器 **MOSI = GP29**，**MISO = GP16**，USB Host **D+ = GP4、D− = GP3**。
- 手柄 USB 设备口：RP2040 **USB_DP / USB_DM**（芯片脚 47 / 46）经 27 Ω。
- 手柄认证口 **GP4 / GP3**，插带 **NXP7105** 的授权口。

重新生成：

```bash
python3 pcb/lceda/generate_lceda.py
```
