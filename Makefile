NAME		:= myos
BIN			:= $(NAME).bin
CC			:= i686-elf-gcc
LD			:= i686-elf-ld -T
CFLAGS		:= -ffreestanding -nostdlib -Wall -Wextra
AS			:= i686-elf-as
LINKER		:= linker.ld
INCLUDES	:= includes

SRCS_DIR	:= srcs/
SRCS		:= kernel.c terminal.c utils.c vga.c printf.c keyboard.c
OBJS_DIR	:= objs/
OBJSNAME	:= $(SRCS:.c=.o)
OBJS		:= $(SRCS:%.c=$(OBJS_DIR)%.o)

$(OBJS_DIR)%.o : $(SRCS_DIR)%.c
	@mkdir -p objs
	$(CC) $(CFLAGS) -c $< -o $@

all			: $(BIN)

$(BIN)		: boot.o $(OBJS)
	$(LD) $(LINKER) -o $@ $^
	mkdir -p isodir/boot/grub
	cp -i grub.cfg isodir/boot/grub
	cp -i $@ isodir/boot/	

boot.o		: boot.s
	$(AS) $^ -o $@

$(NAME).iso : $(BIN)
	@../build/grub-mkrescue -o $@ isodir

.PHONY		: clean
clean		:
	$(RM) $(BIN) boot.o $(OBJS)
	$(RM) -rf isodir objs
