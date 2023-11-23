
#ifndef __GDT_H__
#define __GDT_H__

#include <stdint.h>

#define GDTBASE    0x00000800
#define GDTSIZE    10

struct gdtr {
    uint16_t limit;
    uint32_t base;
} __attribute__ ((packed));

struct gdtdesc {
    uint16_t lim0_15;
    uint16_t base0_15;
    uint8_t base16_23;
    uint8_t acces;
    uint8_t lim16_19:4;
    uint8_t other:4;
    uint8_t base24_31;
} __attribute__ ((packed));

void init_gdt(void);


#endif