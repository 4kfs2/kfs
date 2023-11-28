.global init_seg

init_seg:
	lgdtl (kgdtr)
	movw $0x10, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %ss
	ljmp $0x08, $next
next:
	ret