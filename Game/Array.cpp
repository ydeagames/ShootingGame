#include "Array.h"

Array Array_Create(size_t new_capacity)
{
	Array obj = { NULL, 0, 0 };
	Array_SetSize(&obj, new_capacity);
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

void Array_SetSize(Array* list, size_t size)
{
	Array_Reserve(list, size);

	list->size = size;
}

int Array_GetSize(Array* list)
{
	return (int)list->size;
}

void Array_Delete(Array* list)
{
	free(list->base);
	*list = { NULL, 0, 0 };
}

Object* Array_Get(Array* list, int index)
{
	return &list->base[index];
}

void Array_Set(Array* list, int index, Object element)
{
	list->base[index] = element;
}