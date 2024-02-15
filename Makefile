NAME		:= myos
BIN			:= $(NAME).bin
CC			:= i686-elf-gcc
LD			:= i686-elf-ld -T
CFLAGS		:= -ffreestanding -Wall -Wextra -g
AS			:= i686-elf-as
LINKER		:= linker.ld
INCLUDES	:= includes
SRCS_DIR	:= srcs/
ASMSRCS		:= boot.s init_seg.s
SRCS		:= kernel.c terminal.c utils.c vga.c printf.c keyboard.c debug.c reboot.c gdt.c panic.c init.c mm.c memory.c \
				ordered_array.c kmalloc.c vmalloc.c
OBJS_DIR	:= objs/
OBJSNAME	:= $(SRCS:.c=.o)
OBJS		:= $(SRCS:%.c=$(OBJS_DIR)%.o)
OBJS		+= $(ASMSRCS:%.s=$(OBJS_DIR)%.o)

$(OBJS_DIR)%.o : $(SRCS_DIR)%.c
	@mkdir -p objs
	$(CC) $(CFLAGS) -c $< -o $@ -I $(INCLUDES)

$(OBJS_DIR)%.o : $(SRCS_DIR)asm/%.s
	$(AS) $< -o $@

all			: $(BIN)

$(BIN)		: $(OBJS)
	$(LD) $(LINKER) -o $@ $^ --verbose
	mkdir -p isodir/boot/grub
	cp -i grub.cfg isodir/boot/grub
	cp -i $@ isodir/boot/	

$(NAME).iso : $(BIN)
	@grub-mkrescue -o $@ isodir

clean		:
	$(RM) $(BIN) boot.o $(OBJS)
	$(RM) -rf isodir objs

re			 : clean
	@make $(NAME).iso

.PHONY		: clean re
