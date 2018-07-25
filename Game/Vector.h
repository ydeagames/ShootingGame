#pragma once
#include "GameObject.h"

#define DEFAULT_CAPACITY 10

typedef GameObject Object;

typedef struct {
	Object* first_capacity;
	Object* first;
	Object* last;
	Object* last_capacity;
} Vector;

Vector Vector_Create(void);

void Vector_Delete(Vector* list);

Object* Vector_GetLast(const Vector* list);

Object* Vector_GetFirst(const Vector* list);

Object* Vector_Get(const Vector* list, int index);

void Vector_AddLast(Vector* list, const Object* element);

void Vector_AddFirst(Vector* list, const Object* element);

void Vector_Add(Vector* list, int index, const Object* element);

void Vector_Set(Vector* list, int index, const Object* element);

void Vector_RemoveLast(Vector* list);

void Vector_RemoveFirst(Vector* list);

void Vector_Remove(Vector* list, int index);

size_t Vector_GetSizeT(const Vector* list);

int Vector_GetSize(const Vector* list);

size_t Vector_GetCapacityT(const Vector* list);

size_t Vector_RemainingFirstT(const Vector* list);

size_t Vector_RemainingLastT(const Vector* list);
