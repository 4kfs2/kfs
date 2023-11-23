#ifndef __UTILS_H__
#define __UTILS_H__

#include <stddef.h>
#include <stdint.h>

int		printf(const char *, ...);

size_t	strlen(const char*);
void	*memcpy(void *, const void *, size_t);
int		strncmp(const char *, const char *, size_t);

void	reboot();

#endif
