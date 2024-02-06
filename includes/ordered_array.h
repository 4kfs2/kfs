#ifndef _ORDERED_ARRAY
# define _ORDERED_ARRAY

#include <stdint.h>
typedef void* type_t;
typedef int8_t		(*lessthan_predic_t)(void*, void*);
typedef struct ordered_array
{
	type_t				*arr;
	uint32_t			size;
	uint32_t			max_size;
	lessthan_predic_t	lessthan;
} ordered_array_t;

ordered_array_t init_ordered_array(void *start, uint32_t max_size, lessthan_predic_t less);
void insert_ordered_array(type_t item, ordered_array_t *arr);
type_t select_ordered_array(uint32_t idx, ordered_array_t *arr);

#endif