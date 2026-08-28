/* Linker script to configure memory regions. */

SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)

MEMORY
{
  FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 0x40000
  RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 0x8000
  CODE_RAM (rwx) : ORIGIN = 0x800000, LENGTH = 0x8000
}


INCLUDE "nrf_common.ld"

PROVIDE(__tbss_start__ = __tbss_start);
PROVIDE(__tbss_end__ = __tbss_end);
PROVIDE(__sbss_start__ = __sbss_start);
PROVIDE(__sbss_end__ = __sbss_end);
