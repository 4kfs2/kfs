.text
.global test
test:
	movb $6, (0x3fe001) //first_page_table accessing
	movb $7, (0x7fe002) //second_page_table
	movb $8, (0xbfe003) //third_page_table
	ret