.set ARCH,	0
.set MAGIC,	0xE85250D6

.section .multiboot.data, "a"
.align	8
.long	MAGIC
.long 	ARCH
.long 	multiboot_end - .multiboot.data
.long 	-(MAGIC + ARCH + (multiboot_end - .multiboot.data))
.short	0
.short	0
.long	8
multiboot_end:

.section .bootstrap_stack, "a", @nobits
stack_bottom:
.skip 16384 
stack_top:

.section .bss, "aw", @nobits
	.align 4096
boot_page_directory:
	.skip 4096
boot_page_table1:
	.skip 4096

.section .multiboot.text, "a"
.global _start42
.type _start42, @function
_start42:
	movl $(boot_page_table1 - 0x80000000), %edi
	movl $0, %esi
	movl $1023, %ecx

1:
	cmpl $_kernel_start, %esi
	jl 2f
	cmpl $(_kernel_end - 0x80000000), %esi
	jge 3f

	movl %esi, %edx
	orl $0x003, %edx
	movl %edx, (%edi)

2:
	addl $4096, %esi
	addl $4, %edi
	loop 1b

3:
	movl $(0x000B8000 | 0x003), boot_page_table1 - 0x80000000 + 0 * 4

	movl $(boot_page_table1 - 0x80000000 + 0x003), boot_page_directory - 0x80000000 + 0
	movl $(boot_page_table1 - 0x80000000 + 0x003), boot_page_directory - 0x80000000 + 0x200 * 4

	movl $(boot_page_directory - 0x80000000), %ecx
	movl %ecx, %cr3

	movl %cr0, %ecx
	orl $0x80000000, %ecx
	movl %ecx, %cr0

	lea 4f, %ecx
	jmp *%ecx

.section .text

4:
	movl %cr3, %ecx
	movl %ecx, %cr3

	mov $stack_top, %esp
	pushl	$0
	popf
	pushl   %ebx

	call kernel_main

	cli
1:	hlt
	jmp 1b

