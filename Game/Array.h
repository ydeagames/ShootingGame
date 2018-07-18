#pragma once
#include "GameObject.h"

typedef GameObject Object;

typedef struct {
	int size;
	int mod_count;
	Object* first;
	Object* base;
	Object* last;
} Array;

Array Array_Create(void);

void Array_Delete(Array* list);

void Array_Get(Array* list, int index);

void Array_Set(Array* list, int index, Object element);

void Array_Remove(Array* list, int index);
