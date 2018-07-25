#include "Array.h"
#include <windows.h>

void Array_ReserveRequired(Array* list, size_t min_capacity)
{
	if (list->capacity < min_capacity)
	{
		Array src = *list;
		list->capacity = min_capacity;
		list->base = (Object*)malloc(list->capacity * sizeof(Object));
		if (src.base != NULL)
			memcpy(list->base, src.base, src.capacity);
		free(src.base);
	}
}

void Array_Reserve(Array* list, size_t min_capacity)
{
	if (list->capacity < min_capacity)
	{
		size_t old_capacity = list->capacity;
		size_t new_capacity = max(old_capacity + (old_capacity >> 1), min_capacity);

		Array_ReserveRequired(list, new_capacity);
	}
}

void Array_SetSizeT(Array* list, size_t size)
{
	Array_Reserve(list, size);

	list->size = size;
}

size_t Array_GetSizeT(const Array* list)
{
	return list->size;
}

void Array_SetSize(Array* list, int size)
{
	Array_Reserve(list, (size_t)size);

	list->size = (size_t)size;
}

int Array_GetSize(const Array* list)
{
	return (int)list->size;
}

Array Array_Create(size_t new_capacity)
{
	Array obj = { NULL, 0, 0 };
	Array_SetSize(&obj, new_capacity);
	return obj;
}

void Array_Delete(Array* list)
{
	free(list->base);
	*list = { NULL, 0, 0 };
}

Object* Array_Get(const Array* list, int index)
{
	return &list->base[index];
}

void Array_Set(Array* list, int index, const Object* element)
{
	list->base[index] = *element;
}
