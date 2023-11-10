.set KERNEL_CODE_SEG, kernel_code - GDT_start
.set KERNEL_DATA_SEG, kernel_data - GDT_start
.set ARCH,	0
.set MAGIC,	0xE85250D6

.global _start42

multiboot:
.align	8
.long	MAGIC
.long 	ARCH
.long 	multiboot_end - multiboot
.long 	-(MAGIC + ARCH + (multiboot_end - multiboot))
.short	0
.short	0
.long	8
multiboot_end:

stack_bottom:
.skip 0x4000
stack_top:

_start42:
	cli
	lgdtl GDT_descriptor
	movl %cr0, %eax
	or 1, %al
	movl %eax, %cr0
	jmp $KERNEL_CODE_SEG, $protect_mode

protect_mode:
	mov $stack_top, %esp
	call kernel_main
	jmp .
