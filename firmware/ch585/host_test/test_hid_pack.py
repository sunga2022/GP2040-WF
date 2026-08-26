#!/usr/bin/env python3
"""Host-side check for the CH585 HID report packing rules."""

import struct
import unittest


DPAD_UP = 1 << 0
DPAD_DOWN = 1 << 1
DPAD_LEFT = 1 << 2
DPAD_RIGHT = 1 << 3


def hat_from_dpad(dpad: int) -> int:
    u = bool(dpad & DPAD_UP)
    d = bool(dpad & DPAD_DOWN)
    l = bool(dpad & DPAD_LEFT)
    r = bool(dpad & DPAD_RIGHT)
    if u and r:
        return 1
    if r and d:
        return 3
    if d and l:
        return 5
    if l and u:
        return 7
    if u:
        return 0
    if r:
        return 2
    if d:
        return 4
    if l:
        return 6
    return 8


def pack_hid(dpad: int, buttons: int) -> bytes:
    return bytes(
        [
            buttons & 0xFF,
            (buttons >> 8) & 0xFF,
            hat_from_dpad(dpad) & 0x0F,
            0x80,
            0x80,
            0x80,
            0x80,
            0,
        ]
    )


class HidPackTest(unittest.TestCase):
    def test_idle(self):
        report = pack_hid(0, 0)
        self.assertEqual(len(report), 8)
        self.assertEqual(report[2], 8)
        self.assertEqual(report[0:2], b"\x00\x00")

    def test_up_and_punch(self):
        report = pack_hid(DPAD_UP, 1)
        self.assertEqual(report[2], 0)
        self.assertEqual(report[0], 1)

    def test_diagonal(self):
        self.assertEqual(hat_from_dpad(DPAD_UP | DPAD_RIGHT), 1)
        self.assertEqual(hat_from_dpad(DPAD_DOWN | DPAD_LEFT), 5)

    def test_struct_size(self):
        self.assertEqual(len(pack_hid(0, 0x3FFF)), 8)
        buttons = struct.unpack_from("<H", pack_hid(0, 0x3FFF), 0)[0]
        self.assertEqual(buttons, 0x3FFF)


if __name__ == "__main__":
    unittest.main()
