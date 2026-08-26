# GP2040-WF PCB

两块板：

1. `stick/` 三模手柄主板（RP2040 + Si24R1 + 杰里 UART + 双 USB-C）
2. `receiver/` 2.4G 接收器（RP2040 + Si24R1，USB Switch Pro）

打开 `GP2040-WF-schematic.svg` 看原理图，打开 `stick/GP2040-WF-stick.svg` / `receiver/GP2040-WF-receiver.svg` 看板图。

KiCad 8：`*.kicad_pcb`。投产前请：铺 GND 铜皮、补齐 USB-C 封装、跑 DRC。Gerber zip 可给嘉立创，**建议先在 KiCad 里确认 QFN 扇出**。

RP2040 引脚号来自官方数据手册表 615–621，和 `configs/Pico16/BoardConfig.h` 一致。
