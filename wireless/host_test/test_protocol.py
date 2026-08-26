#!/usr/bin/env python3
"""Pack/unpack GP2040-WF UART frames. Run: python3 wireless/host_test/test_protocol.py"""
import struct
import sys

MAGIC0, MAGIC1 = 0x57, 0x46
FRAME_SIZE = 20
FMT = "<BBBBLHHHHBBB"  # 19 bytes without crc


def crc8(data: bytes) -> int:
    crc = 0
    for byte in data:
        crc ^= byte
        for _ in range(8):
            crc = ((crc << 1) ^ 0x07) & 0xFF if crc & 0x80 else (crc << 1) & 0xFF
    return crc


def pack(seq=1, dpad=1, buttons=0x0001, lx=0x7FFF, ly=0x7FFF, rx=0x7FFF, ry=0x7FFF, lt=0, rt=0, flags=0) -> bytes:
    body = struct.pack(FMT, MAGIC0, MAGIC1, seq, dpad, buttons, lx, ly, rx, ry, lt, rt, flags)
    assert len(body) == FRAME_SIZE - 1
    return body + bytes([crc8(body)])


def unpack(frame: bytes) -> dict:
    if len(frame) != FRAME_SIZE:
        raise ValueError("bad length")
    if crc8(frame[:-1]) != frame[-1]:
        raise ValueError("bad crc")
    magic0, magic1, seq, dpad, buttons, lx, ly, rx, ry, lt, rt, flags = struct.unpack(FMT, frame[:-1])
    if (magic0, magic1) != (MAGIC0, MAGIC1):
        raise ValueError("bad magic")
    return {
        "seq": seq,
        "dpad": dpad,
        "buttons": buttons,
        "lx": lx,
        "ly": ly,
        "rx": rx,
        "ry": ry,
        "lt": lt,
        "rt": rt,
        "flags": flags,
    }


def main() -> int:
    frame = pack()
    assert len(frame) == FRAME_SIZE
    parsed = unpack(frame)
    assert parsed["dpad"] == 1
    assert parsed["buttons"] == 1
    bad = bytearray(frame)
    bad[-1] ^= 0xFF
    try:
        unpack(bytes(bad))
    except ValueError:
        pass
    else:
        raise SystemExit("crc should fail")
    print(f"ok {FRAME_SIZE} bytes  baud=1000000  rate=1000 Hz")
    return 0


if __name__ == "__main__":
    sys.exit(main())
