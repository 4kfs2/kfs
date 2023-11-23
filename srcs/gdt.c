#include "gdt.h"
#include "utils.h"

struct gdtdesc 	kgdt[GDTSIZE];
struct gdtr		kgdtr;

void init_gdt_desc(uint32_t base, uint32_t limite, uint8_t acces, uint8_t other, struct gdtdesc *desc)
{
	desc->lim0_15 = (limite & 0xffff);
	desc->base0_15 = (base & 0xffff);
	desc->base16_23 = (base & 0xff0000) >> 16;
	desc->acces = acces;
	desc->lim16_19 = (limite & 0xf0000) >> 16;
	desc->other = (other & 0xf);
	desc->base24_31 = (base & 0xff000000) >> 24;
	return;
}

void init_gdt(void)
{
	init_gdt_desc(0x0, 0x0, 0x0, 0x0, &kgdt[0]);
	init_gdt_desc(0x0, 0xFFFFF, 0x9B, 0x0D, &kgdt[1]); //kernel_code
	init_gdt_desc(0x0, 0xFFFFF, 0x93, 0x0D, &kgdt[2]); // kernel_data
	init_gdt_desc(0x0, 0x0, 0x97, 0x0D, &kgdt[3]); // kernel_stack

	init_gdt_desc(0x0, 0xFFFFF, 0xFF, 0x0D, &kgdt[4]); //u_code
	init_gdt_desc(0x0, 0xFFFFF, 0xF3, 0x0D, &kgdt[5]); //u_data
	init_gdt_desc(0x0, 0x0, 0xF7, 0x0D, &kgdt[6]); //u_stack

	 kgdtr.limite = GDTSIZE * 8;
	 kgdtr.base = GDTBASE;

	 memcpy((char *) kgdtr.base, (char *) kgdt, kgdtr.limite);

	 asm("lgdtl (kgdtr)");

	 asm("	movw $0x10, %ax	 \n \
			movw %ax, %ds	 \n \
			movw %ax, %es	 \n \
			movw %ax, %ss	 \n \
			ljmp $0x08, $next	 \n \
			next:		  \n");
}
