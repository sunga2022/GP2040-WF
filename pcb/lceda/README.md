# 嘉立创EDA 标准版

立创EDA **标准版** → 文件 → 打开 → 选 json。不要用专业版直接开。

| 文件 | 内容 |
|------|------|
| [GP2040-WF-stick.json](GP2040-WF-stick.json) | 手柄 120×82 mm，Pico19 |
| [GP2040-WF-receiver.json](GP2040-WF-receiver.json) | 2.4G USB-A 接收器 **16.30×11.20 mm**，**CH32X035F8U6** 3×3 + **Si24R1** 4×4 |
| [BOM.csv](BOM.csv) | 物料 |

投板前换成库里的 USB-C / USB-A / RP2040 / CH32X035F8U6 / Si24R1 / XN297L 封装，补线，铺铜，跑 DRC。USB-C 现为占位焊盘。接收器封装是占位，3225 晶振在 11.20 mm 板宽上很紧，投板前按库封装微调。
