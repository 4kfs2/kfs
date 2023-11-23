NAME		:= myos
BIN			:= $(NAME).bin
CC			:= i686-elf-gcc
LD			:= i686-elf-ld -T
CFLAGS		:= -ffreestanding -Wall -Wextra -g
AS			:= i686-elf-as
LINKER		:= linker.ld
INCLUDES	:= includes
SRCS_DIR	:= srcs/
ASMSRCS		:= boot.s
SRCS		:= kernel.c terminal.c utils.c vga.c printf.c keyboard.c debug.c elf.c reboot.c gdt.c
OBJS_DIR	:= objs/
OBJSNAME	:= $(SRCS:.c=.o)
OBJS		:= $(SRCS:%.c=$(OBJS_DIR)%.o)

$(OBJS_DIR)%.o : $(SRCS_DIR)%.c
	@mkdir -p objs
	$(CC) $(CFLAGS) -c $< -o $@ -I $(INCLUDES)

all			: $(BIN)

boot.o			: $(ASMSRCS)
	$(AS) $^ -o $@

$(BIN)		: boot.o $(OBJS)
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