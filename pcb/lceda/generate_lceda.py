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
from generate_pcb import QFN_PADS, RP2040  # noqa: E402

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
    "UP": 15,
    "DOWN": 19,
    "LEFT": 20,
    "RIGHT": 18,
    "B1": 14,
    "B2": 10,
    "B3": 12,
    "B4": 5,
    "R1": 6,
    "L1": 7,
    "R2": 9,
    "L2": 8,
    "S1": 27,
    "S2": 17,
    "A1": 22,
    "A2": 26,
    "L3": 21,
    "R3": 13,
    "TURBO": 23,
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
        "description": "GP2040-WF Pico19 · 嘉立创EDA标准版 JSON",
    }


def head_sch(title: str) -> dict:
    return {
        "docType": "1",
        "editorVersion": EDITOR,
        "newgId": True,
        "hasIdFlag": True,
        "c_para": {"Prefix": "SCHEMATIC", "title": title},
        "title": title,
        "description": "GP2040-WF Pico19 原理框图 · 嘉立创EDA标准版",
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


def crystal_lib(
    ids: Ids,
    cx: float,
    cy: float,
    ref: str = "Y1",
    freq: str = "12MHz",
    net1: str = "XIN",
    net3: str = "XOUT",
) -> str:
    inner = [
        pad(ids, "RECT", cx - 1.9, cy - 1.05, 1.5, 1.3, 1, net1, "1"),
        pad(ids, "RECT", cx + 1.9, cy - 1.05, 1.5, 1.3, 1, "GND", "2"),
        pad(ids, "RECT", cx + 1.9, cy + 1.05, 1.5, 1.3, 1, net3, "3"),
        pad(ids, "RECT", cx - 1.9, cy + 1.05, 1.5, 1.3, 1, "GND", "4"),
        text(ids, cx - 2.6, cy - 2.6, f"{ref} {freq}", 0.75, 3, kind="P"),
    ]
    return lib_wrap(ids, cx, cy, "XTAL-3225", ref, inner)


def sot23_ldo(ids: Ids, cx: float, cy: float, ref: str = "U3") -> str:
    """SOT-23 3.3 V LDO placeholder (ME6206 / XC6206 class). Swap LCSC footprint before fab."""
    inner = [
        pad(ids, "RECT", cx - 0.95, cy + 0.55, 0.8, 0.9, 1, "VIN", "1"),
        pad(ids, "RECT", cx + 0.95, cy + 0.55, 0.8, 0.9, 1, "GND", "2"),
        pad(ids, "RECT", cx, cy - 0.95, 0.8, 0.9, 1, "3V3", "3"),
        text(ids, cx - 2.2, cy - 2.2, f"{ref} 3V3", 0.6, 3, kind="P"),
    ]
    return lib_wrap(ids, cx, cy, "SOT-23", ref, inner)


def qfn20_3x3_lib(ids: Ids, cx: float, cy: float, ref: str, name: str, nets: dict[int, str]) -> str:
    """QFN20 3x3 mm, 0.4 mm pitch, 5 pins per side. KiCad +Y up → EasyEDA +Y down."""
    pitch = 0.4
    span = 0.8
    off = 1.45
    long_pad, short_pad = 0.55, 0.22
    pads: dict[int, tuple[float, float, float, float]] = {}
    for i, y in enumerate([span, span - pitch, 0.0, -pitch, -span]):
        pads[i + 1] = (-off, y, long_pad, short_pad)
    for i, x in enumerate([-span, -pitch, 0.0, pitch, span]):
        pads[i + 6] = (x, -off, short_pad, long_pad)
    for i, y in enumerate([-span, -pitch, 0.0, pitch, span]):
        pads[i + 11] = (off, y, long_pad, short_pad)
    for i, x in enumerate([span, pitch, 0.0, -pitch, -span]):
        pads[i + 16] = (x, off, short_pad, long_pad)
    pads[21] = (0.0, 0.0, 1.55, 1.55)
    inner: list[str] = []
    half = 1.5
    inner.append(
        track(
            ids,
            [(cx - half, cy - half), (cx + half, cy - half), (cx + half, cy + half), (cx - half, cy + half), (cx - half, cy - half)],
            0.1,
            3,
        )
    )
    inner.append(circle(ids, cx - 1.25, cy - 1.25, 0.14, 3))
    for n, (px, py, pw, ph) in pads.items():
        ax, ay = cx + px, cy - py
        inner.append(pad(ids, "RECT", ax, ay, pw, ph, 1, nets.get(n, "GND" if n == 21 else ""), str(n if n != 21 else "EP")))
    inner.append(text(ids, cx - 2.2, cy - 2.6, ref, 0.7, 3, kind="P"))
    inner.append(text(ids, cx - 2.2, cy + 2.7, name, 0.55, 3, kind="N"))
    return lib_wrap(ids, cx, cy, "QFN-20-3x3-P0.4", ref, inner)


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
            {1: "GND", 4: "VIN", 5: "CC1", 6: "GP3", 7: "GP2", 9: "VIN", 12: "GND"},
        )
    )
    s.extend(_passives(ids))
    s.append(ws2812_lib(ids, 40.0, 8.5, "D1"))

    s.extend(header_row(ids, jerry[0], jerry[1], 4, 2.54, "J1 JERRY UART", ["GND", "3V3", "GP24_TX", "NC"]))
    s.extend(header_row(ids, oled[0], oled[1], 4, 2.54, "J2 OLED I2C", ["GND", "3V3", "GP0_SDA", "GP1_SCL"]))
    s.extend(header_row(ids, bat[0], bat[1], 3, 2.54, "J3 VBAT ADC GP29", ["GND", "GP29", "VBAT"]))
    s.append(
        qfn20_3x3_lib(
            ids,
            nrf[0],
            nrf[1],
            "U4",
            "XN297L 3x3",
            {
                1: "GP25",
                2: "GP4",
                3: "GP11",
                4: "GP16",
                5: "NC_MISO",
                6: "IRQ",
                7: "3V3",
                8: "GND",
                9: "RF_XC1",
                10: "RF_XC2",
                13: "ANT",
                15: "3V3",
                20: "GND",
                21: "GND",
            },
        )
    )
    s.append(crystal_lib(ids, nrf[0] + 6.2, nrf[1], "Y2", "16MHz", "RF_XC1", "RF_XC2"))
    s.append(text(ids, nrf[0] - 4.5, nrf[1] - 4.4, "U4 XN297L QFN20 3x3  Si24R1 4x4 电兼容需换封装", 0.7, 3))

    s.append(sw_6mm(ids, 12.0, 10.0, "BOOT", "QSPI_CSn"))
    s.append(sw_6mm(ids, 22.0, 10.0, "RESET", "RUN"))

    for name, (bx, by) in BTN_XY.items():
        g = BTN_GPIO[name]
        s.append(sw_6mm(ids, bx, by, f"{name} GP{g}", f"GP{g}"))
        s.append(via(ids, bx + 3.6, by, 0.6, 0.15, "GND"))
        s.append(track(ids, [(bx, by), (u1[0] + 8.0, u1[1])], 0.25, 1, f"GP{g}"))

    s.append(track(ids, [(u1[0], u1[1]), (jerry[0], jerry[1])], 0.25, 1, "GP24"))
    s.append(track(ids, [(u1[0] + 4, u1[1]), (nrf[0], nrf[1] + 2.54)], 0.3, 1, "GP25"))
    s.append(track(ids, [(6.5, 26.5), (u1[0] - 4, u1[1])], 0.3, 1, "USB_DP"))
    s.append(track(ids, [(6.5, 52.5), (u1[0] - 4, u1[1] + 4)], 0.3, 1, "GP3"))
    s.append(via(ids, u1[0], u1[1], 0.6, 0.15, "GND"))
    s.append(via(ids, u1[0] + 2.2, u1[1] + 2.2, 0.6, 0.15, "GND"))
    s.append(via(ids, flash[0], flash[1] + 3.2, 0.6, 0.15, "GND"))

    s.append(text(ids, 32, 2.2, "GP2040-WF Pico19  stick  120x82 mm  嘉立创EDA标准版", 1.7, 3))
    s.append(text(ids, 32, 5.0, "FN取消  OLED=GP0/1  AUTH D+=GP3 D-=GP2  RGB=GP28  VBAT=GP29  Jerry=GP24", 1.05, 3))
    s.append(text(ids, 4.0, 21.2, "USB-C DEVICE GP47/46", 0.85, 3))
    s.append(text(ids, 4.0, 47.2, "USB-C AUTH GP3/GP2", 0.85, 3))
    return w, h, s


def ch32_f8u6_net(pin: int) -> str:
    """CH32X035F8U6 QFN20 pin numbers (WCH DS / ch32-riscv-ug pin map)."""
    return {
        1: "3V3",
        2: "CE",
        3: "CSN",
        4: "SCK",
        5: "MOSI",
        6: "MISO",
        7: "PA5",
        8: "PA6",
        9: "PA7",
        10: "PB0",
        11: "PB1",
        12: "PB3",
        13: "PB11",
        14: "SWDIO",
        15: "PB12",
        16: "SWCLK",
        17: "USB_DM",
        18: "USB_DP",
        19: "PC14",
        20: "PC15",
        21: "GND",
    }.get(pin, "")


def xn297l_net(pin: int) -> str:
    """Panchip XN297L QFN20 3x3 (CE=1 … ANT=13)."""
    return {
        1: "CE",
        2: "CSN",
        3: "SCK",
        4: "MOSI",
        5: "MISO",
        6: "IRQ",
        7: "3V3",
        8: "GND",
        9: "RF_XC1",
        10: "RF_XC2",
        13: "ANT",
        15: "3V3",
        20: "GND",
        21: "GND",
    }.get(pin, "")


def build_receiver_pcb() -> tuple[float, float, list[str]]:
    """USB-A keyboard dongle: 22 x 12 mm, two QFN20 3x3 (CH32X035F8U6 + XN297L)."""
    ids = Ids()
    w, h = 22.0, 12.0
    s: list[str] = []
    s.extend(outline_rect(ids, w, h))
    s.append(copper_rect(ids, w, h, 0.2, 2, "GND"))
    # USB-A male gold fingers (left ~10 mm plug)
    s.append(track(ids, [(0, 0), (10.0, 0), (10.0, h), (0, h)], 0.1, 3))
    for i, (net, name) in enumerate([("VIN", "VBUS"), ("USB_DM", "D-"), ("USB_DP", "D+"), ("GND", "GND")]):
        y = 1.8 + i * 2.8
        s.append(pad(ids, "RECT", 4.2, y, 7.2, 1.2, 1, net, name))
        s.append(text(ids, 8.0, y - 0.35, name, 0.55, 3))
    mcu = (13.4, 5.0)
    rf = (18.6, 4.4)
    s.append(qfn20_3x3_lib(ids, mcu[0], mcu[1], "U1", "CH32X035 3x3", {n: ch32_f8u6_net(n) for n in range(1, 22)}))
    s.append(qfn20_3x3_lib(ids, rf[0], rf[1], "U2", "XN297L 3x3", {n: xn297l_net(n) for n in range(1, 22)}))
    s.append(crystal_lib(ids, 18.6, 9.2, "Y1", "16MHz", "RF_XC1", "RF_XC2"))
    s.append(sot23_ldo(ids, 13.4, 9.6, "U3"))
    s.append(chip0603(ids, 10.8, 2.2, "C1", "100n", "3V3", "GND", "C"))
    s.append(chip0603(ids, 10.8, 4.4, "C2", "10u", "3V3", "GND", "C"))
    s.append(chip0603(ids, 10.8, 6.6, "C3", "10u", "VIN", "GND", "C"))
    s.append(chip0603(ids, 10.8, 8.8, "C4", "100n", "3V3", "GND", "C"))
    s.append(chip0603(ids, 16.0, 9.6, "Risp", "4k7", "USB_DP", "ISP", "R"))
    s.append(pad(ids, "RECT", 16.0, 11.2, 1.2, 0.9, 1, "ISP", "ISP"))
    s.append(pad(ids, "RECT", 17.4, 11.2, 1.2, 0.9, 1, "3V3", "ISP3V3"))
    s.append(text(ids, 15.2, 11.55, "ISP短接再上电", 0.45, 3))
    s.append(pad(ids, "ELLIPSE", 21.4, 4.4, 1.1, 1.1, 1, "ANT", "ANT"))
    s.append(text(ids, 20.2, 2.5, "ANT", 0.5, 3))
    s.append(track(ids, [(mcu[0], mcu[1]), (rf[0], rf[1])], 0.2, 1, "CE"))
    s.append(track(ids, [(mcu[0] + 0.4, mcu[1] + 0.4), (rf[0] + 0.4, rf[1] + 0.4)], 0.2, 1, "CSN"))
    s.append(track(ids, [(4.2, 4.6), (mcu[0] - 2.2, mcu[1])], 0.25, 1, "USB_DM"))
    s.append(track(ids, [(4.2, 7.4), (mcu[0] - 2.2, mcu[1] + 0.8)], 0.25, 1, "USB_DP"))
    s.append(via(ids, mcu[0], mcu[1], 0.45, 0.12, "GND"))
    s.append(via(ids, rf[0], rf[1], 0.45, 0.12, "GND"))
    s.append(text(ids, 10.6, 0.35, "GP2040-WF RX  22x12  CH32X035+XN297L 3x3  不要RP2040", 0.5, 3))
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
    s.append(sch_text(ids, 40, 28, "GP2040-WF Pico19  原理框图  (嘉立创EDA标准版)", "16pt"))
    s.append(sch_text(ids, 40, 48, "投板前：换成嘉立创库 USB-C / USB-A / RP2040 / CH32X035 / XN297L 封装，铺铜，跑 DRC。", "9pt"))

    s.append(sch_rect(ids, 300, 80, 320, 520))
    s.append(sch_text(ids, 318, 100, "U1 RP2040 QFN-56", "12pt"))
    s.append(sch_text(ids, 318, 118, "GP2040-CE + WfFrame + XN297L", "8pt"))

    lines = [
        "GP0  OLED SDA",
        "GP1  OLED SCL",
        "GP2  USB AUTH D-   ORDER=1",
        "GP3  USB AUTH D+   DPLUS=3",
        "GP4  XN297L CSN",
        "GP5  B4",
        "GP6  R1",
        "GP7  L1",
        "GP8  L2",
        "GP9  R2",
        "GP10 B2",
        "GP11 XN297L SCK",
        "GP12 B3",
        "GP13 R3",
        "GP14 B1",
        "GP15 UP",
        "GP16 XN297L MOSI",
        "GP17 S2 START",
        "GP18 RIGHT",
        "GP19 DOWN",
        "GP20 LEFT",
        "GP21 L3",
        "GP22 A1",
        "GP23 TURBO",
        "GP24 Jerry UART1 TX  (FN已取消)",
        "GP25 XN297L CE",
        "GP26 A2 TOUCH",
        "GP27 S1 SELECT",
        "GP28 WS2812  灯序只跟线",
        "GP29 VBAT ADC 100k/100k",
        "USB_DP/DM 芯片脚 47/46 设备口",
        "接收器 22x12 USB-A  CH32X035+XN297L 都是 3x3",
    ]
    for i, line in enumerate(lines):
        s.append(sch_text(ids, 318, 140 + i * 13, line, "8pt"))

    left = [
        (40, 80, 220, 70, "J1 杰里 AC632N UART", "3V3  GND  TX=GP24  RX NC"),
        (40, 170, 220, 70, "U4 XN297L QFN20 3x3", "CE25 CSN4 SCK11 MOSI16  Si24R1 4x4 电兼容"),
        (40, 260, 220, 70, "J2 OLED 1.3 SSD1306", "SDA GP0  SCL GP1  右上角 87%B"),
        (40, 350, 220, 70, "J3 锂电池 VBAT", "禁止 4.2V 直灌 GP29  100k/100k"),
        (40, 440, 220, 70, "D1 WS2812B GP28", "灯序跟线  末颗电量灯"),
        (40, 530, 220, 70, "按键 对 GND", "无 FN。A2=TOUCH S2=START S1=SELECT"),
    ]
    for x, y, bw, bh, title, sub in left:
        s.append(sch_rect(ids, x, y, bw, bh))
        s.append(sch_text(ids, x + 10, y + 22, title, "10pt"))
        s.append(sch_text(ids, x + 10, y + 44, sub, "8pt"))
        s.append(sch_wire(ids, x + bw, y + bh / 2, 300, y + bh / 2))

    right = [
        (660, 80, 240, 80, "JUSB1 USB-C DEVICE", "D+ GP47  D- GP46  VBUS→AMS1117"),
        (660, 180, 240, 80, "JUSB2 USB-C AUTH", "D+ GP3  D- GP2  插 NXP7105"),
        (660, 280, 240, 70, "U3 AMS1117-3.3", "VIN←VBUS  3V3→MCU/Flash/射频"),
        (660, 370, 240, 70, "U2 W25Q16JV SOIC-8", "QSPI Flash"),
        (660, 460, 240, 70, "Y1 12MHz 3225", "XIN/XOUT + 15pF"),
        (660, 550, 240, 70, "接收器 USB-A 22x12", "CH32X035F8U6 + XN297L  都是 3x3"),
    ]
    for x, y, bw, bh, title, sub in right:
        s.append(sch_rect(ids, x, y, bw, bh))
        s.append(sch_text(ids, x + 10, y + 22, title, "10pt"))
        s.append(sch_text(ids, x + 10, y + 44, sub, "8pt"))
        s.append(sch_wire(ids, 620, y + bh / 2, x, y + bh / 2))

    s.append(sch_text(ids, 40, 640, "Xbox/PS 走手柄 USB 认证口；无线只报 Switch。PS5：JUSB2 插带 NXP7105 的授权口。FORCE_NXP7105_AUTH=1。", "9pt"))
    s.append(sch_text(ids, 40, 662, "GPIO 对照 configs/Pico19/BoardConfig.h。RP2040 引脚号数据手册表 615–621。", "9pt"))
    return s


def write_bom(path: Path) -> None:
    rows = [
        "位号,型号,封装,数量,板,备注",
        "U1,RP2040,QFN-56 7x7 P0.4,1,手柄,主控",
        "U2,W25Q16JV,SOIC-8,1,手柄,QSPI Flash",
        "U3,AMS1117-3.3,SOT-223,1,手柄,5V→3V3",
        "U4,XN297L,QFN-20 3x3 P0.4,1,手柄,CE=GP25 CSN=GP4 SCK=GP11 MOSI=GP16  Si24R1 4x4 电兼容需换封装",
        "U5,AC632N / AC6321A 模块,模块,1,手柄,UART RX ← GP24  不要烧 AC6956A",
        "Y1,12MHz 18pF,3225,1,手柄,RP2040",
        "Y2,16MHz 9pF,3225,1,手柄,XN297L 晶振",
        "JUSB1,USB-C 16P,USB-C,1,手柄,设备口 D+/D− = RP2040 脚 47/46",
        "JUSB2,USB-C 16P,USB-C,1,手柄,认证口 D+=GP3 D−=GP2  插 NXP7105",
        "J1,4P 2.54,排针,1,手柄,杰里 GND 3V3 GP24 NC",
        "J2,4P 2.54,排针,1,手柄,OLED GND 3V3 SDA=GP0 SCL=GP1",
        "J3,3P 2.54,排针,1,手柄,VBAT ADC GP29 必须经 100k/100k",
        "RdivH/RdivL,100k,0603,2,手柄,VBAT 分压到 GP29",
        "RusbP/RusbN,27Ω,0603,2,手柄,USB 串联",
        "Cdec,100nF,0603,12,手柄,",
        "Cbulk,10uF 16V,0603,4,手柄,",
        "Cx,15pF,0603,2,手柄,晶振负载",
        "D1–D16,WS2812B,5050,16,手柄,GP28 走线顺序；第 16 颗电量灯",
        "SWxx,6x6 轻触,插件,21,手柄,18 键+TURBO+BOOT+RESET  无 FN",
        "OLED,SSD1306 1.3 128x64,I2C 模块,1,手柄,右上角 电量%+L/B/G",
        "U1,CH32X035F8U6,QFN-20 3x3 P0.4,1,接收器,USB MCU  D-=PC16 D+=PC17  WCHISP",
        "U2,XN297L,QFN-20 3x3 P0.4,1,接收器,2.4G  CE=PA0 CSN=PA1 SCK=PA2 MOSI=PA3 MISO=PA4",
        "U3,ME6206/XC6206 3.3V,SOT-23,1,接收器,VBUS→3V3  不要 AMS1117",
        "Y1,16MHz 9pF,3225,1,接收器,XN297L 晶振  CH32 用内部 48MHz HSI",
        "JUSB,USB-A 公头,金手指,1,接收器,键盘接收器外形 22×12 mm  枚举 Switch Pro",
        "Risp,4.7k,0603,1,接收器,上电短接 ISP 焊盘把 D+ 拉到 3V3 进 WCHISP",
        "Cdec,100nF,0603,4,接收器,",
        "Cbulk,10uF,0603,2,接收器,",
    ]
    path.write_text("\n".join(rows) + "\n", encoding="utf-8")
    print(f"wrote {path.name}")


def main() -> None:
    OUT.mkdir(parents=True, exist_ok=True)
    w, h, shapes = build_stick_pcb()
    dump_pcb(OUT / "GP2040-WF-stick.json", "GP2040-WF Pico19 stick", w, h, shapes)
    w, h, shapes = build_receiver_pcb()
    dump_pcb(OUT / "GP2040-WF-receiver.json", "GP2040-WF receiver 22x12 CH32+XN297L", w, h, shapes)
    dump_sch(OUT / "GP2040-WF-schematic.json", "GP2040-WF Pico19 schematic", build_schematic())
    write_bom(OUT / "BOM.csv")

    for name in ("GP2040-WF-stick.json", "GP2040-WF-receiver.json", "GP2040-WF-schematic.json"):
        raw = (OUT / name).read_text(encoding="utf-8")
        doc = json.loads(raw)
        assert raw.lstrip()[0] == "{"
        assert doc["head"]["docType"] in ("1", "3")
        assert isinstance(doc["shape"], list) and doc["shape"]

    qfn_n = sum(1 for k in QFN_PADS if str(k).isdigit())
    print(f"QFN numbered pads: {qfn_n}  SOIC pads: {len(SOIC_PADS)}  buttons: {len(BTN_GPIO)}")


if __name__ == "__main__":
    main()
