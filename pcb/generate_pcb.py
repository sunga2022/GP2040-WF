#!/usr/bin/env python3
"""Generate GP2040-WF stick + 2.4G receiver PCB drawings, KiCad, Gerbers, BOM."""
from __future__ import annotations

import math
import os
import re
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parent
QFN_MOD = ROOT / "lib" / "QFN-56.kicad_mod"

# Official RP2040 QFN-56 pin numbers (datasheet tables 615–621).
RP2040 = {
    1: "IOVDD", 2: "GPIO0", 3: "GPIO1", 4: "GPIO2", 5: "GPIO3", 6: "GPIO4",
    7: "GPIO5", 8: "GPIO6", 9: "GPIO7", 10: "IOVDD", 11: "GPIO8", 12: "GPIO9",
    13: "GPIO10", 14: "GPIO11", 15: "GPIO12", 16: "GPIO13", 17: "GPIO14",
    18: "GPIO15", 19: "TESTEN", 20: "XIN", 21: "XOUT", 22: "IOVDD", 23: "DVDD",
    24: "SWCLK", 25: "SWD", 26: "RUN", 27: "GPIO16", 28: "GPIO17", 29: "GPIO18",
    30: "GPIO19", 31: "GPIO20", 32: "GPIO21", 33: "IOVDD", 34: "GPIO22",
    35: "GPIO23", 36: "GPIO24", 37: "GPIO25", 38: "GPIO26", 39: "GPIO27",
    40: "GPIO28", 41: "GPIO29", 42: "IOVDD", 43: "ADC_AVDD", 44: "VREG_VIN",
    45: "VREG_VOUT", 46: "USB_DM", 47: "USB_DP", 48: "USB_VDD", 49: "IOVDD",
    50: "DVDD", 51: "QSPI_SD3", 52: "QSPI_SCLK", 53: "QSPI_SD0", 54: "QSPI_SD2",
    55: "QSPI_SD1", 56: "QSPI_CSn", 57: "GND",
}

# Pico16 firmware GPIO map.
STICK_GPIO = {
    0: "UART_TX/Jerry", 1: "nRF_CE", 2: "L3", 3: "USB_HOST_DM", 4: "USB_HOST_DP",
    5: "L2", 6: "A2/Capture", 7: "A1/Home", 8: "S1/Select", 9: "B4/Y",
    10: "R1", 11: "L1", 12: "B3/X", 13: "B2/B", 14: "R2", 15: "nRF_MOSI",
    16: "RIGHT", 17: "S2/Start", 18: "B1/A", 19: "R3", 20: "UP", 21: "nRF_CSN",
    22: "DOWN", 23: "LEFT", 24: "TURBO", 25: "nRF_SCK", 26: "OLED_SDA",
    27: "OLED_SCL", 28: "WS2812", 29: "VBAT_ADC",
}

RX_GPIO = {1: "nRF_CE", 16: "nRF_MISO", 21: "nRF_CSN", 25: "nRF_SCK", 29: "nRF_MOSI"}


def parse_qfn_pads(path: Path) -> dict[str, tuple[float, float, float, float]]:
    text = path.read_text()
    pads = {}
    for m in re.finditer(
        r'\(pad "([^"]+)" smd roundrect\s+\(at ([-\d.]+) ([-\d.]+)\)\s+\(size ([-\d.]+) ([-\d.]+)\)',
        text,
    ):
        pads[m.group(1)] = (
            float(m.group(2)),
            float(m.group(3)),
            float(m.group(4)),
            float(m.group(5)),
        )
    return pads


QFN_PADS = parse_qfn_pads(QFN_MOD)


class Gerber:
    def __init__(self, name: str):
        self.name = name
        self.ap = {}
        self.body = []
        self._aid = 10

    def _ap_circ(self, d: float) -> str:
        key = ("C", round(d, 4))
        if key not in self.ap:
            self.ap[key] = f"D{self._aid}"
            self._aid += 1
        return self.ap[key]

    def _ap_rect(self, w: float, h: float) -> str:
        key = ("R", round(w, 4), round(h, 4))
        if key not in self.ap:
            self.ap[key] = f"D{self._aid}"
            self._aid += 1
        return self.ap[key]

    def _xy(self, x: float, y: float) -> str:
        return f"X{int(round(x * 1e6))}Y{int(round(y * 1e6))}"

    def flash_circ(self, x, y, d):
        a = self._ap_circ(d)
        self.body.append(f"{a}*\n{self._xy(x, y)}D03*")

    def flash_rect(self, x, y, w, h):
        a = self._ap_rect(w, h)
        self.body.append(f"{a}*\n{self._xy(x, y)}D03*")

    def line(self, x1, y1, x2, y2, w):
        a = self._ap_circ(w)
        self.body.append(f"{a}*\n{self._xy(x1, y1)}D02*\n{self._xy(x2, y2)}D01*")

    def rect_outline(self, x, y, w, h, tw=0.15):
        self.line(x, y, x + w, y, tw)
        self.line(x + w, y, x + w, y + h, tw)
        self.line(x + w, y + h, x, y + h, tw)
        self.line(x, y + h, x, y, tw)

    def write(self, path: Path):
        lines = [
            f"G04 {self.name}*",
            "%FSLAX46Y46*%",
            "%MOMM*%",
            "%LPD*%",
        ]
        for key, name in self.ap.items():
            if key[0] == "C":
                lines.append(f"%AD{name}C,{key[1]:.4f}*%")
            else:
                lines.append(f"%AD{name}R,{key[1]:.4f}X{key[2]:.4f}*%")
        lines.extend(self.body)
        lines.append("M02*")
        path.write_text("\n".join(lines) + "\n")


class Excellon:
    def __init__(self):
        self.holes = []

    def add(self, x, y, d):
        self.holes.append((x, y, d))

    def write(self, path: Path):
        by_d: dict[float, list] = {}
        for x, y, d in self.holes:
            by_d.setdefault(round(d, 3), []).append((x, y))
        lines = ["M48", "METRIC,TZ"]
        tools = {}
        for i, d in enumerate(sorted(by_d), 1):
            tools[d] = f"T{i:02d}"
            lines.append(f"T{i:02d}C{d:.3f}")
        lines.append("%")
        lines.append("G05")
        for d, pts in by_d.items():
            lines.append(tools[d])
            for x, y in pts:
                lines.append(f"X{x:.3f}Y{y:.3f}")
        lines.append("M30")
        path.write_text("\n".join(lines) + "\n")


def svg_header(w, h, title):
    return f'''<?xml version="1.0" encoding="UTF-8"?>
<svg xmlns="http://www.w3.org/2000/svg" viewBox="-8 -12 {w+16} {h+24}" width="{3*(w+16)}" height="{3*(h+24)}">
<title>{title}</title>
<style>
  .brd {{ fill:#1a6b3c; stroke:#0d3d22; stroke-width:0.35; }}
  .cu {{ fill:#c9a227; stroke:#8a7018; stroke-width:0.05; }}
  .mask {{ fill:#1f7a44; }}
  .silk {{ fill:none; stroke:#f4f0e0; stroke-width:0.18; }}
  .slkt {{ fill:#f4f0e0; font: 600 1.35px "DejaVu Sans", sans-serif; }}
  .slks {{ fill:#f4f0e0; font: 500 1.05px "DejaVu Sans", sans-serif; }}
  .th {{ fill:#d8d8d8; stroke:#666; stroke-width:0.08; }}
  .hole {{ fill:#1a1a1a; }}
  .ic {{ fill:#1c1c1c; stroke:#888; stroke-width:0.08; }}
  .mod {{ fill:#2a2a2a; stroke:#ccc; stroke-width:0.12; }}
  .usb {{ fill:#cfcfcf; stroke:#555; stroke-width:0.1; }}
  .dim {{ fill:#222; font: 500 1.2px "DejaVu Sans", sans-serif; }}
</style>
<rect class="brd" x="0" y="0" width="{w}" height="{h}" rx="1.5"/>
'''


def svg_th(x, y, od=1.7, id_=0.9, label=None):
    s = f'<circle class="th" cx="{x:.2f}" cy="{y:.2f}" r="{od/2:.2f}"/><circle class="hole" cx="{x:.2f}" cy="{y:.2f}" r="{id_/2:.2f}"/>'
    if label:
        s += f'<text class="slks" x="{x+1.2:.2f}" y="{y+0.4:.2f}">{label}</text>'
    return s


def kicad_pcb_header(name, w, h, nets):
    layers = """  (layers
    (0 "F.Cu" signal)
    (31 "B.Cu" signal)
    (36 "B.SilkS" user "B.Silkscreen")
    (37 "F.SilkS" user "F.Silkscreen")
    (38 "B.Mask" user)
    (39 "F.Mask" user)
    (44 "Edge.Cuts" user)
    (46 "B.CrtYd" user)
    (47 "F.CrtYd" user)
    (48 "B.Fab" user)
    (49 "F.Fab" user)
  )"""
    net_s = "\n".join(f'  (net {i} "{n}")' for i, n in enumerate(nets))
    return f'''(kicad_pcb
  (version 20240108)
  (generator "gp2040wf")
  (general (thickness 1.6))
  (paper "A4")
  (title_block (title "{name}") (date "2026-08-26") (rev "1.0") (company "sunga2022 / GP2040-WF"))
{layers}
  (setup (pad_to_mask_clearance 0.05) (aux_axis_origin 0 0))
{net_s}
  (gr_rect (start 0 0) (end {w} {h}) (stroke (width 0.1) (type default)) (fill none) (layer "Edge.Cuts"))
'''


def emit_qfn(cx, cy, nets_by_pin: dict[int, tuple[int, str]]) -> str:
    """Embed RP2040 QFN at (cx,cy) board mm, KiCad Y+ up from bottom-left via at."""
    parts = [
        f'  (footprint "Package_DFN_QFN:QFN-56-1EP_7x7mm_P0.4mm_EP3.2x3.2mm" (layer "F.Cu")',
        f'    (at {cx:.4f} {cy:.4f})',
        f'    (descr "RP2040")',
        f'    (property "Reference" "U1" (at 0 -5.2 0) (layer "F.SilkS") (effects (font (size 0.8 0.8) (thickness 0.12))))',
        f'    (property "Value" "RP2040" (at 0 5.2 0) (layer "F.Fab") (effects (font (size 0.8 0.8) (thickness 0.12))))',
        f'    (attr smd)',
        f'    (fp_text user "RP2040" (at 0 0) (layer "F.SilkS") (effects (font (size 0.7 0.7) (thickness 0.1))))',
    ]
    for num, (x, y, w, h) in QFN_PADS.items():
        net = ""
        if num.isdigit():
            n = int(num)
            if n in nets_by_pin:
                ni, nn = nets_by_pin[n]
                net = f' (net {ni} "{nn}")'
        parts.append(
            f'    (pad "{num}" smd roundrect (at {x} {y}) (size {w} {h}) '
            f'(layers "F.Cu" "F.Mask" "F.Paste") (roundrect_rratio 0.25){net})'
        )
    parts.append("  )")
    return "\n".join(parts)


def net_index(nets, name):
    if name not in nets:
        nets.append(name)
    return nets.index(name)


def stick_board(out: Path):
    w, h = 120.0, 82.0
    nets = [""]
    gpio_net = {g: f"GP{g}" for g in STICK_GPIO}
    for n in ["GND", "3V3", "5V", "USB_DP", "USB_DM", "QSPI_CSn", "QSPI_SCLK",
              "QSPI_SD0", "QSPI_SD1", "QSPI_SD2", "QSPI_SD3", "XIN", "XOUT",
              "RUN", "SWCLK", "SWD", "VREG_VIN", "VREG_VOUT"] + list(gpio_net.values()):
        net_index(nets, n)

    qfn_nets = {}
    for pin, name in RP2040.items():
        if name.startswith("GPIO"):
            g = int(name.replace("GPIO", "").split()[0].split("/")[0])
            nn = gpio_net.get(g, name)
        elif name in ("IOVDD", "USB_VDD", "ADC_AVDD"):
            nn = "3V3"
        elif name == "DVDD":
            nn = "VREG_VOUT"
        elif name == "TESTEN":
            nn = "GND"
        else:
            nn = name if name in nets else name
            net_index(nets, nn)
        qfn_nets[pin] = (net_index(nets, nn), nn)

    u1 = (22.0, 28.0)
    flash = (36.0, 28.0)
    usb = (4.0, 28.0)
    nrf = (100.0, 58.0)
    jerry = (100.0, 18.0)
    oled = (100.0, 38.0)

    # Stickless button coordinates (mm), left cluster + right 8 + meta
    buttons = {
        "UP": (18, 62), "LEFT": (10, 70), "DOWN": (18, 78), "RIGHT": (26, 70),
        "B3": (78, 58), "B4": (90, 54), "R1": (102, 58), "L1": (114, 62),
        "B1": (78, 70), "B2": (90, 66), "R2": (102, 70), "L2": (114, 74),
        "S1": (48, 54), "S2": (60, 54), "A1": (48, 46), "A2": (60, 46),
        "L3": (36, 70), "R3": (66, 70), "TURBO": (66, 78),
    }
    btn_gpio = {
        "UP": 20, "DOWN": 22, "LEFT": 23, "RIGHT": 16,
        "B1": 18, "B2": 13, "B3": 12, "B4": 9, "R1": 10, "L1": 11, "R2": 14, "L2": 5,
        "S1": 8, "S2": 17, "A1": 7, "A2": 6, "L3": 2, "R3": 19, "TURBO": 24,
    }

    gtl, gbl, gto, gts, gko = Gerber("GTL"), Gerber("GBL"), Gerber("GTO"), Gerber("GTS"), Gerber("GKO")
    drl = Excellon()
    gko.rect_outline(0, 0, w, h, 0.15)

    svg = [svg_header(w, h, "GP2040-WF Pico16 trimode motherboard")]
    svg.append(f'<text class="slkt" x="4" y="-3">GP2040-WF 三模手柄主板  {w:.0f}×{h:.0f} mm  2-layer 1.6mm</text>')
    svg.append('<text class="dim" x="4" y="80.8">USB-C 手柄口 · USB-C 认证口 · Si24R1 · 杰里 UART · RP2040 QFN-56</text>')

    # USB-C outline (device)
    svg.append(f'<rect class="usb" x="0.4" y="22" width="7.5" height="9" rx="0.6"/>')
    svg.append('<text class="slks" x="0.6" y="21.2">USB-C DEVICE</text>')
    gtl.flash_rect(4.0, 26.5, 6.5, 0.6)
    gtl.flash_rect(4.0, 29.5, 6.5, 0.6)

    # RP2040 body
    svg.append(f'<rect class="ic" x="{u1[0]-3.5:.2f}" y="{u1[1]-3.5:.2f}" width="7" height="7" rx="0.3"/>')
    svg.append(f'<text class="slks" x="{u1[0]-3.2:.2f}" y="{u1[1]-4.1:.2f}">U1 RP2040</text>')
    svg.append(f'<circle fill="#eee" cx="{u1[0]-3.1:.2f}" cy="{u1[1]+2.6:.2f}" r="0.25"/>')
    for pin, (px, py, pw, ph) in QFN_PADS.items():
        if not pin.isdigit():
            gtl.flash_rect(u1[0] + px, u1[1] + py, pw, ph)
            continue
        gtl.flash_rect(u1[0] + px, u1[1] + py, pw, ph)
        gts.flash_rect(u1[0] + px, u1[1] + py, pw + 0.1, ph + 0.1)

    # Flash SOIC-8
    svg.append(f'<rect class="ic" x="{flash[0]-2.5:.2f}" y="{flash[1]-2.6:.2f}" width="5" height="5.2"/>')
    svg.append(f'<text class="slks" x="{flash[0]-2.5:.2f}" y="{flash[1]-3.2:.2f}">U2 W25Q16</text>')
    for i in range(8):
        gtl.flash_rect(flash[0] + (-2.48 if i < 4 else 2.48), flash[1] + (-1.9 + (i % 4) * 1.27), 1.6, 0.55)

    # Crystal
    svg.append(f'<rect class="ic" x="15.5" y="16.5" width="3.2" height="2.5"/>')
    svg.append('<text class="slks" x="15.5" y="16">Y1 12MHz</text>')
    gtl.flash_rect(16.3, 17.75, 1.2, 1.1)
    gtl.flash_rect(18.0, 17.75, 1.2, 1.1)

    # LDO
    svg.append('<rect class="ic" x="8" y="38" width="6.5" height="3.5"/>')
    svg.append('<text class="slks" x="8" y="37.4">U3 AMS1117-3.3</text>')
    gtl.flash_rect(9.2, 39.7, 1.5, 1.2)
    gtl.flash_rect(11.2, 39.7, 3.0, 1.6)
    gtl.flash_rect(13.2, 39.7, 1.5, 1.2)

    # nRF24 module 15.4 x 28
    svg.append(f'<rect class="mod" x="{nrf[0]-7.7:.1f}" y="{nrf[1]-4:.1f}" width="15.4" height="28" rx="0.4"/>')
    svg.append(f'<text class="slks" x="{nrf[0]-7.5:.1f}" y="{nrf[1]-4.8:.1f}">U4 Si24R1 / nRF24</text>')
    nrf_pins = ["GND", "CE", "CSN", "SCK", "MOSI", "MISO", "IRQ", "3V3"]
    for i, p in enumerate(nrf_pins):
        x, y = nrf[0] - 6.5, nrf[1] + i * 2.54
        svg.append(svg_th(x, y, 1.6, 0.8, p))
        gtl.flash_circ(x, y, 1.6)
        gbl.flash_circ(x, y, 1.6)
        drl.add(x, y, 0.8)

    # Jerry UART 4-pin
    svg.append(f'<text class="slks" x="{jerry[0]-8:.1f}" y="{jerry[1]-3:.1f}">J1 杰里 AC632N UART</text>')
    for i, p in enumerate(["GND", "3V3", "GP0 TX", "NC"]):
        x, y = jerry[0] - 4 + i * 2.54, jerry[1]
        svg.append(svg_th(x, y, 1.7, 0.9, p))
        gtl.flash_circ(x, y, 1.7)
        drl.add(x, y, 0.9)

    # OLED
    svg.append(f'<text class="slks" x="{oled[0]-8:.1f}" y="{oled[1]-3:.1f}">J2 OLED I2C</text>')
    for i, p in enumerate(["GND", "3V3", "SDA26", "SCL27"]):
        x, y = oled[0] - 4 + i * 2.54, oled[1]
        svg.append(svg_th(x, y, 1.7, 0.9, p))
        gtl.flash_circ(x, y, 1.7)
        drl.add(x, y, 0.9)

    # Battery ADC divider (GP29). 100k/100k: VBAT -- R1 -- GP29 -- R2 -- GND
    bat = (oled[0], oled[1] + 10)
    svg.append(f'<text class="slks" x="{bat[0]-8:.1f}" y="{bat[1]-3:.1f}">J3 VBAT ADC GP29</text>')
    for i, p in enumerate(["GND", "GP29", "VBAT"]):
        x, y = bat[0] - 4 + i * 2.54, bat[1]
        svg.append(svg_th(x, y, 1.7, 0.9, p))
        gtl.flash_circ(x, y, 1.7)
        drl.add(x, y, 0.9)

    # USB host auth
    svg.append('<rect class="usb" x="0.4" y="48" width="7.5" height="9" rx="0.6"/>')
    svg.append('<text class="slks" x="0.6" y="47.3">USB-C AUTH (PS/Xbox)</text>')

    # Buttons
    for name, (x, y) in buttons.items():
        svg.append(f'<circle class="th" cx="{x}" cy="{y}" r="2.6"/>')
        svg.append(f'<circle class="hole" cx="{x}" cy="{y}" r="0.6"/>')
        svg.append(f'<text class="slks" x="{x-2.2:.1f}" y="{y-3.1:.1f}">{name} GP{btn_gpio[name]}</text>')
        gtl.flash_circ(x, y, 5.2)
        gbl.flash_circ(x, y, 5.2)
        drl.add(x, y, 1.2)
        # GND via next to button
        gtl.flash_circ(x + 3.4, y, 0.9)
        gbl.flash_circ(x + 3.4, y, 0.9)
        drl.add(x + 3.4, y, 0.4)
        # trace toward MCU
        gtl.line(x, y, u1[0] + 8, u1[1], 0.25)

    # SPI traces to nRF
    for label, gx in [("CE", 1), ("CSN", 21), ("SCK", 25), ("MOSI", 15)]:
        gtl.line(u1[0] + 4, u1[1], nrf[0] - 6.5, nrf[1], 0.3)
    gtl.line(u1[0], u1[1], jerry[0], jerry[1], 0.25)

    # BOOT / RESET
    svg.append(svg_th(12, 12, 3.2, 1.5, "BOOT"))
    svg.append(svg_th(20, 12, 3.2, 1.5, "RESET"))
    gtl.flash_circ(12, 12, 3.2)
    gtl.flash_circ(20, 12, 3.2)
    drl.add(12, 12, 1.5)
    drl.add(20, 12, 1.5)

    svg.append("</svg>")

    out.mkdir(parents=True, exist_ok=True)
    (out / "GP2040-WF-stick.svg").write_text("".join(svg), encoding="utf-8")
    gtl.write(out / "gerber" / "Stick-F_Cu.gtl") if False else None
    gerber_dir = out / "gerber"
    gerber_dir.mkdir(exist_ok=True)
    gtl.write(gerber_dir / "Stick-F_Cu.gtl")
    gbl.write(gerber_dir / "Stick-B_Cu.gbl")
    gto.write(gerber_dir / "Stick-F_SilkS.gto")
    gts.write(gerber_dir / "Stick-F_Mask.gts")
    gko.write(gerber_dir / "Stick-Edge_Cuts.gko")
    drl.write(gerber_dir / "Stick.drl")

    pcb = [kicad_pcb_header("GP2040-WF Stick Pico16", w, h, nets)]
    pcb.append(emit_qfn(u1[0], u1[1], qfn_nets))
    pcb.append(
        f'  (gr_text "GP2040-WF Pico16 trimode  USB+BT+2.4G" (at 60 6) (layer "F.SilkS") '
        f'(effects (font (size 1.6 1.6) (thickness 0.2))))'
    )
    pcb.append(")\n")
    (out / "GP2040-WF-stick.kicad_pcb").write_text("\n".join(pcb), encoding="utf-8")
    (out / "GP2040-WF-stick.kicad_pro").write_text(
        '{\n  "board": {"design_settings": {}},\n  "meta": {"filename": "GP2040-WF-stick.kicad_pro", "version": 3},\n'
        '  "text_variables": {}\n}\n',
        encoding="utf-8",
    )

    bom = ["Reference,Value,Footprint,Qty,Notes"]
    bom += [
        "U1,RP2040,QFN-56 7x7 P0.4,1,Raspberry Pi",
        "U2,W25Q16JVUXIQ,SOIC-8,1,or W25Q128",
        "U3,AMS1117-3.3,SOT-223,1,5V to 3V3",
        "U4,Si24R1 / nRF24L01+,2.54 8-pin module,1,2.4G TX",
        "Y1,12MHz 18pF,3225,1,USB bootloader requires 12MHz",
        "JUSB1,USB-C 16P,USB-C-16,1,device to PC/console",
        "JUSB2,USB-C 16P,USB-C-16,1,PS/Xbox auth dongle host",
        "J1,4P 2.54,Header,1,Jerry AC632N UART 3V3 GND GP0",
        "J2,4P 2.54,Header,1,SSD1306 I2C GP26/27",
        "J3,3P 2.54,Header,1,VBAT ADC GP29 through 100k/100k divider",
        "Rdiv,100k,0402,2,VBAT divider to GP29",
        "SW_BOOT,6x6 tactile,TH,1,USB boot",
        "SW_RST,6x6 tactile,TH,1,RUN to GND",
        "BTN_*,Arcade / 6mm pad,TH 1.2mm,20,see silk GP numbers",
        "C_bulk,10uF 16V,0805,4,3V3 and 5V",
        "C_dec,100nF,0402,12,each IOVDD + flash + radio",
        "C_xosc,15pF,0402,2,crystal load",
        "R_usb,27 ohm,0402,2,USB_DP/DM series",
        "LED,WS2812B,5050,15,GP28 buttons 0-13 plus battery lamp 14",
    ]
    (out / "BOM.csv").write_text("\n".join(bom) + "\n", encoding="utf-8")
    return w, h


def receiver_board(out: Path):
    w, h = 42.0, 18.0
    svg = [svg_header(w, h, "GP2040-WF 2.4G Switch Pro receiver")]
    svg.append(f'<text class="slkt" x="2" y="-3">2.4G 接收器  {w:.0f}×{h:.0f} mm  USB-C  ·  RP2040  ·  Si24R1</text>')
    svg.append('<rect class="usb" x="0.3" y="4.5" width="7.2" height="9" rx="0.5"/>')
    svg.append('<text class="slks" x="0.5" y="3.8">USB-C</text>')
    svg.append('<rect class="ic" x="12" y="5.5" width="7" height="7" rx="0.3"/>')
    svg.append('<text class="slks" x="12" y="5">U1 RP2040</text>')
    svg.append('<rect class="ic" x="21" y="6.2" width="5" height="5.2"/>')
    svg.append('<text class="slks" x="21" y="5.6">U2 FLASH</text>')
    svg.append('<rect class="mod" x="28.2" y="1.2" width="12.5" height="15.6" rx="0.3"/>')
    svg.append('<text class="slks" x="28.4" y="0.7">U3 Si24R1</text>')
    pins = [("CE", "GP1"), ("CSN", "GP21"), ("SCK", "GP25"), ("MOSI", "GP29"), ("MISO", "GP16")]
    for i, (a, b) in enumerate(pins):
        svg.append(f'<text class="slks" x="28.5" y="{3.2 + i * 2.6}">{a} {b}</text>')
    svg.append('<text class="dim" x="2" y="16.8">VID 057E PID 2009 Switch Pro USB · 插 Switch 底座或 Steam</text>')
    svg.append("</svg>")

    gtl, gbl, gko = Gerber("GTL"), Gerber("GBL"), Gerber("GKO")
    drl = Excellon()
    gko.rect_outline(0, 0, w, h, 0.12)
    # USB pads
    for i in range(6):
        gtl.flash_rect(3.0, 6 + i * 1.1, 4.5, 0.5)
    # QFN
    for pin, (px, py, pw, ph) in QFN_PADS.items():
        gtl.flash_rect(15.5 + px, 9 + py, pw, ph)
    # module header
    for i in range(8):
        x, y = 30.5, 2.5 + i * 1.7
        gtl.flash_circ(x, y, 1.5)
        drl.add(x, y, 0.8)

    out.mkdir(parents=True, exist_ok=True)
    (out / "GP2040-WF-receiver.svg").write_text("".join(svg), encoding="utf-8")
    gerber_dir = out / "gerber"
    gerber_dir.mkdir(exist_ok=True)
    gtl.write(gerber_dir / "RX-F_Cu.gtl")
    gbl.write(gerber_dir / "RX-B_Cu.gbl")
    gko.write(gerber_dir / "RX-Edge_Cuts.gko")
    drl.write(gerber_dir / "RX.drl")

    nets = ["", "GND", "3V3", "USB_DP", "USB_DM"]
    pcb = [kicad_pcb_header("GP2040-WF 2.4G Receiver", w, h, nets)]
    qfn_nets = {57: (1, "GND"), 46: (3, "USB_DM"), 47: (4, "USB_DP")}
    pcb.append(emit_qfn(15.5, 9.0, qfn_nets))
    pcb.append(")\n")
    (out / "GP2040-WF-receiver.kicad_pcb").write_text("\n".join(pcb), encoding="utf-8")
    (out / "GP2040-WF-receiver.kicad_pro").write_text(
        '{\n  "board": {"design_settings": {}},\n  "meta": {"filename": "GP2040-WF-receiver.kicad_pro", "version": 3},\n'
        '  "text_variables": {}\n}\n',
        encoding="utf-8",
    )
    (out / "BOM.csv").write_text(
        "\n".join([
            "Reference,Value,Footprint,Qty,Notes",
            "U1,RP2040,QFN-56 7x7 P0.4,1,",
            "U2,W25Q16JV,SOIC-8,1,",
            "U3,Si24R1 / nRF24L01+,2.54 8-pin,1,MISO wired GP16",
            "Y1,12MHz,3225,1,",
            "JUSB,USB-C 16P,USB-C,1,Switch Pro 057E:2009",
            "U4,AMS1117-3.3,SOT-223,1,",
            "R_usb,27 ohm,0402,2,",
            "C_dec,100nF,0402,8,",
        ]) + "\n",
        encoding="utf-8",
    )


def schematic_svg(path: Path):
    s = '''<?xml version="1.0" encoding="UTF-8"?>
<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1100 780" width="1100" height="780">
<rect width="1100" height="780" fill="#f7f4ea"/>
<text x="32" y="40" font-size="22" font-family="DejaVu Sans" font-weight="700">GP2040-WF 原理图 · 三模手柄 + 2.4G 接收器</text>
<text x="32" y="64" font-size="13" font-family="DejaVu Sans">RP2040 QFN-56 pin 号来自 Raspberry Pi 数据手册表 615–621。固件脚位 = configs/Pico16/BoardConfig.h</text>
<rect x="30" y="90" width="280" height="200" rx="8" fill="#fff" stroke="#333"/>
<text x="46" y="118" font-size="16" font-weight="700">U1 RP2040</text>
<text x="46" y="142" font-size="12">USB_DP 47 · USB_DM 46 · 27Ω</text>
<text x="46" y="160" font-size="12">XIN 20 / XOUT 21 · 12MHz</text>
<text x="46" y="178" font-size="12">QSPI 51–56 · W25Q16</text>
<text x="46" y="196" font-size="12">VREG_VIN 44 ← 3V3</text>
<text x="46" y="214" font-size="12">VREG_VOUT 45 → DVDD 23/50</text>
<text x="46" y="232" font-size="12">IOVDD 1/10/22/33/42/49 = 3V3</text>
<text x="46" y="250" font-size="12">TESTEN 19 = GND · RUN 26</text>
<text x="46" y="272" font-size="12">BOOT: QSPI_CSn 经 1k 拉到 BOOT 键</text>
<rect x="340" y="90" width="240" height="200" rx="8" fill="#fff" stroke="#333"/>
<text x="356" y="118" font-size="16" font-weight="700">按键 (对 GND)</text>
<text x="356" y="142" font-size="12">GP20 UP  GP22 DOWN</text>
<text x="356" y="160" font-size="12">GP23 LEFT  GP16 RIGHT</text>
<text x="356" y="178" font-size="12">GP18 B1  GP13 B2  GP12 B3  GP9 B4</text>
<text x="356" y="196" font-size="12">GP10 R1  GP11 L1  GP14 R2  GP5 L2</text>
<text x="356" y="214" font-size="12">GP8 S1  GP17 S2  GP2 L3  GP19 R3</text>
<text x="356" y="232" font-size="12">GP7 A1  GP6 A2  GP24 TURBO（无 FN）</text>
<text x="356" y="258" font-size="12">GP26/27 OLED  GP28 WS2812  GP29 ADC</text>
<rect x="610" y="90" width="220" height="200" rx="8" fill="#fff" stroke="#333"/>
<text x="626" y="118" font-size="16" font-weight="700">2.4G Si24R1</text>
<text x="626" y="142" font-size="12">CE  GP1</text>
<text x="626" y="160" font-size="12">CSN GP21</text>
<text x="626" y="178" font-size="12">SCK GP25</text>
<text x="626" y="196" font-size="12">MOSI GP15</text>
<text x="626" y="214" font-size="12">MISO 手柄可不接</text>
<text x="626" y="232" font-size="12">接收器 MISO = GP16</text>
<text x="626" y="258" font-size="12">VCC 3V3  频道 80  2Mbps</text>
<rect x="850" y="90" width="220" height="200" rx="8" fill="#fff" stroke="#333"/>
<text x="866" y="118" font-size="16" font-weight="700">杰里 AC632N</text>
<text x="866" y="142" font-size="12">GP0 UART0 TX 1Mbps</text>
<text x="866" y="160" font-size="12">→ AC632N UART RX</text>
<text x="866" y="178" font-size="12">共地 3.3V</text>
<text x="866" y="196" font-size="12">BT 名 Pro Controller</text>
<text x="866" y="214" font-size="12">VID 057E PID 2009</text>
<text x="866" y="232" font-size="12">Switch Pro 无加密</text>
<text x="866" y="258" font-size="12">Xbox/PS 走手柄 USB 认证</text>
<rect x="30" y="320" width="500" height="200" rx="8" fill="#fff" stroke="#333"/>
<text x="46" y="348" font-size="16" font-weight="700">USB</text>
<text x="46" y="376" font-size="13">JUSB1 设备口：USB_DP/DM + 27Ω → 电脑 / 主机。Xbox/PS 引导认证走这口 + JUSB2 上带 NXP7105 的官方/兼容街机手柄或 dongle。</text>
<text x="46" y="404" font-size="13">JUSB2 主机口：GP4 D+、GP3 D-（PIO USB）。PS5 固定报街机手柄类型 7，不要用 DualShock VID/PID。</text>
<text x="46" y="432" font-size="13">手柄 USB 一旦枚举，固件 tud_mounted() 静音无线，避免双设备。</text>
<text x="46" y="460" font-size="13">开机：A Switch Pro · B Xbox 360 · X PS3 · Y PS4 · R1 Xbox One · L1 PS5。电量 GP29 ADC（100k/100k 分压）。</text>
<rect x="560" y="320" width="510" height="200" rx="8" fill="#fff" stroke="#333"/>
<text x="576" y="348" font-size="16" font-weight="700">接收器（第二块板）</text>
<text x="576" y="376" font-size="13">同样 RP2040 + W25Q16 + 12MHz + USB-C + Si24R1（多一根 MISO GP16）。</text>
<text x="576" y="404" font-size="13">USB 枚举 Switch Pro 057E:2009，处理 0x80 握手和 0x01 子命令。</text>
<text x="576" y="432" font-size="13">空中 20 字节 WfFrame，管道 E7 E7 E7 E7 57，2480MHz。</text>
<text x="576" y="460" font-size="13">插 Switch 底座或 PC/Steam。不要插 Xbox/PS 主机。</text>
<rect x="30" y="546" width="1040" height="200" rx="8" fill="#1a1a1a"/>
<text x="46" y="578" fill="#f4f0e0" font-size="16" font-weight="700">电源</text>
<text x="46" y="606" fill="#ddd" font-size="13">VBUS 5V → AMS1117-3.3 → 3V3。每个 IOVDD 100nF，VREG_VIN 1uF+100nF，VREG_VOUT 1uF 到 DVDD。</text>
<text x="46" y="630" fill="#ddd" font-size="13">ADC_AVDD 经 47Ω + 100nF 到 3V3。USB_VDD 接 3V3。热焊盘 GND 过孔阵列。</text>
<text x="46" y="654" fill="#ddd" font-size="13">投板：嘉立创 2 层，最小线宽 0.12mm（QFN 扇出 0.2mm pad / 0.12mm 线），过孔 0.3/0.15 或 0.4/0.2。</text>
<text x="46" y="686" fill="#f4c542" font-size="13">QFN-56 必须按数据手册焊盘图。先在 KiCad 打开 .kicad_pcb 填铜、DRC，再下 Gerber。</text>
<text x="46" y="718" fill="#aaa" font-size="12">GP2040-WF · sunga2022 · 2026-08-26 · 与仓库 configs/Pico16 和 firmware/receiver_pico 一致</text>
</svg>
'''
    path.write_text(s, encoding="utf-8")


def zip_gerbers(folder: Path, zip_name: str):
    zpath = folder.parent / zip_name
    with zipfile.ZipFile(zpath, "w", zipfile.ZIP_DEFLATED) as z:
        for p in sorted(folder.iterdir()):
            z.write(p, p.name)
    return zpath


def main():
    stick = ROOT / "stick"
    rx = ROOT / "receiver"
    stick.mkdir(parents=True, exist_ok=True)
    rx.mkdir(parents=True, exist_ok=True)
    schematic_svg(ROOT / "GP2040-WF-schematic.svg")
    stick_board(stick)
    receiver_board(rx)
    zip_gerbers(stick / "gerber", "GP2040-WF-stick-gerber.zip")
    zip_gerbers(rx / "gerber", "GP2040-WF-receiver-gerber.zip")
    import sys

    sys.path.insert(0, str(ROOT))
    from draw_sheets import write_all

    write_all()
    try:
        sys.path.insert(0, str(ROOT / "lceda"))
        from generate_lceda import main as lceda_main

        lceda_main()
    except Exception as exc:
        print("lceda skip:", exc)
    readme = ROOT / "README.md"
    readme.write_text(
        """# GP2040-WF PCB

嘉立创标准版 JSON 和 BOM 在 [`lceda/`](lceda/)。板型 Pico19。

重新出 JSON：`python3 pcb/lceda/generate_lceda.py`
""",
        encoding="utf-8",
    )
    # Raster copies for people who do not open SVG.
    try:
        import cairosvg

        for svg, png, wpx in [
            (ROOT / "GP2040-WF-schematic.svg", ROOT / "GP2040-WF-schematic.png", 1800),
            (stick / "GP2040-WF-stick.svg", stick / "GP2040-WF-stick-1to1.png", 2400),
            (rx / "GP2040-WF-receiver.svg", rx / "GP2040-WF-receiver-1to1.png", 2000),
        ]:
            cairosvg.svg2png(url=str(svg), write_to=str(png), output_width=wpx)
    except Exception as exc:
        print("png skip:", exc)
    print("wrote", ROOT)


if __name__ == "__main__":
    print("PCB deliverable is 嘉立创 JSON. Run: python3 pcb/lceda/generate_lceda.py")
