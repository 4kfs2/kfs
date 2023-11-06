#include "../includes/terminal.h"
#include "../includes/utils.h"

#include <stdarg.h>

static const char chars[16] = "0123456789ABCDEF";

static int print_str(va_list args)
{
	int r = 0;
	char *s = va_arg(args, char*);
	if (s == NULL)
		s = "(NULL)";
	terminal_writestring(s);
	r += strlen(s);
	return r;
}

static int print_hex(va_list args)
{
	int r = 0;
	unsigned int x = va_arg(args, unsigned int);

	terminal_writestring("0x");
	r += 2;
	while (x)
	{
		terminal_putchar(chars[x % 16]);
		x /= 16;
		++r;
	}
	return r;
}

static int print_int(va_list args)
{
	int r = 0;
	int d = va_arg(args, int);

	while (d)
	{
		terminal_putchar(chars[d % 10]);
		d /= 10;
		++r;
	}
	return r;
}

static int print_char(va_list args)
{
	int r = 0;
	int c = va_arg(args, int);
	terminal_putchar(c);
	++r;
	return r;
}

static int print_percent()
{
	int r = 0;
	terminal_putchar('%');
	++r;
	return r;
}

static int vprintf(const char *fmt, va_list args)
{
	int r;

	r = 0;
	while  (*fmt)
	{
		if (*fmt == '%')
		{
			++fmt;
			switch (*fmt)
			{
				case 's':
					r += print_str(args); break;
				case 'd':
					r += print_int(args); break;
				case 'x':
					r += print_hex(args); break;
				case 'c':
					r += print_char(args); break;
				case '%':
					r += print_percent(); break;
				default:
					goto print_c;
			}
		}
		print_c:
			terminal_putchar(*fmt);
			++r;
		++fmt;
	}
	return r;
}

int printf(const char *fmt, ...)
{
	va_list args;
	int r;
	va_start(args, fmt);
	r = vprintf(fmt, args);
	va_end(args);
	return r;
}
