#include "Array.h"

#define DEFAULT_CAPACITY 10

Array Array_Create(void)
{
	Array obj = { NULL, 0, 0, 0 };
	Array_ReserveRequired(&obj, DEFAULT_CAPACITY);
	return obj;
}

void Array_ReserveRequired(Array* list, size_t min_capacity)
{
	if (list->capacity < min_capacity)
	{
		Array src = *list;
		list->capacity = min_capacity;
		list->base = (Object*)malloc(list->capacity * sizeof(Object));
		memcpy(list->base, src.base, src.capacity);
		free(src.base);
		list->limit = min_capacity;
	}
}

void Array_Reserve(Array* list, size_t min_capacity)
{
	if (list->capacity < min_capacity)
	{
		size_t old_capacity = list->capacity;
		int new_capacity = max(old_capacity + (old_capacity >> 1), min_capacity);

		Array_ReserveRequired(list, new_capacity);
	}
}

void Array_Delete(Array* list)
{
	free(list->base);
	*list = { NULL, 0, 0, 0 };
}

Object* Array_Get(Array* list, int index)
{
	return &list->base[index];
}

void Array_Set(Array* list, int index, Object element)
{
	list->base[index] = element;
}