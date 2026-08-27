# GP2040-WF PCB

看图从 `sheets/` 开始（字大、能读）。1:1 Gerber / KiCad 在 `stick/`、`receiver/`。

| 图纸 | 文件 |
|------|------|
| 原理图 | [GP2040-WF-schematic.png](GP2040-WF-schematic.png) |
| 手柄装配图 | [sheets/stick-layout.png](sheets/stick-layout.png) |
| GPIO 脚位表 | [sheets/gpio-map.png](sheets/gpio-map.png) |
| 接收器 | [sheets/receiver.png](sheets/receiver.png) |

**嘉立创EDA 标准版 JSON**（可直接打开编辑）：[`lceda/`](lceda/)

- `stick/` 三模手柄主板 120×82 mm（RP2040 + Si24R1 + 杰里 UART + 双 USB-C + GP29 电量 ADC）
- `receiver/` 2.4G 接收器 42×18 mm（MOSI 仍是 GP29）

KiCad 8：`*.kicad_pcb`。投产前请铺 GND 铜皮、补齐 USB-C 封装、跑 DRC。QFN-56 按 RP2040 数据手册表 615–621。

重新出图：`python3 pcb/generate_pcb.py`  
只出嘉立创 JSON：`python3 pcb/lceda/generate_lceda.py`
