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

int	strncmp(const char *s1, const char *s2, size_t n)
{
	size_t			i;
	unsigned char	*p1;
	unsigned char	*p2;

	p1 = (unsigned char *)s1;
	p2 = (unsigned char *)s2;
	i = 0;
	while (i < n && (*p1 || *p2))
	{
		if (*p1 - *p2)
			return (*p1 - *p2);
		i++;
		p1++;
		p2++;
	}
	return (0);
}
