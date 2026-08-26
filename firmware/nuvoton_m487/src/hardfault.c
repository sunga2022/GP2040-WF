#include <stdint.h>

uint32_t ProcessHardFault(uint32_t lr, uint32_t msp, uint32_t psp)
{
    (void)lr;
    (void)msp;
    (void)psp;
    while (1) {
    }
}
