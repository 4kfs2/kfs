.set ARCH,	0
.set MAGIC,	0xE85250D6

.section .multiboot, "a"
.align	8
.long	MAGIC
.long 	ARCH
.long 	multiboot_end - .multiboot
.long 	-(MAGIC + ARCH + (multiboot_end - .multiboot))
.short	0
.short	0
.long	8
multiboot_end:

.section .multiboot.text, "a"
.global _start42
.type _start42, @function
_start42:
	movl $(boot_page_table1 - 0xC0000000), %edi
	movl $0, %esi
	movl $1024, %ecx

1:
	cmpl $_start, %esi
	jl 2f
	cmpl $_mapping_size, %esi
	jge 3f

	movl %esi, %edx
	orl $0x003, %edx
	movl %edx, (%edi)

2:
	addl $4096, %esi
	addl $4, %edi
	loop 1b

3:
	movl $(boot_page_table1 - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + 0 // identity mapping
	movl $(boot_page_table1 - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + 0x300 * 4
	movl $(boot_page_table1), boot_page_directory - 0xC0000000 + 0x1000 + 0x300 * 4 // pgdir->tables[0x300] = boot_page_table1

	movl $(boot_page_directory - 0xC0000000), %ecx
	movl %ecx, %cr3

	movl %cr0, %ecx
	orl $0x80000000, %ecx
	movl %ecx, %cr0 // cr0에 로드된 후, 페이징기능 켜짐. 즉, 아래 줄 부터 가상주소로 읽음.

	lea half, %ecx // 해당 부분을 실행하기 위해서 identity mapping 부분이 필요
	jmp *%ecx // 가상주소를 0xC0000000 위치로 이동

.section .bootstrap_stack, "a"
.align 16
stack_bottom:
.skip 0x4000
stack_top:

.global boot_page_directory, boot_page_table1
.section .bss, "a"
	.align 4096
boot_page_directory:
	.skip 0x2000
boot_page_table1:
	.skip 4096

.section .text
half:
	movl $0, boot_page_directory + 0 // identity mapping한 엔트리 필요 x

	# Reload crc3 to force a TLB flush so the changes to take effect.
	movl %cr3, %ecx
	movl %ecx, %cr3

	movl $stack_top, %esp
	pushl	$0
	popf
	pushl   %ebx
	call kernel_main
	