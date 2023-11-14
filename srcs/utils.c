#include "../includes/utils.h"

size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

void* memcpy( void* d, const void* s, size_t n )
{
	char* dest;
	char* src;

	dest = (char*)d;
	src = (char*)s;

	while (n--) {
		*dest++ = *src++;
	}

	return d;
}