#include <assert.h>
#include "Vector.h"

#define max(a,b)	(((a) > (b)) ? (a) : (b))

void Vector_ReserveLastRequired(Vector* list, size_t min_capacity)
{
	size_t capacity = Vector_GetCapacityT(list) + min_capacity;
	Vector src = *list;

	list->first_capacity = (Object*)malloc(capacity * sizeof(Object));
	list->last_capacity = list->first_capacity + capacity;

	if (src.first_capacity == NULL)
	{
		list->first = list->first_capacity;
		list->last = list->first_capacity;
	}
	else
	{
		size_t size = Vector_GetSize(&src);
		list->first = list->first_capacity + Vector_RemainingFirstT(&src);
		list->last = list->first + size;
		memcpy(Vector_GetFirst(list), Vector_GetFirst(&src), size * sizeof(Object));
	}
	free(src.first_capacity);
}

void Vector_ReserveLast(Vector* list, size_t min_capacity)
{
	if (Vector_RemainingLastT(list) <= 0)
	{
		size_t old_capacity = Vector_GetCapacityT(list);
		size_t new_capacity = max(old_capacity + (old_capacity >> 1), max(8, min_capacity));

		Vector_ReserveLastRequired(list, new_capacity);
	}
}

void Vector_ReserveFirstRequired(Vector* list, size_t min_capacity)
{
	size_t capacity = Vector_GetCapacityT(list) + min_capacity;
	Vector src = *list;

	list->first_capacity = (Object*)malloc(capacity * sizeof(Object));
	list->last_capacity = list->first_capacity + capacity;

	if (src.first_capacity == NULL)
	{
		list->first = list->last_capacity;
		list->last = list->last_capacity;
	}
	else
	{
		size_t size = Vector_GetSize(&src);
		list->last = list->last_capacity - Vector_RemainingLastT(&src);
		list->first = list->last - size;
		memcpy(Vector_GetFirst(list), Vector_GetFirst(&src), size * sizeof(Object));
	}
	free(src.first_capacity);
}

void Vector_ReserveFirst(Vector* list, size_t min_capacity)
{
	if (Vector_RemainingFirstT(list) <= 0)
	{
		size_t old_capacity = Vector_GetCapacityT(list);
		size_t new_capacity = max(old_capacity + (old_capacity >> 1), max(8, min_capacity));

		Vector_ReserveFirstRequired(list, new_capacity);
	}
}

Vector Vector_Create(void)
{
	return{ NULL, NULL, NULL, NULL };
}

void Vector_Delete(Vector* list)
{
	free(list->first_capacity);
	*list = { NULL, NULL, NULL, NULL };
}

Object* Vector_GetLast(const Vector* list)
{
	return list->last;
}

Object* Vector_GetFirst(const Vector* list)
{
	return list->first + 1;
}

Object* Vector_Get(const Vector* list, int index)
{
	return &Vector_GetFirst(list)[index];
}

void Vector_AddLast(Vector* list, const Object* element)
{
	Vector_ReserveLast(list, Vector_GetSize(list) + 1);
	list->last[1] = *element;
	list->last++;
}

void Vector_AddFirst(Vector* list, const Object* element)
{
	Vector_ReserveFirst(list, Vector_GetSize(list) + 1);
	list->first[0] = *element;
	list->first--;
}

void Vector_Add(Vector* list, int index, const Object* element)
{
	int i;
	int size = Vector_GetSize(list);

	assert((0 <= index && index <= size) && "IndexOutOfBoundsException");

	if (index < size / 2)
	{
		Vector_ReserveFirst(list, size + 1);
		for (i = 0; i < index; i++)
			Vector_GetFirst(list)[i - 1] = Vector_GetFirst(list)[i];
		list->first--;
		Vector_GetFirst(list)[index] = *element;
	}
	else
	{
		Vector_ReserveLast(list, size + 1);
		for (i = 0; i < index; i++)
			Vector_GetLast(list)[-(i - 1)] = Vector_GetLast(list)[-i];
		list->last++;
		Vector_GetLast(list)[-index] = *element;
	}
}

void Vector_Set(Vector* list, int index, const Object* element)
{
	Vector_GetFirst(list)[index] = *element;
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
		for (i = index; i > 0; i--)
			Vector_GetFirst(list)[i] = Vector_GetFirst(list)[i - 1];
		list->first++;
	}
	else
	{
		for (i = index; i > 0; i--)
			Vector_GetLast(list)[1 - i] = Vector_GetLast(list)[1 - (i - 1)];
		list->last--;
	}
}

size_t Vector_GetSizeT(const Vector* list)
{
	if (list->first_capacity == NULL)
		return 0;
	return list->last - list->first;
}

int Vector_GetSize(const Vector* list)
{
	return (int)Vector_GetSizeT(list);
}

size_t Vector_GetCapacityT(const Vector* list)
{
	if (list->first_capacity == NULL)
		return 0;
	return list->last_capacity - list->first_capacity;
}

size_t Vector_RemainingFirstT(const Vector* list)
{
	if (list->first_capacity == NULL)
		return 0;
	return list->first - list->first_capacity;
}

size_t Vector_RemainingLastT(const Vector* list)
{
	if (list->first_capacity == NULL)
		return 0;
	return list->last_capacity - list->last;
}

VectorIterator Vector_NextIterator(Vector* list)
{
	return VectorIterator_Create(list, 0, 1);
}

VectorIterator Vector_PrevIterator(Vector* list)
{
	return VectorIterator_Create(list, Vector_GetSize(list) - 1, -1);
}

VectorIterator VectorIterator_Create(Vector* list, int current, int next)
{
	return{ list, current, next };
}

BOOL VectorIterator_HasNext(const VectorIterator* itr)
{
	if (itr->next > 0)
		return itr->current < Vector_GetSize(itr->list);
	else
		return 0 <= itr->current;
}

Object* VectorIterator_Next(VectorIterator* itr)
{
	Object* obj = Vector_Get(itr->list, itr->current);
	itr->current += itr->next;
	return obj;
}

int VectorIterator_NextIndex(const VectorIterator* itr)
{
	return itr->current;
}

void VectorIterator_Add(VectorIterator* itr, const Object* element)
{
	Vector_Add(itr->list, itr->current++, element);
}

void VectorIterator_Set(VectorIterator* itr, const Object* element)
{
	Vector_Set(itr->list, itr->current, element);
}

void VectorIterator_Remove(VectorIterator* itr)
{
	Vector_Remove(itr->list, itr->current);
	itr -= itr->next;
}
