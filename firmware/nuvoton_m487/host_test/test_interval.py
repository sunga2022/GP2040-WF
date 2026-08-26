#!/usr/bin/env python3
"""Fail the build if the HS HID interval is not 1 (8 kHz)."""

import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


class IntervalTest(unittest.TestCase):
    def test_hs_binterval_is_one(self):
        header = (ROOT / "include" / "hid_usb.h").read_text()
        match = re.search(r"#define\s+HID_DEFAULT_INT_IN_INTERVAL\s+(\d+)", header)
        self.assertIsNotNone(match)
        self.assertEqual(int(match.group(1)), 1)

    def test_report_desc_length(self):
        header = (ROOT / "include" / "hid_usb.h").read_text()
        match = re.search(r"#define\s+HID_REPORT_DESC_LEN\s+(\d+)", header)
        self.assertEqual(int(match.group(1)), 71)

    def test_not_mouse_sample_interval(self):
        header = (ROOT / "include" / "hid_usb.h").read_text()
        self.assertNotIn("HID_DEFAULT_INT_IN_INTERVAL     10", header)


if __name__ == "__main__":
    unittest.main()
