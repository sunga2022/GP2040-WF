#!/usr/bin/env python3
"""Readable presentation drawings (not 1:1 gerber silk)."""
from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parent

# Pico16 GPIO after FN cancelled: MOSI on 15, ADC on 29, L2 on 5.
GPIO = {
    0: ("Jerry UART TX", "addon"),
    1: ("Si24R1 CE", "radio"),
    2: ("L3", "btn"),
    3: ("USB AUTH D−", "usb"),
    4: ("USB AUTH D+", "usb"),
    5: ("L2  (was FN)", "btn"),
    6: ("A2 Capture", "btn"),
    7: ("A1 Home", "btn"),
    8: ("S1 Select", "btn"),
    9: ("B4 / Y", "btn"),
    10: ("R1", "btn"),
    11: ("L1", "btn"),
    12: ("B3 / X", "btn"),
    13: ("B2 / B", "btn"),
    14: ("R2", "btn"),
    15: ("Si24R1 MOSI", "radio"),
    16: ("RIGHT", "btn"),
    17: ("S2 Start", "btn"),
    18: ("B1 / A", "btn"),
    19: ("R3", "btn"),
    20: ("UP", "btn"),
    21: ("Si24R1 CSN", "radio"),
    22: ("DOWN", "btn"),
    23: ("LEFT", "btn"),
    24: ("TURBO", "btn"),
    25: ("Si24R1 SCK", "radio"),
    26: ("OLED SDA", "i2c"),
    27: ("OLED SCL", "i2c"),
    28: ("WS2812 DIN", "led"),
    29: ("VBAT ADC", "adc"),
}

KIND_FILL = {
    "btn": "#2d6a4f",
    "radio": "#1d4e89",
    "usb": "#7b2d8e",
    "addon": "#9a3412",
    "i2c": "#0f766e",
    "led": "#a16207",
    "adc": "#b45309",
}


def _esc(s: str) -> str:
    return s.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")


def _svg(w: int, h: int, title: str, body: str, subtitle: str | None = None) -> str:
    sub = subtitle or "GP2040-WF Pico16 · 与 firmware-bin/GP2040-WF_Pico16.uf2 同一套脚位 · 无 FN 键"
    return f'''<?xml version="1.0" encoding="UTF-8"?>
<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {w} {h}" width="{w}" height="{h}">
<title>{_esc(title)}</title>
<rect width="{w}" height="{h}" fill="#0f1419"/>
<text x="40" y="48" fill="#f4f0e0" font-size="28" font-family="DejaVu Sans, sans-serif" font-weight="700">{_esc(title)}</text>
<text x="40" y="74" fill="#9aa3ad" font-size="14" font-family="DejaVu Sans, sans-serif">{_esc(sub)}</text>
{body}
</svg>
'''


def box(x, y, w, h, fill, title, lines, title_fill="#f4f0e0"):
    parts = [
        f'<rect x="{x}" y="{y}" width="{w}" height="{h}" rx="10" fill="{fill}" stroke="#d8dee6" stroke-width="1.4"/>',
        f'<text x="{x + 14}" y="{y + 26}" fill="{title_fill}" font-size="16" font-family="DejaVu Sans, sans-serif" font-weight="700">{_esc(title)}</text>',
    ]
    for i, line in enumerate(lines):
        parts.append(
            f'<text x="{x + 14}" y="{y + 50 + i * 18}" fill="#e8edf2" font-size="13" font-family="DejaVu Sans, sans-serif">{_esc(line)}</text>'
        )
    return "\n".join(parts)


def wire(x1, y1, x2, y2, color="#8ab4ff"):
    return f'<path d="M{x1} {y1} L{x2} {y2}" fill="none" stroke="{color}" stroke-width="2"/>'


def schematic() -> str:
    body = []
    # MCU
    body.append(box(40, 100, 280, 390, "#1b2430", "U1 RP2040  QFN-56", [
        "12 MHz 晶振 · W25Q16 QSPI",
        "设备 USB：脚 47 DP / 46 DM",
        "认证 USB：GP4 D+  GP3 D−",
        "3V3 ← AMS1117 ← VBUS 5V",
        "",
        "GP0  UART → 杰里 RX",
        "GP1  Si24R1 CE",
        "GP15 Si24R1 MOSI（从 29 挪来）",
        "GP21 CSN   GP25 SCK",
        "GP26/27 OLED I2C",
        "GP28 WS2812 + 电量灯",
        "GP29 VBAT ADC3 必须分压",
        "GP5  L2（原 FN 焊盘）",
        "FN 按键已取消",
    ]))
    body.append(box(380, 100, 240, 170, "#1d4e89", "U4  Si24R1 / nRF24", [
        "CE   GP1",
        "CSN  GP21",
        "SCK  GP25",
        "MOSI GP15   手柄发射",
        "MISO 可不接",
        "3V3 / GND   2 Mbps ch80",
    ]))
    body.append(box(660, 100, 240, 170, "#9a3412", "杰里 AC632N", [
        "GP0 UART0 TX 1 Mbps",
        "→ 芯片 UART RX (PA5)",
        "共地 3.3 V",
        "BT 名 Pro Controller",
        "VID 057E PID 2009",
        "只做 Switch 经典蓝牙",
    ]))
    body.append(box(940, 100, 240, 170, "#0f766e", "J2 OLED + 灯", [
        "SDA GP26  SCL GP27",
        "SSD1306 128×64",
        "WS2812 DIN GP28",
        "按键灯 0–13 + 电量灯 14",
        "绿=正常  红≈3.5 V",
        "插 USB 一定绿",
    ]))
    body.append(box(380, 300, 240, 200, "#b45309", "J3 电量 ADC", [
        "锂电池 4.2 V 禁止直焊 GP29",
        "VBAT — 100k — GP29",
        "GP29 — 100k — GND",
        "BATTERY_ADC_SCALE = 2.0",
        "空 3.3 V · 满 4.2 V",
        "亏电阈值 3.5 V",
        "RP2040 ADC3 = GPIO29",
    ]))
    body.append(box(660, 300, 240, 200, "#7b2d8e", "USB", [
        "JUSB1 设备口 → 电脑/主机",
        "  USB_DP 47  USB_DM 46",
        "  串 27 Ω",
        "JUSB2 认证口 NXP7105",
        "  GP4 D+  GP3 D−",
        "  PIO USB host order=1",
        "PS5 街机类型 7 引导认证",
    ]))
    body.append(box(940, 300, 240, 200, "#2d6a4f", "按键（对 GND）", [
        "方向  GP20/22/23/16",
        "B1–B4 GP18/13/12/9",
        "R1 L1 R2 L2",
        "     GP10/11/14/5",
        "S1 S2 A1 A2 L3 R3",
        "     8 / 17 / 7 / 6 / 2 / 19",
        "TURBO GP24   无 FN",
    ]))
    body.append(box(40, 520, 1140, 100, "#1b2430", "接收器（第二块板 42×18 mm，脚位与手柄不同）", [
        "RP2040 + Si24R1：CE GP1 · CSN GP21 · SCK GP25 · MOSI GP29 · MISO GP16",
        "接收器 MOSI 仍是 GP29。手柄 MOSI 才是 GP15。USB 枚举 Switch Pro 057E:2009。插 Switch 底座或 Steam，不要插 Xbox / PS。",
    ]))
    body.append(wire(320, 185, 380, 185))
    body.append(wire(320, 220, 380, 220))
    body.append(wire(320, 160, 660, 160, "#ffb38a"))
    body.append(wire(320, 400, 380, 400, "#fbbf24"))
    body.append(wire(320, 440, 660, 400, "#d8b4fe"))
    body.append(
        '<text x="40" y="650" fill="#9aa3ad" font-size="13" font-family="DejaVu Sans, sans-serif">'
        "投板：嘉立创 2 层 1.6 mm。QFN-56 按 RP2040 数据手册表 615–621。先 KiCad 铺铜、DRC，再下 Gerber。</text>"
    )
    return _svg(1220, 680, "GP2040-WF 原理图", "\n".join(body))


def stick_layout() -> str:
    # 10 px / mm, board origin (60, 110)
    s = 10
    ox, oy = 60, 110
    bw, bh = 120 * s, 82 * s

    def X(mm):
        return ox + mm * s

    def Y(mm):
        return oy + mm * s

    parts = [
        f'<rect x="{ox}" y="{oy}" width="{bw}" height="{bh}" rx="14" fill="#1a6b3c" stroke="#d4af37" stroke-width="3"/>',
        f'<text x="{ox + 16}" y="{oy - 12}" fill="#f4f0e0" font-size="18" font-family="DejaVu Sans, sans-serif" font-weight="700">手柄主板 120 × 82 mm  顶层丝印</text>',
    ]

    def usb(x, y, label):
        parts.append(f'<rect x="{X(x)}" y="{Y(y)}" width="{7.5 * s}" height="{9 * s}" rx="6" fill="#d1d5db" stroke="#111"/>')
        parts.append(f'<text x="{X(x)}" y="{Y(y) - 6}" fill="#f4f0e0" font-size="13" font-family="DejaVu Sans, sans-serif">{_esc(label)}</text>')

    usb(0.4, 22, "USB-C 设备口")
    usb(0.4, 48, "USB-C AUTH / NXP7105")

    def ic(x, y, w, h, label):
        parts.append(f'<rect x="{X(x)}" y="{Y(y)}" width="{w * s}" height="{h * s}" rx="4" fill="#111" stroke="#ccc"/>')
        parts.append(f'<text x="{X(x)}" y="{Y(y) - 6}" fill="#f4f0e0" font-size="13" font-family="DejaVu Sans, sans-serif">{_esc(label)}</text>')

    ic(18.5, 24.5, 7, 7, "U1 RP2040")
    ic(33.5, 25.4, 5, 5.2, "U2 FLASH")
    ic(8, 38, 6.5, 3.5, "U3 3V3")
    ic(15.5, 16.5, 3.2, 2.5, "12MHz")
    parts.append(f'<rect x="{X(70)}" y="{Y(6)}" width="{15.4 * s}" height="{20 * s}" rx="6" fill="#222" stroke="#eee"/>')
    parts.append(f'<text x="{X(70)}" y="{Y(6) - 8}" fill="#8ab4ff" font-size="14" font-family="DejaVu Sans, sans-serif" font-weight="700">U4 Si24R1  MOSI=GP15</text>')

    def hdr(x, y, labels, title, color="#f4f0e0"):
        parts.append(f'<text x="{X(x)}" y="{Y(y) - 8}" fill="{color}" font-size="13" font-family="DejaVu Sans, sans-serif" font-weight="700">{_esc(title)}</text>')
        for i, lab in enumerate(labels):
            px = X(x) + i * 26
            py = Y(y)
            parts.append(f'<circle cx="{px}" cy="{py}" r="8" fill="#d8d8d8" stroke="#333"/>')
            parts.append(f'<circle cx="{px}" cy="{py}" r="3" fill="#111"/>')
            parts.append(f'<text x="{px - 10}" y="{py + 22}" fill="#f4f0e0" font-size="11" font-family="DejaVu Sans, sans-serif">{_esc(lab)}</text>')

    hdr(96, 18, ["GND", "3V3", "GP0", "NC"], "J1 杰里 UART")
    hdr(96, 38, ["GND", "3V3", "SDA", "SCL"], "J2 OLED")
    hdr(96, 48, ["GND", "GP29", "VBAT"], "J3 电量 ADC（分压）", "#fbbf24")

    buttons = {
        "UP": (18, 62, 20), "LEFT": (10, 70, 23), "DOWN": (18, 78, 22), "RIGHT": (26, 70, 16),
        "B3": (78, 58, 12), "B4": (90, 54, 9), "R1": (102, 58, 10), "L1": (114, 62, 11),
        "B1": (78, 70, 18), "B2": (90, 66, 13), "R2": (102, 70, 14), "L2": (114, 74, 5),
        "S1": (48, 54, 8), "S2": (60, 54, 17), "A1": (48, 46, 7), "A2": (60, 46, 6),
        "L3": (36, 70, 2), "R3": (66, 70, 19), "TURBO": (66, 78, 24),
    }
    for name, (x, y, gp) in buttons.items():
        fill = "#fbbf24" if name == "L2" else "#e5e7eb"
        parts.append(f'<circle cx="{X(x)}" cy="{Y(y)}" r="22" fill="{fill}" stroke="#111" stroke-width="2"/>')
        parts.append(f'<circle cx="{X(x)}" cy="{Y(y)}" r="5" fill="#111"/>')
        parts.append(f'<text x="{X(x)}" y="{Y(y) - 28}" text-anchor="middle" fill="#fff" font-size="12" font-family="DejaVu Sans, sans-serif" font-weight="700">{name}</text>')
        parts.append(f'<text x="{X(x)}" y="{Y(y) + 38}" text-anchor="middle" fill="#d1fae5" font-size="12" font-family="DejaVu Sans, sans-serif">GP{gp}</text>')

    # BOOT RESET
    parts.append(f'<circle cx="{X(12)}" cy="{Y(12)}" r="14" fill="#e5e7eb" stroke="#111"/>')
    parts.append(f'<text x="{X(12)}" y="{Y(12) - 20}" text-anchor="middle" fill="#fff" font-size="12">BOOT</text>')
    parts.append(f'<circle cx="{X(20)}" cy="{Y(12)}" r="14" fill="#e5e7eb" stroke="#111"/>')
    parts.append(f'<text x="{X(20)}" y="{Y(12) - 20}" text-anchor="middle" fill="#fff" font-size="12">RESET</text>')

    # legend
    lx, ly = 1320, 130
    parts.append(f'<rect x="{lx}" y="{ly}" width="250" height="720" rx="10" fill="#1b2430" stroke="#445"/>')
    parts.append(f'<text x="{lx + 16}" y="{ly + 32}" fill="#f4f0e0" font-size="16" font-weight="700" font-family="DejaVu Sans, sans-serif">改动对照</text>')
    notes = [
        "GP29 电量 ADC",
        "  100k / 100k 分压",
        "  不要直焊 4.2 V",
        "",
        "GP15 Si24R1 MOSI",
        "  原来在 GP29",
        "  原 FN/L2 焊盘改射频",
        "",
        "GP5 = L2",
        "  原来的 FN 固件脚",
        "  没有 FN 键",
        "",
        "认证口 GP4 D+ / GP3 D−",
        "插带 NXP7105 的狗",
        "PS5 街机类型 7",
        "",
        "接收器 MOSI 仍是 GP29",
        "接收器多一根 MISO GP16",
    ]
    for i, n in enumerate(notes):
        parts.append(f'<text x="{lx + 16}" y="{ly + 60 + i * 22}" fill="#e8edf2" font-size="13" font-family="DejaVu Sans, sans-serif">{_esc(n)}</text>')

    return _svg(1600, 980, "GP2040-WF 手柄主板装配图", "\n".join(parts))


def gpio_map() -> str:
    rows = []
    rows.append('<rect x="40" y="96" width="1520" height="40" rx="6" fill="#1d4e89"/>')
    headers = [(50, "GP"), (130, "功能"), (420, "说明"), (900, "分类")]
    for x, t in headers:
        rows.append(f'<text x="{x}" y="122" fill="#fff" font-size="16" font-weight="700" font-family="DejaVu Sans, sans-serif">{t}</text>')
    for i in range(30):
        y = 144 + i * 28
        kind = GPIO[i][1]
        fill = KIND_FILL[kind]
        bg = "#151b22" if i % 2 == 0 else "#1b2430"
        rows.append(f'<rect x="40" y="{y}" width="1520" height="28" fill="{bg}"/>')
        rows.append(f'<rect x="40" y="{y + 4}" width="8" height="20" fill="{fill}"/>')
        rows.append(f'<text x="54" y="{y + 20}" fill="#f4f0e0" font-size="14" font-family="DejaVu Sans, sans-serif">GP{i:02d}</text>')
        rows.append(f'<text x="130" y="{y + 20}" fill="#f4f0e0" font-size="14" font-family="DejaVu Sans, sans-serif">{_esc(GPIO[i][0])}</text>')
        extra = {
            5: "L2 接到这块焊盘，不再是 FN",
            15: "Si24R1 MOSI，从 GP29 挪过来",
            29: "ADC3，锂电池必须分压",
            3: "PIO USB host D−（order=1）",
            4: "PIO USB host D+",
        }.get(i, "")
        rows.append(f'<text x="420" y="{y + 20}" fill="#cbd5e1" font-size="14" font-family="DejaVu Sans, sans-serif">{_esc(extra)}</text>')
        rows.append(f'<text x="900" y="{y + 20}" fill="{fill}" font-size="14" font-family="DejaVu Sans, sans-serif">{kind}</text>')
    return _svg(1600, 1020, "GP2040-WF Pico16 GPIO 脚位表", "\n".join(rows))


def receiver() -> str:
    s = 22
    ox, oy = 80, 140
    parts = [
        f'<rect x="{ox}" y="{oy}" width="{42 * s}" height="{18 * s}" rx="12" fill="#1a6b3c" stroke="#d4af37" stroke-width="3"/>',
        f'<rect x="{ox + 0.3 * s}" y="{oy + 4.5 * s}" width="{7.2 * s}" height="{9 * s}" rx="8" fill="#d1d5db"/>',
        f'<text x="{ox + 8}" y="{oy + 4.2 * s}" fill="#fff" font-size="16">USB-C</text>',
        f'<rect x="{ox + 12 * s}" y="{oy + 5.5 * s}" width="{7 * s}" height="{7 * s}" rx="6" fill="#111" stroke="#ccc"/>',
        f'<text x="{ox + 12 * s}" y="{oy + 5.2 * s}" fill="#fff" font-size="14">U1 RP2040</text>',
        f'<rect x="{ox + 21 * s}" y="{oy + 6.2 * s}" width="{5 * s}" height="{5.2 * s}" fill="#111" stroke="#ccc"/>',
        f'<text x="{ox + 21 * s}" y="{oy + 6 * s}" fill="#fff" font-size="14">FLASH</text>',
        f'<rect x="{ox + 28.2 * s}" y="{oy + 1.2 * s}" width="{12.5 * s}" height="{15.6 * s}" rx="6" fill="#222" stroke="#eee"/>',
        f'<text x="{ox + 28.4 * s}" y="{oy + 1 * s}" fill="#8ab4ff" font-size="16" font-weight="700">U3 Si24R1</text>',
    ]
    pins = [("CE", "GP1"), ("CSN", "GP21"), ("SCK", "GP25"), ("MOSI", "GP29"), ("MISO", "GP16")]
    for i, (a, b) in enumerate(pins):
        parts.append(
            f'<text x="{ox + 29 * s}" y="{oy + (3.4 + i * 2.4) * s}" fill="#f4f0e0" font-size="16" font-family="DejaVu Sans, sans-serif">{a}  {b}</text>'
        )
    parts.append(
        f'<text x="{ox}" y="{oy + 18 * s + 36}" fill="#cbd5e1" font-size="16" font-family="DejaVu Sans, sans-serif">'
        "42 × 18 mm · USB 枚举 Switch Pro 057E:2009 · 接收器 MOSI 仍是 GP29，和手柄 GP15 不同</text>"
    )
    return _svg(
        1200,
        680,
        "GP2040-WF 2.4G 接收器装配图",
        "\n".join(parts),
        "firmware-bin/GP2040-WF_receiver.uf2 · MOSI 仍是 GP29（和手柄 GP15 不同）",
    )


def write_all() -> list[Path]:
    sheets = ROOT / "sheets"
    sheets.mkdir(parents=True, exist_ok=True)
    files = [
        (ROOT / "GP2040-WF-schematic.svg", schematic()),
        (sheets / "stick-layout.svg", stick_layout()),
        (sheets / "gpio-map.svg", gpio_map()),
        (sheets / "receiver.svg", receiver()),
    ]
    for path, text in files:
        path.write_text(text, encoding="utf-8")
    pngs = []
    try:
        import cairosvg
        mapping = [
            (ROOT / "GP2040-WF-schematic.svg", ROOT / "GP2040-WF-schematic.png", 1800),
            (sheets / "stick-layout.svg", sheets / "stick-layout.png", 1800),
            (sheets / "gpio-map.svg", sheets / "gpio-map.png", 1800),
            (sheets / "receiver.svg", sheets / "receiver.png", 1600),
            (sheets / "stick-layout.svg", ROOT / "stick" / "GP2040-WF-stick.png", 1800),
            (sheets / "receiver.svg", ROOT / "receiver" / "GP2040-WF-receiver.png", 1600),
        ]
        for svg, png, w in mapping:
            cairosvg.svg2png(url=str(svg), write_to=str(png), output_width=w)
            pngs.append(png)
    except Exception as exc:
        print("png skip:", exc)
    return [p for p, _ in files] + pngs
