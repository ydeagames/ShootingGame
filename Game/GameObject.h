#pragma once
#include "Vec2.h"

// 列挙型の定義 ============================================================

// <位置関係> ----------------------------------------------------------
typedef enum
{
	NONE = 0,
	CENTER_X = 1,				// X中央
	LEFT = -2,					// 左
	RIGHT = 2,					// 右
	CENTER_Y = -1,				// Y中央
	TOP = -3,					// 上
	BOTTOM = 3					// 下
} ObjectSide;

// <縁の位置> ----------------------------------------------------------
typedef enum
{
	EDGESIDE_CENTER = 0,		// 縁の上
	EDGESIDE_OUTER = -1,		// 縁の外側
	EDGESIDE_INNER = 1			// 縁の内側
} ObjectEdgeSide;

// <ワールドのつながり> ------------------------------------------------
typedef enum
{
	CONNECTION_NONE = 0,		// 繋がりなし、見えない空間に移動
	CONNECTION_BARRIER,			// 壁があり、進めない
	CONNECTION_LOOP				// 反対側から出てくる
} ObjectConnection;

// <オブジェクトの形> --------------------------------------------------
typedef enum
{
	SHAPE_BOX,					// 長方形
	SHAPE_CIRCLE,				// 円
} ObjectShape;

// 構造体の宣言 ============================================================

// <テクスチャ>
typedef struct
{
	HGRP texture;				// <テクスチャ>
	Vec2 anchor;				// <テクスチャ基点>
	Vec2 size;					// <テクスチャサイズ>
	Vec2 center;				// <テクスチャ中心>
} GameTexture;

// <スプライトオブジェクト>
typedef struct
{
	unsigned int color;			// <色>
	GameTexture texture;		// <テクスチャ>
	Vec2 offset;				// <オフセット>
	float scale;				// <スケール>
	float angle;				// <回転>
} GameSprite;

// <ゲームオブジェクト>
typedef struct
{
	Vec2 pos;					// <位置>
	Vec2 vel;					// <速度>
	Vec2 size;					// <大きさ>
	ObjectShape shape;			// <形>
	GameSprite sprite;			// <スプライト>
	BOOL alive;					// <表示状態>
	int state;					// <状態>
} GameObject;

// 定数の定義 ==============================================================

// <テクスチャ>
#define TEXTURE_MISSING -1		// テクスチャが見つかりません
#define TEXTURE_NONE -2			// テクスチャなし

// 関数の宣言 ==============================================================

// <<テクスチャ>> ------------------------------------------------------

// <テクスチャ作成>
GameTexture GameTexture_Create(HGRP texture, Vec2 anchor, Vec2 size);

// <テクスチャなし>
GameTexture GameTexture_CreateNone();

// <<スプライト>> ------------------------------------------------------

// <スプライト作成>
GameSprite GameSprite_Create(GameTexture texture, float scale = 1, float angle = 0);

// <スプライトなし>
GameSprite GameSprite_CreateNone();

// <<オブジェクト>> ----------------------------------------------------

// <オブジェクト作成>
GameObject GameObject_Create(Vec2 pos = Vec2_Create(), Vec2 vel = Vec2_Create(), Vec2 size = Vec2_Create());

// <オブジェクト削除>
void GameObject_Dispose(GameObject* obj);

// <オブジェクト確認>
BOOL GameObject_IsAlive(GameObject* obj);

// <オブジェクト座標更新>
void GameObject_UpdatePosition(GameObject* obj);

// <オブジェクトXオフセット>
float GameObject_OffsetX(GameObject* obj, ObjectSide side, float pos = 0.f, float margin = 0.f);

// <オブジェクトXオフセット>
float GameObject_OffsetY(GameObject* obj, ObjectSide side, float pos = 0.f, float margin = 0.f);

// <オブジェクトX位置ゲット>
float GameObject_GetX(GameObject* obj, ObjectSide side, float margin = 0.f);

// <オブジェクトY位置ゲット>
float GameObject_GetY(GameObject* obj, ObjectSide side, float margin = 0.f);

// <オブジェクト当たり判定>
BOOL GameObject_IsHit(GameObject* obj1, GameObject* obj2);

// <オブジェクト描画>
void GameObject_Render(GameObject* obj);

// <<フィールドオブジェクト>> ------------------------------------------

// <フィールドオブジェクト作成>
GameObject GameObject_Field_Create(void);

// <フィールド上下衝突処理>
ObjectSide GameObject_Field_CollisionVertical(GameObject* field, GameObject* obj, ObjectConnection connection, ObjectEdgeSide edge);

// <フィールド左右衝突処理>
ObjectSide GameObject_Field_CollisionHorizontal(GameObject* field, GameObject* obj, ObjectConnection connection, ObjectEdgeSide edge);

// <フィールド描画>
void GameObject_Field_Render(GameObject* field);
