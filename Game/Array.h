#pragma once
#include "GameObject.h"

typedef GameObject Object;

typedef struct {
	Object* base;
	size_t capacity;
	size_t size;
} Array;

Array Array_Create(size_t new_capacity);

void Array_Delete(Array* list);

Object* Array_Get(Array* list, int index);

void Array_Set(Array* list, int index, Object element);
