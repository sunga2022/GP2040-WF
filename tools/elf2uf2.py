#!/usr/bin/env python3
"""Convert RP2040 ELF to UF2. Family 0xe48bff56.

RP2040 BOOTSEL only accepts 256-byte-aligned flash pages. ELF .data LMA
is often unaligned; this packs bytes into aligned 256-byte pages.
"""
from __future__ import annotations

import struct
import sys
from pathlib import Path

UF2_MAGIC_START0 = 0x0A324655
UF2_MAGIC_START1 = 0x9E5D5157
UF2_MAGIC_END = 0x0AB16F30
FAMILY_RP2040 = 0xE48BFF56
FLAG_FAMILYID = 0x00002000
PAGE = 256
FLASH_START = 0x10000000
FLASH_END = 0x10200000  # 2 MiB, Pico / W25Q16
PT_LOAD = 1


def load_flash_pages(path: Path) -> dict[int, bytearray]:
    data = path.read_bytes()
    if data[:4] != b"\x7fELF":
        raise SystemExit(f"not ELF: {path}")
    if data[4] != 1:
        raise SystemExit("need 32-bit ELF")
    e_phoff = struct.unpack_from("<I", data, 28)[0]
    e_phentsize = struct.unpack_from("<H", data, 42)[0]
    e_phnum = struct.unpack_from("<H", data, 44)[0]
    pages: dict[int, bytearray] = {}
    for i in range(e_phnum):
        off = e_phoff + i * e_phentsize
        p_type, p_offset, _p_vaddr, p_paddr, p_filesz, _p_memsz, _p_flags, _p_align = struct.unpack_from(
            "<IIIIIIII", data, off
        )
        if p_type != PT_LOAD or p_filesz == 0:
            continue
        if not (FLASH_START <= p_paddr < FLASH_END):
            continue
        chunk = data[p_offset : p_offset + p_filesz]
        for n, byte in enumerate(chunk):
            addr = p_paddr + n
            if addr >= FLASH_END:
                raise SystemExit(f"flash overflow at {addr:#x}")
            page = addr & ~(PAGE - 1)
            slot = pages.get(page)
            if slot is None:
                slot = bytearray(PAGE)
                pages[page] = slot
            slot[addr - page] = byte
    if not pages:
        raise SystemExit("no flash PT_LOAD segments")
    return pages


def to_uf2(pages: dict[int, bytearray]) -> bytes:
    addrs = sorted(pages)
    n = len(addrs)
    out = bytearray()
    for i, addr in enumerate(addrs):
        if addr % PAGE:
            raise SystemExit(f"unaligned page {addr:#x}")
        chunk = bytes(pages[addr])
        hd = struct.pack(
            "<IIIIIIII",
            UF2_MAGIC_START0,
            UF2_MAGIC_START1,
            FLAG_FAMILYID,
            addr,
            PAGE,
            i,
            n,
            FAMILY_RP2040,
        )
        block = hd + chunk + b"\x00" * (476 - PAGE) + struct.pack("<I", UF2_MAGIC_END)
        if len(block) != 512:
            raise SystemExit(f"bad block {len(block)}")
        out += block
    return bytes(out)


def main() -> None:
    if len(sys.argv) != 3:
        print("usage: elf2uf2.py in.elf out.uf2")
        sys.exit(2)
    inp, outp = Path(sys.argv[1]), Path(sys.argv[2])
    uf2 = to_uf2(load_flash_pages(inp))
    outp.write_bytes(uf2)
    print(f"wrote {outp}  {len(uf2)//512} blocks  {len(uf2)} bytes")


if __name__ == "__main__":
    main()
