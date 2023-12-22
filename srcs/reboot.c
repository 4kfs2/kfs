#include "../includes/utils.h"
#include "../includes/keyboard.h"
#include "../includes/x86.h"

void reboot()
{
    uint8_t good = 0x02;
    while (good & 0x02)
        good = inb(KEYBOARD_COMMAND_PORT);
    outb(KEYBOARD_COMMAND_PORT, KBRD_RESET);
	asm volatile ("hlt");
}
