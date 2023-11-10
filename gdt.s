GDT_start:
	GDT_null:
		.long 0x0
		.long 0x0

	kernel_code:
		.word 0xffff
		.word 0x0
		.byte 0x0
		.byte 0b10011010
		.byte 0b11001111
		.byte 0x0

	kernel_data:
		.word 0xffff
		.word 0x0
		.byte 0x0
		.byte 0b10010010
		.byte 0b11001111
		.byte 0x0

	user_code:
		.word 0xffff
		.word 0x0
		.byte 0x0
		.byte 0b11111010
		.byte 0b11001111
		.byte 0x0

	user_data:
		.word 0xffff
		.word 0x0
		.byte 0x0
		.byte 0b11110010
		.byte 0b11001111
		.byte 0x0
GDT_end:

GDT_descriptor:
	.word GDT_end - GDT_start - 1
	.long GDT_start
	