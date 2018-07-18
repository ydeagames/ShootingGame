#include "Vector.h"

Vector Vector_Create(void);

void Vector_Delete(Vector* list);

void Vector_GetLast(Vector* list);

void Vector_GetFirst(Vector* list);

void Vector_Get(Vector* list, int index);

void Vector_AddLast(Vector* list, Object element);

void Vector_AddFirst(Vector* list, Object element);

void Vector_Add(Vector* list, int index, Object element);

void Vector_Set(Vector* list, int index, Object element);

void Vector_RemoveLast(Vector* list);

void Vector_RemoveFirst(Vector* list);

void Vector_Remove(Vector* list, int index);
