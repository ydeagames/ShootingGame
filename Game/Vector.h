#pragma once
#include "GameObject.h"

#define DEFAULT_CAPACITY 10

#define foreach(vec, var, exp) \
{ \
	VectorIterator itr; \
	for (itr = Vector_NextIterator(vec); VectorIterator_HasNext(&itr);) \
	{ \
		Object* var = VectorIterator_Next(&itr); \
		exp \
	} \
}

typedef GameObject Object;

typedef struct {
	Object* first_capacity;
	Object* first;
	Object* last;
	Object* last_capacity;
} Vector;

typedef struct {
	Vector* list;
	int current;
	int next;
} VectorIterator;

// コンストラクタ
Vector Vector_Create(void);

// デストラクタ
void Vector_Delete(Vector* list);

// 終端を取得
Object* Vector_GetLast(const Vector* list);

// 先頭を取得
Object* Vector_GetFirst(const Vector* list);

// 要素番号から取得
Object* Vector_Get(const Vector* list, int index);

// 終端に追加
void Vector_AddLast(Vector* list, const Object* element);

// 先頭に追加
void Vector_AddFirst(Vector* list, const Object* element);

// 要素番号へ追加
void Vector_Add(Vector* list, int index, const Object* element);

// 要素番号の位置の要素を置き換え
void Vector_Set(Vector* list, int index, const Object* element);

// 終端の要素を削除
void Vector_RemoveLast(Vector* list);

// 先頭の要素を削除
void Vector_RemoveFirst(Vector* list);

// 要素番号の要素を削除
void Vector_Remove(Vector* list, int index);

// サイズを取得
size_t Vector_GetSizeT(const Vector* list);

// サイズを取得
int Vector_GetSize(const Vector* list);

// 容量を取得
size_t Vector_GetCapacityT(const Vector* list);

// 先頭の空き容量を取得
size_t Vector_RemainingFirstT(const Vector* list);

// 終端の空き容量を取得
size_t Vector_RemainingLastT(const Vector* list);

// 昇順の反復子を作成
VectorIterator Vector_NextIterator(Vector* list);

// 降順の反復子を作成
VectorIterator Vector_PrevIterator(Vector* list);

// 反復子を作成
VectorIterator VectorIterator_Create(Vector* list, int current, int next);

// 次の要素が存在するか
BOOL VectorIterator_HasNext(const VectorIterator* itr);

// 次へ進む
Object* VectorIterator_Next(VectorIterator* itr);

// 次の要素の要素番号
int VectorIterator_NextIndex(const VectorIterator* itr);

// 現在の要素の前に追加 (※一度のNextにつき、一度しか呼び出すことはできません)
void VectorIterator_Add(VectorIterator* itr, const Object* element);

// 現在の要素を置き換え
void VectorIterator_Set(VectorIterator* itr, const Object* element);

// 現在の要素を削除する (※一度のNextにつき、一度しか呼び出すことはできません)
void VectorIterator_Remove(VectorIterator* itr);
