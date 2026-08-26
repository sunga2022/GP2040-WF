#!/usr/bin/env python3
"""Convert RP2040 ELF to UF2. Family 0xe48bff56."""
import struct
import sys
from pathlib import Path

UF2_MAGIC_START0 = 0x0A324655
UF2_MAGIC_START1 = 0x9E5D5157
UF2_MAGIC_END = 0x0AB16F30
FAMILY_RP2040 = 0xE48BFF56
FLAG_FAMILYID = 0x00002000
PAGE = 256


def load_elf_segments(path: Path):
    data = path.read_bytes()
    if data[:4] != b"\x7fELF":
        raise SystemExit(f"not ELF: {path}")
    ei_class = data[4]
    if ei_class != 1:
        raise SystemExit("need 32-bit ELF")
    e_phoff, e_phentsize, e_phnum = struct.unpack_from("<IHH", data, 28)[0], struct.unpack_from("<H", data, 42)[0], struct.unpack_from("<H", data, 44)[0]
    # e_phoff at 28, e_phentsize 42, e_phnum 44 for ELF32
    e_phoff = struct.unpack_from("<I", data, 28)[0]
    e_phentsize = struct.unpack_from("<H", data, 42)[0]
    e_phnum = struct.unpack_from("<H", data, 44)[0]
    segs = []
    for i in range(e_phnum):
        off = e_phoff + i * e_phentsize
        p_type, p_offset, p_vaddr, p_paddr, p_filesz, p_memsz, p_flags, p_align = struct.unpack_from(
            "<IIIIIIII", data, off
        )
        if p_type != 1 or p_filesz == 0:
            continue
        chunk = data[p_offset : p_offset + p_filesz]
        segs.append((p_paddr, chunk))
    segs.sort()
    return segs


def to_uf2(segs):
    blocks = []
    for addr, payload in segs:
        pad = (-len(payload)) % PAGE
        payload = payload + b"\x00" * pad
        for i in range(0, len(payload), PAGE):
            blocks.append((addr + i, payload[i : i + PAGE]))
    out = bytearray()
    n = len(blocks)
    for i, (addr, chunk) in enumerate(blocks):
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


def main():
    if len(sys.argv) != 3:
        print("usage: elf2uf2.py in.elf out.uf2")
        sys.exit(2)
    inp, outp = Path(sys.argv[1]), Path(sys.argv[2])
    uf2 = to_uf2(load_elf_segments(inp))
    outp.write_bytes(uf2)
    print(f"wrote {outp}  {len(uf2)//512} blocks  {len(uf2)} bytes")


if __name__ == "__main__":
    main()
