#include "ordered_array.h"
#include "utils.h"

ordered_array_t init_ordered_array(void *start, uint32_t max_size, lessthan_predic_t less)
{
	ordered_array_t new;
	new.arr = (type_t *)start;
	memset(new.arr, 0, max_size*sizeof(type_t));
	new.size = 0;
	new.max_size = max_size;
	new.lessthan = less;
	return new;
}