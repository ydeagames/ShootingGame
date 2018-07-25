#include "Vector.h"

void Vector_ReserveLastRequired(Vector* list, size_t min_capacity)
{
	int capacity = Vector_GetCapacity(list);
	if (capacity < min_capacity)
	{
		Vector src = *list;
		list->first_capacity = (Object*)malloc(min_capacity * sizeof(Object));
		list->last_capacity = list->first_capacity + min_capacity;
		if (src.first != NULL)
		{
			size_t size = Vector_GetSize(&src);
			list->first = list->first_capacity + (src.first - src.first_capacity);
			list->last = list->first + size;
			memcpy(list->first, src.first, size);
		}
		else
		{
			list->first = list->first_capacity;
			list->last = list->first_capacity;
		}
		free(src.first_capacity);
	}
}

void Vector_ReserveLast(Vector* list, size_t min_capacity)
{
	int capacity = Vector_GetCapacity(list);
	if (capacity < min_capacity)
	{
		size_t old_capacity = capacity;
		size_t new_capacity = max(old_capacity + (old_capacity >> 1), min_capacity);

		Vector_ReserveLastRequired(list, new_capacity);
	}
}

void Vector_ReserveFirstRequired(Vector* list, size_t min_capacity)
{
	int capacity = Vector_GetCapacity(list);
	if (capacity < min_capacity)
	{
		Vector src = *list;
		list->first_capacity = (Object*)malloc(min_capacity * sizeof(Object));
		list->last_capacity = list->first_capacity + min_capacity;
		if (src.first != NULL)
		{
			size_t size = Vector_GetSize(&src);
			list->last = list->last_capacity - (src.last_capacity - src.last);
			list->first = list->last - size;
			memcpy(list->first, src.first, size);
		}
		else
		{
			list->first = list->last_capacity;
			list->last = list->last_capacity;
		}
		free(src.first_capacity);
	}
}

void Vector_ReserveFirst(Vector* list, size_t min_capacity)
{
	int capacity = Vector_GetCapacity(list);
	if (capacity < min_capacity)
	{
		size_t old_capacity = capacity;
		size_t new_capacity = max(old_capacity + (old_capacity >> 1), min_capacity);

		Vector_ReserveFirstRequired(list, new_capacity);
	}
}

Vector Vector_Create(void)
{
	return{ NULL, NULL, NULL, NULL };
}

void Vector_Delete(Vector* list)
{
	free(list->base);
	*list = { NULL, NULL, NULL, NULL };
}

Object* Vector_GetLast(Vector* list)
{
	return list->last;
}

Object* Vector_GetFirst(Vector* list)
{
	return list->first;
}

Object* Vector_Get(Vector* list, int index)
{
	return &list->first[index];
}

void Vector_AddLast(Vector* list, const Object* element)
{
	Vector_ReserveLast(list, Vector_GetSize(list) + 1);
	list->last++;
	*list->last = *element;
}

void Vector_AddFirst(Vector* list, const Object* element)
{
	Vector_ReserveFirst(list, Vector_GetSize(list) + 1);
	list->first--;
	*list->first = *element;
}

void Vector_Add(Vector* list, int index, const Object* element)
{
	int i;
	int size = Vector_GetSize(list);
	if (index < size / 2)
	{
		Vector_ReserveFirst(list, size + 1);
		for (i = 0; i < index; i++)
			list->first[i - 1] = list->first[i];
		list->first--;
	}
	else
	{
		Vector_ReserveLast(list, size + 1);
		for (i = 0; i < index; i++)
			list->last[-(i - 1)] = list->last[-i];
		list->last++;
	}
}

void Vector_Set(Vector* list, int index, const Object* element)
{
	list->first[index] = *element;
}

void Vector_RemoveLast(Vector* list)
{
	list->last--;
}

void Vector_RemoveFirst(Vector* list)
{
	list->first++;
}

void Vector_Remove(Vector* list, int index)
{
	int i;
	int size = Vector_GetSize(list);
	if (index < size / 2)
	{
		for (i = index - 1; i >= 0; i--)
			list->first[i + 1] = list->first[i];
		list->first++;
	}
	else
	{
		for (i = index - 1; i >= 0; i--)
			list->last[-(i + 1)] = list->last[-i];
		list->last--;
	}
}

int Vector_GetSize(Vector* list)
{
	return (int)(list->last - list->first);
}

int Vector_GetCapacity(Vector* list)
{
	return (int)(list->last_capacity - list->first_capacity);
}
