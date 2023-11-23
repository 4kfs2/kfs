#include "../includes/utils.h"

size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

void	*memcpy(void *dst, const void *src, size_t n)
{
	char		*dstptr;

	if (!dst && !src)
		return (NULL);
	dstptr = dst;
	while (n-- > 0)
		*(char *)dst++ = *(char *)src++;
	return (dstptr);
}

