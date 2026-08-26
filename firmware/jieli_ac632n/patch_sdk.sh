#!/usr/bin/env bash
# Patch official fw-AC63_BT_SDK HID project with GP2040-WF Switch Pro UART glue.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
SDK="${1:?usage: patch_sdk.sh /path/to/fw-AC63_BT_SDK}"

HID="$SDK/apps/hid"
BOARD="$HID/board/bd19"
DST="$HID/gp2040wf"
mkdir -p "$DST"

cp -f "$ROOT/firmware/jieli_ac632n/gp2040wf_uart_hid.c" "$DST/"
cp -f "$ROOT/firmware/jieli_ac632n/gp2040wf_uart_hid.h" "$DST/"
cp -f "$ROOT/firmware/jieli_ac632n/sdk_glue.c" "$DST/"
cp -f "$ROOT/wireless/switch_pro.c" "$DST/"
cp -f "$ROOT/wireless/switch_pro.h" "$DST/"
cp -f "$ROOT/wireless/switch_pro_desc.h" "$DST/"
cp -f "$ROOT/wireless/protocol.h" "$DST/"
cp -f "$ROOT/wireless/uart_rx.h" "$DST/"
cp -f "$ROOT/wireless/hid_pack.h" "$DST/"

python3 - "$SDK" << 'PY'
import pathlib, sys
sdk = pathlib.Path(sys.argv[1])
mk = sdk / "apps/hid/board/bd19/Makefile"
text = mk.read_text(encoding="utf-8", errors="replace")
if "gp2040wf/sdk_glue.c" not in text:
    needle = "../../../../apps/hid/examples/keyboard/app_keyboard.c \\"
    insert = needle + """
	../../../../apps/hid/gp2040wf/sdk_glue.c \\
	../../../../apps/hid/gp2040wf/gp2040wf_uart_hid.c \\
	../../../../apps/hid/gp2040wf/switch_pro.c \\"""
    if needle not in text:
        raise SystemExit("Makefile: keyboard source line not found")
    text = text.replace(needle, insert, 1)
if "-DJL_AC63_SDK" not in text:
    text = text.replace(
        "-DUSB_PC_NO_APP_MODE \\",
        "-DUSB_PC_NO_APP_MODE \\\n	-DJL_AC63_SDK \\",
        1,
    )
inc = "-I../../../../apps/hid/gp2040wf \\"
if inc not in text:
    text = text.replace(
        "-I../../../../apps/hid/include \\",
        "-I../../../../apps/hid/include \\\n	" + inc,
        1,
    )
mk.write_text(text, encoding="utf-8")

kb = sdk / "apps/hid/examples/keyboard/app_keyboard.c"
k = kb.read_text(encoding="utf-8", errors="replace")
if "wf_jieli_sdk_start" not in k:
    k = k.replace(
        '#include "app_comm_bt.h"',
        '#include "app_comm_bt.h"\n#include "gp2040wf_uart_hid.h"\nextern void wf_jieli_sdk_start(void);',
        1,
    )
    k = k.replace(
        "    sys_key_event_enable();",
        "    wf_jieli_sdk_start();\n    sys_key_event_enable();",
        1,
    )
if "switch_pro_desc.h" not in k:
    k = k.replace(
        "static const u8 hidkey_report_map[] = {",
        '#include "switch_pro_desc.h"\nstatic const u8 *hidkey_report_map = wf_switch_pro_report_descriptor;\nstatic const u8 hidkey_report_map_unused[] = {',
        1,
    )
    k = k.replace(
        ".report_map = hidkey_report_map,\n    .report_map_size = sizeof(hidkey_report_map),",
        ".report_map = (u8 *)wf_switch_pro_report_descriptor,\n    .report_map_size = sizeof(wf_switch_pro_report_descriptor),",
    )
    k = k.replace("BD_CLASS_KEYBOARD", "BD_CLASS_GAMEPAD")
    k = k.replace("BLE_APPEARANCE_HID_KEYBOARD", "BLE_APPEARANCE_HID_GAMEPAD")
kb.write_text(k, encoding="utf-8")

uc = sdk / "apps/hid/modules/user_cfg.c"
u = uc.read_text(encoding="utf-8", errors="replace")
u = u.replace('.edr_name        = "JL_HID_DEBUG"', '.edr_name        = "Pro Controller"')
uc.write_text(u, encoding="utf-8")

edr = sdk / "apps/hid/modules/bt/app_comm_edr.c"
e = edr.read_text(encoding="utf-8", errors="replace")
if "wf_jieli_edr_output" not in e:
    e = e.replace(
        "user_hid_init(NULL);",
        "extern void wf_jieli_edr_output(u8 *packet, u16 size, u16 channel);\n    user_hid_init(wf_jieli_edr_output);",
        1,
    )
edr.write_text(e, encoding="utf-8")
print("patched", sdk)
PY
echo "patched $SDK"
