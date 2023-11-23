#include "gdt.h"
#include "utils.h"

struct gdtdesc 	kgdt[GDTSIZE];
struct gdtr		kgdtr;

void init_gdt_desc(uint32_t base, uint32_t limit, uint8_t acces, uint8_t flag, struct gdtdesc *desc)
{
	desc->lim0_15 = (limit & 0xffff);
	desc->base0_15 = (base & 0xffff);
	desc->base16_23 = (base & 0xff0000) >> 16;
	desc->acces = acces; // P(present 1? valid seg) DPL(privilege level 0/3) S(type 1? code/data) E(1? code) DC RW A
	desc->lim16_19 = (limit & 0xf0000) >> 16;
	desc->flag = (flag & 0xf); // G(Granularity 1? 4KB) DB(size 1? 32bit) L(1? 64bit) AVL(reserved)
	desc->base24_31 = (base & 0xff000000) >> 24;
	return;
}

void init_gdt(void)
{
	init_gdt_desc(0x0, 0x0, 0x0, 0x0, &kgdt[0]);
	init_gdt_desc(0x0, 0xFFFFF, 0x9A, 0x0C, &kgdt[1]); //kernel_code
	init_gdt_desc(0x0, 0xFFFFF, 0x92, 0x0C, &kgdt[2]); // kernel_data
	init_gdt_desc(0x0, 0x0, 0x97, 0x0C, &kgdt[3]); // kernel_stack

	init_gdt_desc(0x0, 0xFFFFF, 0xFA, 0x0C, &kgdt[4]); //u_code
	init_gdt_desc(0x0, 0xFFFFF, 0xF2, 0x0C, &kgdt[5]); //u_data
	init_gdt_desc(0x0, 0x0, 0xF7, 0x0C, &kgdt[6]); //u_stack

	 kgdtr.limit = GDTSIZE * 8;
	 kgdtr.base = GDTBASE;

	 memcpy((char *) kgdtr.base, (char *) kgdt, kgdtr.limit);

	 asm("lgdtl (kgdtr)");

	 asm("	movw $0x10, %ax	 \n \
			movw %ax, %ds	 \n \
			movw %ax, %es	 \n \
			movw %ax, %ss	 \n \
			ljmp $0x08, $next	 \n \
			next:		  \n");
}
