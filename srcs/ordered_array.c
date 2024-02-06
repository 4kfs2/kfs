#include "ordered_array.h"
#include "utils.h"
#include "panic.h"

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

void insert_ordered_array(type_t item, ordered_array_t *arr)
{
	uint32_t iterator = 0;
	while (iterator < arr->size && arr->lessthan(arr->arr[iterator], item))
		iterator++;
	if (iterator == arr->size)
		arr->arr[arr->size++] = item;
	else
	{
		type_t tmp = arr->arr[iterator];
		arr->arr[iterator] = item;
		while (iterator < arr->size)
		{
			iterator++;
			type_t tmp2 = arr->arr[iterator];
			arr->arr[iterator] = tmp;
			tmp = tmp2;
		}
		arr->size++;
	}
}

type_t select_ordered_array(uint32_t idx, ordered_array_t *arr)
{
	if (idx >= arr->size) // for debug
		panic_1("check arr_size, idx");
	return arr->arr[idx];
}
