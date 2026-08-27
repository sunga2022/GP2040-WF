#!/usr/bin/env python3
"""Generate 嘉立创EDA / EasyEDA Standard JSON (docType 1 schematic, 3 PCB).

Units inside the file are 10 mil: 1 unit = 0.254 mm.
Canvas origin is (4000, 3000). Board millimetres map with Y downward.

Open in 立创EDA 标准版: 文件 → 打开 → 选 .json
Not EasyEDA Pro (.epro / .epcb JSONL).
"""
from __future__ import annotations

import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))
from generate_pcb import QFN_PADS, RP2040, STICK_GPIO  # noqa: E402

SOIC_MOD = ROOT / "lib" / "SOIC-8.kicad_mod"
OUT = Path(__file__).resolve().parent

MM_PER_EU = 0.254
ORIGIN = (4000.0, 3000.0)
EDITOR = "6.5.45"

# Stick button pads — same millimetre map as pcb/generate_pcb.py
BTN_XY = {
    "UP": (18, 62),
    "LEFT": (10, 70),
    "DOWN": (18, 78),
    "RIGHT": (26, 70),
    "B3": (78, 58),
    "B4": (90, 54),
    "R1": (102, 58),
    "L1": (114, 62),
    "B1": (78, 70),
    "B2": (90, 66),
    "R2": (102, 70),
    "L2": (114, 74),
    "S1": (48, 54),
    "S2": (60, 54),
    "A1": (48, 46),
    "A2": (60, 46),
    "L3": (36, 70),
    "R3": (66, 70),
    "TURBO": (66, 78),
}
BTN_GPIO = {
    "UP": 20,
    "DOWN": 22,
    "LEFT": 23,
    "RIGHT": 16,
    "B1": 18,
    "B2": 13,
    "B3": 12,
    "B4": 9,
    "R1": 10,
    "L1": 11,
    "R2": 14,
    "L2": 5,
    "S1": 8,
    "S2": 17,
    "A1": 7,
    "A2": 6,
    "L3": 2,
    "R3": 19,
    "TURBO": 24,
}

LAYERS = [
    "1~TopLayer~#FF0000~true~true~true~",
    "2~BottomLayer~#0000FF~true~false~true~",
    "3~TopSilkLayer~#FFCC00~true~true~true~",
    "4~BottomSilkLayer~#66CC33~true~false~true~",
    "5~TopPasteMaskLayer~#808080~true~true~true~",
    "6~BottomPasteMaskLayer~#800000~true~false~true~",
    "7~TopSolderMaskLayer~#800080~true~true~true~0.3",
    "8~BottomSolderMaskLayer~#AA00FF~true~false~true~0.3",
    "10~BoardOutLine~#FF00FF~true~true~true~",
    "11~Multi-Layer~#C0C0C0~true~true~true~",
    "12~Document~#FFFFFF~true~true~true~",
]
OBJECTS = [
    "All~true~false",
    "Component~true~true",
    "Prefix~true~true",
    "Name~true~true",
    "Track~true~true",
    "Pad~true~true",
    "Via~true~true",
    "Hole~true~true",
    "Copper_Area~true~true",
    "Circle~true~true",
    "Arc~true~true",
    "Solid_Region~true~true",
    "Text~true~true",
    "Image~true~true",
    "Rect~true~true",
    "Dimension~true~true",
    "Protractor~true~true",
]


def eu(mm: float) -> float:
    return round(mm / MM_PER_EU, 4)


def xy(xmm: float, ymm: float) -> tuple[float, float]:
    return (round(ORIGIN[0] + eu(xmm), 4), round(ORIGIN[1] + eu(ymm), 4))


class Ids:
    def __init__(self) -> None:
        self.n = 0

    def next(self) -> str:
        self.n += 1
        return f"gge{self.n}"


def parse_soic8() -> dict[str, tuple[float, float, float, float]]:
    text = SOIC_MOD.read_text(encoding="utf-8")
    pads: dict[str, tuple[float, float, float, float]] = {}
    for m in re.finditer(
        r'\(pad (\d+) smd roundrect \(at ([-\d.]+) ([-\d.]+)\) \(size ([-\d.]+) ([-\d.]+)\)',
        text,
    ):
        pads[m.group(1)] = (
            float(m.group(2)),
            float(m.group(3)),
            float(m.group(4)),
            float(m.group(5)),
        )
    if len(pads) != 8:
        raise RuntimeError(f"SOIC-8 expected 8 pads, got {len(pads)}")
    return pads


SOIC_PADS = parse_soic8()


def rp2040_net(pin: int) -> str:
    name = RP2040.get(pin, "")
    if name.startswith("GPIO"):
        g = int(name.replace("GPIO", ""))
        return f"GP{g}"
    if name in ("IOVDD", "USB_VDD", "ADC_AVDD"):
        return "3V3"
    if name == "DVDD":
        return "VREG_VOUT"
    if name == "TESTEN":
        return "GND"
    if name == "USB_DP":
        return "USB_DP"
    if name == "USB_DM":
        return "USB_DM"
    return name or ""


def canvas_pcb() -> str:
    ox, oy = ORIGIN
    return (
        f"CA~1000~1000~#000000~yes~#FFFFFF~10~1000~1000~line~0.2~mm~1~45~"
        f"visible~0.1~{ox}~{oy}~0~yes"
    )


def canvas_sch() -> str:
    return "CA~1000~1000~#FFFFFF~yes~#CCCCCC~10~1000~1000~line~5~pixel~5~0~0"


def head_pcb(title: str) -> dict:
    return {
        "docType": "3",
        "editorVersion": EDITOR,
        "newgId": True,
        "hasIdFlag": True,
        "x": str(int(ORIGIN[0])),
        "y": str(int(ORIGIN[1])),
        "c_para": {
            "title": title,
            "Router Width": "0.254",
            "Router Angle": "45",
        },
        "title": title,
        "description": "GP2040-WF Pico16 · 嘉立创EDA标准版 JSON",
    }


def head_sch(title: str) -> dict:
    return {
        "docType": "1",
        "editorVersion": EDITOR,
        "newgId": True,
        "hasIdFlag": True,
        "c_para": {"Prefix": "SCHEMATIC", "title": title},
        "title": title,
        "description": "GP2040-WF Pico16 原理框图 · 嘉立创EDA标准版",
    }


def track(ids: Ids, pts_mm: list[tuple[float, float]], width_mm: float, layer: int, net: str = "") -> str:
    pts = " ".join(f"{x} {y}" for x, y in (xy(*p) for p in pts_mm))
    return f"TRACK~{eu(width_mm)}~{layer}~{net}~{pts}~{ids.next()}~0"


def hole(ids: Ids, xmm: float, ymm: float, dia_mm: float) -> str:
    x, y = xy(xmm, ymm)
    return f"HOLE~{x}~{y}~{eu(dia_mm)}~{ids.next()}~0"


def via(ids: Ids, xmm: float, ymm: float, dia_mm: float = 0.6, hole_r_mm: float = 0.15, net: str = "") -> str:
    x, y = xy(xmm, ymm)
    return f"VIA~{x}~{y}~{eu(dia_mm)}~{net}~{eu(hole_r_mm)}~{ids.next()}~0"


def circle(ids: Ids, xmm: float, ymm: float, r_mm: float, layer: int, width_mm: float = 0.15) -> str:
    x, y = xy(xmm, ymm)
    return f"CIRCLE~{x}~{y}~{eu(r_mm)}~{eu(width_mm)}~{layer}~{ids.next()}~0~~"


def text(ids: Ids, xmm: float, ymm: float, s: str, size_mm: float = 1.2, layer: int = 3, rot: float = 0, kind: str = "L") -> str:
    x, y = xy(xmm, ymm)
    stroke = eu(max(0.12, size_mm * 0.08))
    return (
        f"TEXT~{kind}~{x}~{y}~{stroke}~{rot}~0~{layer}~~{eu(size_mm)}~{s}~~~{ids.next()}~~0"
    )


def pad(
    ids: Ids,
    shape: str,
    xmm: float,
    ymm: float,
    w_mm: float,
    h_mm: float,
    layer: int,
    net: str,
    number: str,
    hole_r_mm: float = 0.0,
    rot: float = 0.0,
    plated: str = "Y",
) -> str:
    x, y = xy(xmm, ymm)
    return (
        f"PAD~{shape}~{x}~{y}~{eu(w_mm)}~{eu(h_mm)}~{layer}~{net}~{number}~"
        f"{eu(hole_r_mm)}~~{rot}~{ids.next()}~0~~{plated}~0~~~{x},{y}"
    )


def copper_rect(ids: Ids, w: float, h: float, inset: float, layer: int, net: str) -> str:
    x0, y0 = xy(inset, inset)
    x1, y1 = xy(w - inset, h - inset)
    path = f"M {x0} {y0} L {x1} {y0} L {x1} {y1} L {x0} {y1} Z"
    return (
        f"COPPERAREA~{eu(0.254)}~{layer}~{net}~{path}~{eu(0.254)}~solid~"
        f"{ids.next()}~spoke~none~~0~~1~1~1~1~yes~0"
    )


def outline_rect(ids: Ids, w: float, h: float) -> list[str]:
    return [track(ids, [(0, 0), (w, 0), (w, h), (0, h), (0, 0)], 0.15, 10)]


def lib_wrap(ids: Ids, xmm: float, ymm: float, package: str, ref: str, inner: list[str], rot: float = 0) -> str:
    x, y = xy(xmm, ymm)
    body = "#@$".join(inner)
    return f"LIB~{x}~{y}~package`{package}`pre`{ref}`~{rot}~~{ids.next()}~1#@${body}"


def qfn56_lib(ids: Ids, cx: float, cy: float, ref: str, net_of) -> str:
    """RP2040 QFN-56. KiCad +Y up → EasyEDA +Y down (negate local Y)."""
    inner: list[str] = []
    half = 3.5
    inner.append(track(ids, [(cx - half, cy - half), (cx + half, cy - half), (cx + half, cy + half), (cx - half, cy + half), (cx - half, cy - half)], 0.12, 3))
    inner.append(circle(ids, cx - 3.2, cy + 3.2, 0.22, 3))
    has_ep = False
    for num, (px, py, pw, ph) in QFN_PADS.items():
        if not str(num).isdigit():
            continue
        n = int(num)
        ax, ay = cx + px, cy - py
        if n == 57:
            has_ep = True
        inner.append(pad(ids, "RECT", ax, ay, pw, ph, 1, net_of(n) if n != 57 else "GND", str(n)))
    if not has_ep:
        inner.append(pad(ids, "RECT", cx, cy, 3.2, 3.2, 1, "GND", "57"))
    inner.append(text(ids, cx - 4.4, cy - 5.2, ref, 1.1, 3, kind="P"))
    inner.append(text(ids, cx - 4.4, cy + 5.4, "RP2040", 0.9, 3, kind="N"))
    return lib_wrap(ids, cx, cy, "QFN-56-1EP-7x7-P0.4", ref, inner)


def soic8_lib(ids: Ids, cx: float, cy: float, ref: str, name: str, nets: dict[int, str]) -> str:
    inner: list[str] = []
    inner.append(track(ids, [(cx - 2.0, cy - 2.5), (cx + 2.0, cy - 2.5), (cx + 2.0, cy + 2.5), (cx - 2.0, cy + 2.5), (cx - 2.0, cy - 2.5)], 0.12, 3))
    for num, (px, py, pw, ph) in SOIC_PADS.items():
        n = int(num)
        ax, ay = cx + px, cy - py
        inner.append(pad(ids, "RECT", ax, ay, pw, ph, 1, nets.get(n, ""), str(n)))
    inner.append(text(ids, cx - 2.6, cy - 3.4, ref, 1.0, 3, kind="P"))
    inner.append(text(ids, cx - 2.6, cy + 3.6, name, 0.8, 3, kind="N"))
    return lib_wrap(ids, cx, cy, "SOIC-8", ref, inner)


def sot223_lib(ids: Ids, cx: float, cy: float, ref: str = "U3") -> str:
    inner = [
        pad(ids, "RECT", cx, cy + 2.15, 3.8, 2.15, 1, "GND", "TAB"),
        pad(ids, "RECT", cx - 2.3, cy - 2.15, 1.2, 1.5, 1, "3V3", "1"),
        pad(ids, "RECT", cx, cy - 2.15, 1.2, 1.5, 1, "GND", "2"),
        pad(ids, "RECT", cx + 2.3, cy - 2.15, 1.2, 1.5, 1, "VIN", "3"),
        text(ids, cx - 3.4, cy - 4.0, ref, 1.0, 3, kind="P"),
        text(ids, cx - 3.4, cy + 4.4, "AMS1117-3.3", 0.8, 3, kind="N"),
    ]
    return lib_wrap(ids, cx, cy, "SOT-223", ref, inner)


def crystal_lib(ids: Ids, cx: float, cy: float, ref: str = "Y1") -> str:
    inner = [
        pad(ids, "RECT", cx - 1.9, cy - 1.05, 1.5, 1.3, 1, "XIN", "1"),
        pad(ids, "RECT", cx + 1.9, cy - 1.05, 1.5, 1.3, 1, "GND", "2"),
        pad(ids, "RECT", cx + 1.9, cy + 1.05, 1.5, 1.3, 1, "XOUT", "3"),
        pad(ids, "RECT", cx - 1.9, cy + 1.05, 1.5, 1.3, 1, "GND", "4"),
        text(ids, cx - 2.6, cy - 2.6, f"{ref} 12MHz", 0.9, 3, kind="P"),
    ]
    return lib_wrap(ids, cx, cy, "XTAL-3225", ref, inner)


def chip0603(ids: Ids, cx: float, cy: float, ref: str, value: str, net_a: str, net_b: str, kind: str) -> str:
    pkg = "R0603" if kind == "R" else "C0603"
    inner = [
        pad(ids, "RECT", cx - 0.85, cy, 0.9, 0.95, 1, net_a, "1"),
        pad(ids, "RECT", cx + 0.85, cy, 0.9, 0.95, 1, net_b, "2"),
        text(ids, cx - 1.6, cy - 1.35, f"{ref} {value}", 0.65, 3, kind="P"),
    ]
    return lib_wrap(ids, cx, cy, pkg, ref, inner)


def ws2812_lib(ids: Ids, cx: float, cy: float, ref: str = "D1") -> str:
    # 5050 4-pin: DIN VDD GND DOUT
    inner = [
        pad(ids, "RECT", cx - 1.6, cy - 1.6, 1.4, 1.4, 1, "GP28", "DIN"),
        pad(ids, "RECT", cx + 1.6, cy - 1.6, 1.4, 1.4, 1, "5V", "VDD"),
        pad(ids, "RECT", cx - 1.6, cy + 1.6, 1.4, 1.4, 1, "GND", "GND"),
        pad(ids, "RECT", cx + 1.6, cy + 1.6, 1.4, 1.4, 1, "LED_DO", "DOUT"),
        text(ids, cx - 2.6, cy - 3.4, f"{ref} WS2812B", 0.8, 3, kind="P"),
    ]
    return lib_wrap(ids, cx, cy, "LED-5050", ref, inner)


def usb_c_lib(ids: Ids, cx: float, cy: float, ref: str, nets: dict[int, str]) -> str:
    inner: list[str] = []
    inner.append(track(ids, [(cx - 4.5, cy - 2.4), (cx + 4.5, cy - 2.4), (cx + 4.5, cy + 2.4), (cx - 4.5, cy + 2.4), (cx - 4.5, cy - 2.4)], 0.12, 3))
    for i in range(8):
        x = cx - 1.75 + i * 0.5
        inner.append(pad(ids, "RECT", x, cy - 1.55, 0.28, 1.05, 1, nets.get(i + 1, ""), str(i + 1)))
        inner.append(pad(ids, "RECT", x, cy + 1.55, 0.28, 1.05, 1, nets.get(i + 9, ""), str(i + 9)))
    inner.append(pad(ids, "RECT", cx - 4.3, cy, 1.5, 1.1, 1, "GND", "S1"))
    inner.append(pad(ids, "RECT", cx + 4.3, cy, 1.5, 1.1, 1, "GND", "S2"))
    inner.append(text(ids, cx - 4.8, cy - 3.4, ref, 0.9, 3, kind="P"))
    return lib_wrap(ids, cx, cy, "USB-C-16P-PLACEHOLDER", ref, inner)


def sw_6mm(ids: Ids, cx: float, cy: float, ref: str, net: str) -> str:
    inner = [
        pad(ids, "ELLIPSE", cx - 3.25, cy - 2.25, 1.7, 1.7, 11, net, "1", 0.6),
        pad(ids, "ELLIPSE", cx + 3.25, cy - 2.25, 1.7, 1.7, 11, net, "2", 0.6),
        pad(ids, "ELLIPSE", cx - 3.25, cy + 2.25, 1.7, 1.7, 11, "GND", "3", 0.6),
        pad(ids, "ELLIPSE", cx + 3.25, cy + 2.25, 1.7, 1.7, 11, "GND", "4", 0.6),
        circle(ids, cx, cy, 2.6, 3, 0.15),
        text(ids, cx - 2.4, cy - 4.6, ref, 0.9, 3, kind="P"),
    ]
    return lib_wrap(ids, cx, cy, "SW-6x6", ref, inner)


def header_row(
    ids: Ids,
    x0: float,
    y0: float,
    n: int,
    pitch: float,
    ref: str,
    nets: list[str],
    vertical: bool = False,
) -> list[str]:
    shapes: list[str] = [text(ids, x0 - 1.4, y0 - 2.6, ref, 0.9, 3)]
    for i in range(n):
        x, y = (x0, y0 + i * pitch) if vertical else (x0 + i * pitch, y0)
        net = nets[i] if i < len(nets) else ""
        shapes.append(pad(ids, "ELLIPSE", x, y, 1.7, 1.7, 11, net, str(i + 1), 0.45))
        lx, ly = (x + 1.6, y) if vertical else (x - 0.7, y + 1.6)
        shapes.append(text(ids, lx, ly, net or str(i + 1), 0.65, 3))
    return shapes


def bbox(w_mm: float, h_mm: float) -> dict:
    return {
        "x": ORIGIN[0] - 20,
        "y": ORIGIN[1] - 20,
        "width": eu(w_mm) + 40,
        "height": eu(h_mm) + 40,
    }


def dump_pcb(path: Path, title: str, w: float, h: float, shapes: list[str]) -> None:
    doc = {
        "head": head_pcb(title),
        "canvas": canvas_pcb(),
        "shape": shapes,
        "layers": LAYERS,
        "objects": OBJECTS,
        "BBox": bbox(w, h),
        "preference": {"hideFootprints": "", "hideNets": ""},
        "DRCRULE": {
            "Default": {
                "trackWidth": 1,
                "clearance": 0.6,
                "viaHoleDiameter": 2.4,
                "viaHoleD": 1.2,
            },
            "isRealtime": False,
            "isDrcOnRoutingOrPlaceVia": False,
            "checkObjectToCopperarea": True,
            "showDRCRangeLine": True,
        },
        "netColors": {},
    }
    path.write_text(json.dumps(doc, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    print(f"wrote {path.name}  {len(shapes)} shapes  {path.stat().st_size} bytes")


def dump_sch(path: Path, title: str, shapes: list[str]) -> None:
    doc = {
        "head": head_sch(title),
        "canvas": canvas_sch(),
        "shape": shapes,
        "BBox": {"x": 0, "y": 0, "width": 1100, "height": 780},
        "colors": {},
    }
    path.write_text(json.dumps(doc, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    print(f"wrote {path.name}  {len(shapes)} shapes  {path.stat().st_size} bytes")


def _passives(ids: Ids) -> list[str]:
    s = []
    parts = [
        ("C1", 16.0, 22.0, "100n", "3V3", "GND", "C"),
        ("C2", 28.0, 22.0, "100n", "3V3", "GND", "C"),
        ("C3", 16.0, 34.0, "100n", "3V3", "GND", "C"),
        ("C4", 28.0, 34.0, "100n", "VREG_VOUT", "GND", "C"),
        ("C5", 40.0, 22.0, "100n", "3V3", "GND", "C"),
        ("C6", 18.0, 40.5, "10u", "3V3", "GND", "C"),
        ("C7", 18.0, 44.0, "10u", "VIN", "GND", "C"),
        ("C8", 12.4, 14.6, "15p", "XIN", "GND", "C"),
        ("C9", 21.2, 14.6, "15p", "XOUT", "GND", "C"),
        ("Cdiv", 88.0, 48.0, "100n", "GP29", "GND", "C"),
        ("RusbP", 14.0, 32.5, "27R", "JUSB1_DP", "USB_DP", "R"),
        ("RusbN", 14.0, 36.0, "27R", "JUSB1_DM", "USB_DM", "R"),
        ("RdivH", 88.0, 44.5, "100k", "VBAT", "GP29", "R"),
        ("RdivL", 88.0, 51.5, "100k", "GP29", "GND", "R"),
        ("Rled", 40.0, 12.0, "330R", "GP28", "GP28", "R"),
    ]
    for ref, x, y, val, a, b, k in parts:
        s.append(chip0603(ids, x, y, ref, val, a, b, k))
    return s


def build_stick_pcb() -> tuple[float, float, list[str]]:
    ids = Ids()
    w, h = 120.0, 82.0
    s: list[str] = []
    s.extend(outline_rect(ids, w, h))
    s.append(copper_rect(ids, w, h, 0.4, 2, "GND"))
    for hx, hy in [(3.5, 3.5), (w - 3.5, 3.5), (3.5, h - 3.5), (w - 3.5, h - 3.5)]:
        s.append(hole(ids, hx, hy, 3.2))

    u1 = (22.0, 28.0)
    flash = (36.0, 28.0)
    nrf = (93.5, 52.0)
    jerry = (92.0, 18.0)
    oled = (92.0, 32.0)
    bat = (92.0, 42.0)

    s.append(qfn56_lib(ids, u1[0], u1[1], "U1", rp2040_net))
    s.append(
        soic8_lib(
            ids,
            flash[0],
            flash[1],
            "U2",
            "W25Q16JV",
            {
                1: "3V3",
                2: "QSPI_CSn",
                3: "QSPI_SD1",
                4: "GND",
                5: "QSPI_SD0",
                6: "QSPI_SCLK",
                7: "QSPI_SD2",
                8: "QSPI_SD3",
            },
        )
    )
    s.append(sot223_lib(ids, 11.5, 40.5, "U3"))
    s.append(crystal_lib(ids, 16.8, 17.8, "Y1"))
    s.append(
        usb_c_lib(
            ids,
            6.5,
            26.5,
            "JUSB1 DEVICE",
            {1: "GND", 4: "VIN", 5: "CC1", 6: "JUSB1_DP", 7: "JUSB1_DM", 9: "VIN", 12: "GND"},
        )
    )
    s.append(
        usb_c_lib(
            ids,
            6.5,
            52.5,
            "JUSB2 AUTH",
            {1: "GND", 4: "VIN", 5: "CC1", 6: "GP4", 7: "GP3", 9: "VIN", 12: "GND"},
        )
    )
    s.extend(_passives(ids))
    s.append(ws2812_lib(ids, 40.0, 8.5, "D1"))

    s.extend(header_row(ids, jerry[0], jerry[1], 4, 2.54, "J1 JERRY UART", ["GND", "3V3", "GP0_TX", "NC"]))
    s.extend(header_row(ids, oled[0], oled[1], 4, 2.54, "J2 OLED I2C", ["GND", "3V3", "GP26_SDA", "GP27_SCL"]))
    s.extend(header_row(ids, bat[0], bat[1], 3, 2.54, "J3 VBAT ADC GP29", ["GND", "GP29", "VBAT"]))
    s.extend(
        header_row(
            ids,
            nrf[0],
            nrf[1],
            8,
            2.54,
            "U4 Si24R1 MOSI=GP15",
            ["GND", "GP1_CE", "GP21_CSN", "GP25_SCK", "GP15_MOSI", "GP16_MISO", "IRQ", "3V3"],
            vertical=True,
        )
    )

    s.append(sw_6mm(ids, 12.0, 10.0, "BOOT", "QSPI_CSn"))
    s.append(sw_6mm(ids, 22.0, 10.0, "RESET", "RUN"))

    for name, (bx, by) in BTN_XY.items():
        g = BTN_GPIO[name]
        s.append(sw_6mm(ids, bx, by, f"{name} GP{g}", f"GP{g}"))
        s.append(via(ids, bx + 3.6, by, 0.6, 0.15, "GND"))
        s.append(track(ids, [(bx, by), (u1[0] + 8.0, u1[1])], 0.25, 1, f"GP{g}"))

    s.append(track(ids, [(u1[0], u1[1]), (jerry[0], jerry[1])], 0.25, 1, "GP0"))
    s.append(track(ids, [(u1[0] + 4, u1[1]), (nrf[0], nrf[1] + 2.54)], 0.3, 1, "GP1"))
    s.append(track(ids, [(6.5, 26.5), (u1[0] - 4, u1[1])], 0.3, 1, "USB_DP"))
    s.append(track(ids, [(6.5, 52.5), (u1[0] - 4, u1[1] + 4)], 0.3, 1, "GP4"))
    s.append(via(ids, u1[0], u1[1], 0.6, 0.15, "GND"))
    s.append(via(ids, u1[0] + 2.2, u1[1] + 2.2, 0.6, 0.15, "GND"))
    s.append(via(ids, flash[0], flash[1] + 3.2, 0.6, 0.15, "GND"))

    s.append(text(ids, 32, 2.2, "GP2040-WF Pico16  stick  120x82 mm  嘉立创EDA标准版", 1.7, 3))
    s.append(text(ids, 32, 5.0, "FN取消  L2=GP5  MOSI=GP15  VBAT ADC=GP29 经100k/100k  JUSB2=NXP7105", 1.05, 3))
    s.append(text(ids, 4.0, 21.2, "USB-C DEVICE GP47/46", 0.85, 3))
    s.append(text(ids, 4.0, 47.2, "USB-C AUTH GP4/GP3", 0.85, 3))
    return w, h, s


def rx_net(pin: int) -> str:
    return rp2040_net(pin)


def build_receiver_pcb() -> tuple[float, float, list[str]]:
    ids = Ids()
    w, h = 42.0, 18.0
    s: list[str] = []
    s.extend(outline_rect(ids, w, h))
    s.append(copper_rect(ids, w, h, 0.3, 2, "GND"))
    s.append(qfn56_lib(ids, 15.5, 9.0, "U1", rx_net))
    s.append(
        soic8_lib(
            ids,
            23.5,
            8.8,
            "U2",
            "W25Q16",
            {1: "3V3", 2: "QSPI_CSn", 3: "QSPI_SD1", 4: "GND", 5: "QSPI_SD0", 6: "QSPI_SCLK", 7: "QSPI_SD2", 8: "QSPI_SD3"},
        )
    )
    s.append(sot223_lib(ids, 23.5, 14.5, "U4"))
    s.append(crystal_lib(ids, 8.5, 14.2, "Y1"))
    s.append(
        usb_c_lib(
            ids,
            6.0,
            8.5,
            "JUSB HOST",
            {1: "GND", 4: "VIN", 6: "HOST_DP", 7: "HOST_DM", 9: "VIN", 12: "GND"},
        )
    )
    s.extend(
        header_row(
            ids,
            38.0,
            2.2,
            8,
            1.8,
            "U3 nRF MOSI=GP29",
            ["GND", "GP1_CE", "GP21_CSN", "GP25_SCK", "GP29_MOSI", "GP16_MISO", "IRQ", "3V3"],
            vertical=True,
        )
    )
    s.append(chip0603(ids, 30.5, 4.5, "C1", "100n", "3V3", "GND", "C"))
    s.append(chip0603(ids, 30.5, 7.5, "C2", "100n", "3V3", "GND", "C"))
    s.append(chip0603(ids, 30.5, 10.5, "C3", "10u", "3V3", "GND", "C"))
    s.append(chip0603(ids, 30.5, 13.5, "RusbP", "27R", "HOST_DP", "GP4", "R"))
    s.append(chip0603(ids, 34.5, 13.5, "RusbN", "27R", "HOST_DM", "GP3", "R"))
    s.append(text(ids, 1.0, 0.7, "GP2040-WF receiver 42x18  MOSI=GP29 MISO=GP16  D+=GP4 D-=GP3", 0.75, 3))
    s.append(via(ids, 15.5, 9.0, 0.6, 0.15, "GND"))
    return w, h, s


def sch_rect(ids: Ids, x: float, y: float, w: float, h: float) -> str:
    return f"R~{x}~{y}~~~{w}~{h}~#000000~1~0~none~{ids.next()}~0~"


def sch_text(ids: Ids, x: float, y: float, s: str, size: str = "10pt") -> str:
    return f"T~L~{x}~{y}~0~#000000~~{size}~~~~comment~{s}~1~start~{ids.next()}~0~"


def sch_wire(ids: Ids, x1: float, y1: float, x2: float, y2: float) -> str:
    return f"W~{x1} {y1} {x2} {y2}~#008800~1~0~none~{ids.next()}~0"


def build_schematic() -> list[str]:
    ids = Ids()
    s: list[str] = []
    s.append(sch_text(ids, 40, 28, "GP2040-WF Pico16  原理框图  (嘉立创EDA标准版)", "16pt"))
    s.append(sch_text(ids, 40, 48, "投板前：换成嘉立创库 USB-C / RP2040 封装，铺铜，跑 DRC。USB-C 现为占位焊盘。", "9pt"))

    s.append(sch_rect(ids, 300, 80, 320, 520))
    s.append(sch_text(ids, 318, 100, "U1 RP2040 QFN-56", "12pt"))
    s.append(sch_text(ids, 318, 118, "GP2040-CE + WfFrame + Si24R1", "8pt"))

    lines = [
        "GP0  Jerry UART TX 1Mbps",
        "GP1  Si24R1 CE",
        "GP2  L3",
        "GP3  USB AUTH D-  / 接收器 HOST D-",
        "GP4  USB AUTH D+  / 接收器 HOST D+",
        "GP5  L2  (旧FN焊盘，FN已取消)",
        "GP6  A2",
        "GP7  A1",
        "GP8  S1",
        "GP9  B4",
        "GP10 R1",
        "GP11 L1",
        "GP12 B3",
        "GP13 B2",
        "GP14 R2",
        "GP15 Si24R1 MOSI  (摇杆)",
        "GP16 RIGHT / 接收器 MISO",
        "GP17 S2",
        "GP18 B1",
        "GP19 R3",
        "GP20 UP",
        "GP21 Si24R1 CSN",
        "GP22 DOWN",
        "GP23 LEFT",
        "GP24 TURBO",
        "GP25 Si24R1 SCK",
        "GP26 OLED SDA",
        "GP27 OLED SCL",
        "GP28 WS2812",
        "GP29 VBAT ADC3 经100k/100k (摇杆) / MOSI (接收器)",
        "GP46 USB DEVICE D-   GP47 USB DEVICE D+",
        "QSPI 51-56 W25Q16   XIN20/XOUT21 12MHz",
        "IOVDD=3V3  TESTEN=GND  RUN=RESET",
    ]
    for i, line in enumerate(lines):
        s.append(sch_text(ids, 318, 140 + i * 13, line, "8pt"))

    left = [
        (40, 80, 220, 70, "J1 杰里 AC632N UART", "3V3  GND  TX=GP0  RX NC"),
        (40, 170, 220, 70, "U4 Si24R1 2.54 模块", "摇杆 MOSI=GP15  接收器 MOSI=GP29"),
        (40, 260, 220, 70, "J2 OLED 1.3 SSD1306", "SDA GP26  SCL GP27"),
        (40, 350, 220, 70, "J3 锂电池 VBAT", "禁止 4.2V 直灌 GP29  100k/100k"),
        (40, 440, 220, 70, "D1 WS2812B", "DIN = GP28  按键灯 + 电量灯"),
        (40, 530, 220, 70, "按键 对 GND", "无 FN。L2=GP5。BOOT / RESET"),
    ]
    for x, y, bw, bh, title, sub in left:
        s.append(sch_rect(ids, x, y, bw, bh))
        s.append(sch_text(ids, x + 10, y + 22, title, "10pt"))
        s.append(sch_text(ids, x + 10, y + 44, sub, "8pt"))
        s.append(sch_wire(ids, x + bw, y + bh / 2, 300, y + bh / 2))

    right = [
        (660, 80, 240, 80, "JUSB1 USB-C DEVICE", "D+ GP47  D- GP46  VBUS→AMS1117"),
        (660, 180, 240, 80, "JUSB2 USB-C AUTH", "D+ GP4  D- GP3  插 NXP7105"),
        (660, 280, 240, 70, "U3 AMS1117-3.3", "VIN←VBUS  3V3→MCU/Flash/射频"),
        (660, 370, 240, 70, "U2 W25Q16JV SOIC-8", "QSPI Flash"),
        (660, 460, 240, 70, "Y1 12MHz 3225", "XIN/XOUT + 15pF"),
        (660, 550, 240, 70, "接收器 USB HOST", "D+=GP4 D-=GP3  Switch Pro 057E:2009"),
    ]
    for x, y, bw, bh, title, sub in right:
        s.append(sch_rect(ids, x, y, bw, bh))
        s.append(sch_text(ids, x + 10, y + 22, title, "10pt"))
        s.append(sch_text(ids, x + 10, y + 44, sub, "8pt"))
        s.append(sch_wire(ids, 620, y + bh / 2, x, y + bh / 2))

    s.append(sch_text(ids, 40, 640, "Xbox/PS 走手柄 USB 认证口；无线只报 Switch。PS5：JUSB2 插带 NXP7105 的授权口。FORCE_NXP7105_AUTH=1。", "9pt"))
    s.append(sch_text(ids, 40, 662, "GPIO 对照固件 configs/Pico16/BoardConfig.h。RP2040 引脚号数据手册表 615–621。", "9pt"))
    return s


def main() -> None:
    OUT.mkdir(parents=True, exist_ok=True)
    # drop dead helper if someone imported the stub
    w, h, shapes = build_stick_pcb()
    dump_pcb(OUT / "GP2040-WF-stick.json", "GP2040-WF Pico16 stick", w, h, shapes)
    w, h, shapes = build_receiver_pcb()
    dump_pcb(OUT / "GP2040-WF-receiver.json", "GP2040-WF receiver", w, h, shapes)
    dump_sch(OUT / "GP2040-WF-schematic.json", "GP2040-WF Pico16 schematic", build_schematic())

    for name in ("GP2040-WF-stick.json", "GP2040-WF-receiver.json", "GP2040-WF-schematic.json"):
        raw = (OUT / name).read_text(encoding="utf-8")
        doc = json.loads(raw)
        assert raw.lstrip()[0] == "{"
        assert doc["head"]["docType"] in ("1", "3")
        assert isinstance(doc["shape"], list) and doc["shape"]
        assert all(isinstance(x, str) and x.split("~", 1)[0] for x in doc["shape"])

    qfn_n = sum(1 for k in QFN_PADS if str(k).isdigit())
    print(f"QFN numbered pads: {qfn_n}  SOIC pads: {len(SOIC_PADS)}  GPIO keys: {len(STICK_GPIO)}")


if __name__ == "__main__":
    main()
