#include "Array.h"

#define DEFAULT_CAPACITY 10

Array Array_Create(void)
{
	Array obj = {};
	//malloc();
	return obj;
}

void Array_Delete(Array* list);

void Array_Get(Array* list, int index);

void Array_Set(Array* list, int index, Object element);

void Array_Remove(Array* list, int index);
