.set ARCH,	0
.set ALGN,	1<<0
.set MEM,	1<<1
.set MAGIC,	0xE85250D6
.set FLAGS,	ALGN | MEM

.section .multiboot
.align	8
.long	MAGIC
.long 	ARCH
.long 	multiboot_end - .multiboot
.long 	-(MAGIC + ARCH + (multiboot_end - .multiboot))
.short	0
.short	0
.long	8
multiboot_end:

.section .bss
.align 16
stack_bottom:
.skip 0x4000
stack_top:

.section .text
.global _start42
.type _start42, @function
_start42:
	movl $stack_top, %esp
	pushl	$0
	popf
	pushl   %ebx
	call kernel_main
1010:	hlt
	jmp 1010b
