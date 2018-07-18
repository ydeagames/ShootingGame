#pragma once
#include "GameObject.h"

typedef GameObject Object;

typedef struct {
	Object* base;
	size_t capacity;
	size_t size;
} Array;

Array Array_Create(size_t new_capacity);

void Array_ReserveRequired(Array* list, size_t min_capacity);

void Array_Reserve(Array* list, size_t min_capacity);

void Array_SetSize(Array* list, size_t size);

size_t Array_GetSize(Array* list);

void Array_Delete(Array* list);

Object* Array_Get(Array* list, int index);

void Array_Set(Array* list, int index, Object element);
