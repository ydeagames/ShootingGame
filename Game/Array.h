#pragma once
#include "GameObject.h"

typedef GameObject Object;

typedef struct {
	Object* base;
	size_t capacity;
	size_t size;
} Array;

void Array_ReserveRequired(Array* list, size_t min_capacity);

void Array_Reserve(Array* list, size_t min_capacity);

void Array_SetSizeT(Array* list, size_t size);

size_t Array_GetSizeT(const Array* list);

void Array_SetSize(Array* list, int size);

int Array_GetSize(const Array* list);

Array Array_Create(size_t new_capacity);

void Array_Delete(Array* list);

Object* Array_Get(const Array* list, int index);

void Array_Set(Array* list, int index, const Object* element);
