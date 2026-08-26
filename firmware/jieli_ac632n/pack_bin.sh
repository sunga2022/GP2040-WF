#!/usr/bin/env bash
# Turn sdk.elf into app.bin / jl_isd.bin / jl_isd.ufw (no USB device required).
set -euo pipefail
TOOLS="${1:?usage: pack_bin.sh /path/to/cpu/bd19/tools}"
cd "$TOOLS"
export PATH="/opt/jieli/q32s/bin:/opt/jieli/common/bin:/opt/jieli/postbuild:$TOOLS:$PATH"
OBJCOPY="${OBJCOPY:-/opt/jieli/q32s/bin/objcopy}"

"$OBJCOPY" -O binary -j .text sdk.elf text.bin
"$OBJCOPY" -O binary -j .data sdk.elf data.bin
"$OBJCOPY" -O binary -j .data_code sdk.elf data_code.bin
"$OBJCOPY" -O binary -j .overlay_aec sdk.elf aec.bin || : > aec.bin
"$OBJCOPY" -O binary -j .overlay_aac sdk.elf aac.bin || : > aac.bin
"$OBJCOPY" -O binary -j .overlay_aptx sdk.elf aptx.bin || : > aptx.bin
"$OBJCOPY" -O binary -j .common sdk.elf common.bin || : > common.bin
bank_files=""
for i in $(seq 0 20); do
  "$OBJCOPY" -O binary -j ".overlay_bank$i" sdk.elf "bank$i.bin" 2>/dev/null || true
  if [ ! -s "bank$i.bin" ]; then
    break
  fi
  bank_files="$bank_files bank$i.bin 0x0 "
done
chmod +x lz4_packet 2>/dev/null || true
./lz4_packet -dict text.bin -input common.bin 0 $bank_files -o bank.bin
cat text.bin data.bin data_code.bin aec.bin aac.bin bank.bin aptx.bin > app.bin

if command -v xvfb-run >/dev/null; then
  xvfb-run -a isd_download isd_config.ini -tonorflash -dev bd19 -boot 0x2000 -div8 -wait 1 \
    -uboot uboot.boot -app app.bin cfg_tool.bin -res p11_code.bin -uboot_compress -flash-params flash_params.bin \
    || true
else
  isd_download isd_config.ini -tonorflash -dev bd19 -boot 0x2000 -div8 -wait 1 \
    -uboot uboot.boot -app app.bin cfg_tool.bin -res p11_code.bin -uboot_compress -flash-params flash_params.bin \
    || true
fi
fw_add -noenc -fw jl_isd.fw -add ota.bin -type 100 -out jl_isd.fw || true
fw_add -noenc -fw jl_isd.fw -add script.ver -out jl_isd.fw || true
ufw_maker -fw_to_ufw jl_isd.fw
ls -lh app.bin jl_isd.bin jl_isd.fw jl_isd.ufw
