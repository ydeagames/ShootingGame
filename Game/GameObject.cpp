#include "GameObject.h"
#include "GameUtils.h"
#include "GameMain.h"
#include "Vector.h"
#include <math.h>

// 定数の定義 ==============================================================

// <デバッグ用当たり判定表示>
#define DEBUG_HITBOX (g_debug_mode)

// グローバル変数の定義 ====================================================

BOOL g_debug_mode = FALSE;

// <<ティック>> --------------------------------------------------------

// <最終時刻>
static int g_lastcount = -1;
// <デルタミリ秒>
static int g_deltamilliseconds = 0;

// 関数の宣言 ==============================================================

static BOOL GameObject_IsHitOvalPoint(GameObject* oval, Vec2* p);
static BOOL GameObject_IsHitOval(GameObject* obj1, GameObject* obj2);
static BOOL GameObject_IsHitCirclePoint(GameObject* oval, Vec2* p);
static BOOL GameObject_IsHitCircle(GameObject* obj1, GameObject* obj2);
static BOOL GameObject_IsHitBox(GameObject* obj1, GameObject* obj2);

// 関数の定義 ==============================================================

// <<テクスチャ>> ------------------------------------------------------

// <テクスチャ作成>
GameTexture GameTexture_Create(HGRP texture, Vec2 anchor, Vec2 size)
{
	return{ texture, anchor, size, Vec2_Create(size.x / 2, size.y / 2) };
}

// <テクスチャなし>
GameTexture GameTexture_CreateNone()
{
	return GameTexture_Create(TEXTURE_NONE, Vec2_Create(), Vec2_Create());
}

// <<スプライトアニメーション>> ----------------------------------------

// <スプライトアニメーション作成>
GameSpriteAnimation GameSpriteAnimation_Create(int frames_start, int frames_end, int num_columns, int frame_duration)
{
	return{ frames_start, frames_end, num_columns, frame_duration, frames_start, 0, TRUE, ANIMATION_RUNNING };
}

// <スプライトアニメーションなし>
GameSpriteAnimation GameSpriteAnimation_CreateNone()
{
	return GameSpriteAnimation_Create(0, 0, 1, 1);
}

// <スプライトアニメーション更新>
AnimationState GameSpriteAnimation_Update(GameSpriteAnimation* animate_sprite)
{
	// アニメーションしているか
	animate_sprite->result = ANIMATION_RUNNING;
	// 経過時間
	animate_sprite->elapsed_time++;
	
	// フレーム経過
	if (animate_sprite->elapsed_time > animate_sprite->frame_duration)
	{
		// 経過時間
		animate_sprite->elapsed_time = 0;
		// フレーム番号
		animate_sprite->frame_index++;

		// 最初に戻る
		if (animate_sprite->frame_index > animate_sprite->frame_end)
		{
			// ループするなら
			if (animate_sprite->loop_flag)
				// 最初に戻る
				animate_sprite->frame_index = animate_sprite->frame_start;
			// アニメーション完了
			animate_sprite->result = ANIMATION_FINISHED;
		}
	}

	return animate_sprite->result;
}

// <<スプライト>> ------------------------------------------------------

// <スプライト作成>
GameSprite GameSprite_Create(GameTexture texture, float scale, float angle)
{
	return{ COLOR_WHITE, texture, Vec2_Create(), scale, angle, GameSpriteAnimation_CreateNone() };
}

// <スプライトなし>
GameSprite GameSprite_CreateNone()
{
	return GameSprite_Create(GameTexture_CreateNone(), 0, 0);
}

// <スプライト描画>
void GameSprite_Render(const GameSprite* sprite, const Vec2* pos)
{
	int column = sprite->animation.frame_index%sprite->animation.num_columns;
	int row = sprite->animation.frame_index/sprite->animation.num_columns;

	Vec2 anchor = Vec2_Add(&sprite->texture.anchor, &Vec2_Create(sprite->texture.size.x * column, sprite->texture.size.y * row));

	// スプライト描画
	DrawRectRotaGraph2F(
		pos->x + sprite->offset.x, pos->y + sprite->offset.y,
		(int)anchor.x, (int)anchor.y,
		(int)sprite->texture.size.x, (int)sprite->texture.size.y,
		sprite->texture.center.x, sprite->texture.center.y,
		(double)sprite->scale,
		(double)sprite->angle,
		sprite->texture.texture,
		TRUE
	);
}

// <<ティック>> --------------------------------------------------------

// <オブジェクト作成>
void GameTick_Update(void)
{
	int now = GetNowCount();
	g_deltamilliseconds = GetMin(100, now - g_lastcount);
	g_lastcount = now;
}

// <<オブジェクト>> ----------------------------------------------------

// <オブジェクト作成>
GameObject GameObject_Create(Vec2 pos, Vec2 vel, Vec2 size)
{
	return{ pos, vel, size, SHAPE_BOX, GameSprite_CreateNone(), FALSE, 0, CONNECTION_NONE, TRUE, 1, 0, GameTimer_Create() };
}

// <オブジェクト削除>
void GameObject_Dispose(GameObject* obj)
{
	obj->alive = FALSE;
}

// <オブジェクト確認>
BOOL GameObject_IsAlive(const GameObject* obj)
{
	return obj->alive;
}

// <オブジェクト座標更新>
void GameObject_UpdatePosition(GameObject* obj)
{
	obj->pos.x += obj->vel.x * (g_deltamilliseconds / 17.f);
	obj->pos.y += obj->vel.y * (g_deltamilliseconds / 17.f);
}

// <オブジェクトXオフセット>
float GameObject_OffsetX(const GameObject* obj, ObjectSide side, float pos, float padding)
{
	float offset = 0;
	switch (side)
	{
	case LEFT:
		offset = -(padding + obj->size.x / 2.f);
		break;
	case RIGHT:
		offset = (padding + obj->size.x / 2.f);
		break;
	}
	return pos + offset;
}

// <オブジェクトXオフセット>
float GameObject_OffsetY(const GameObject* obj, ObjectSide side, float pos, float padding)
{
	float offset = 0;
	switch (side)
	{
	case TOP:
		offset = -(padding + obj->size.y / 2.f);
		break;
	case BOTTOM:
		offset = (padding + obj->size.y / 2.f);
		break;
	}
	return pos + offset;
}

// <オブジェクトX位置ゲット>
float GameObject_GetX(const GameObject* obj, ObjectSide side, float padding)
{
	return GameObject_OffsetX(obj, side, obj->pos.x, padding);
}

// <オブジェクトY位置ゲット>
float GameObject_GetY(const GameObject* obj, ObjectSide side, float padding)
{
	return GameObject_OffsetY(obj, side, obj->pos.y, padding);
}

// <矩形オブジェクト×矩形オブジェクト当たり判定>
static BOOL GameObject_IsHitBox(const GameObject* obj1, const GameObject* obj2)
{
	return (
		GameObject_GetX(obj2, LEFT) < GameObject_GetX(obj1, RIGHT) &&
		GameObject_GetX(obj1, LEFT) < GameObject_GetX(obj2, RIGHT) &&
		GameObject_GetY(obj2, TOP) < GameObject_GetY(obj1, BOTTOM) &&
		GameObject_GetY(obj1, TOP) < GameObject_GetY(obj2, BOTTOM)
		);
}

// <楕円オブジェクト×楕円オブジェクト当たり判定> // ※未使用
static BOOL GameObject_IsHitOval(const GameObject* obj1, const GameObject* obj2)
{
	// STEP1 : obj2を単位円にする変換をobj1に施す
	float nx = obj1->size.x / 2;
	float ny = obj1->size.y / 2;
	float px = obj2->size.x / 2;
	float py = obj2->size.y / 2;
	float ox = (obj2->pos.x - obj1->pos.x);
	float oy = (obj2->pos.y - obj1->pos.y);

	// STEP2 : 一般式の算出
	float rx_pow2 = 1 / (nx*nx);
	float ry_pow2 = 1 / (ny*ny);
	float ax = rx_pow2 * px*px;
	float ay = ry_pow2 * py*py;
	float bx = 2 * rx_pow2*px*ox;
	float by = 2 * ry_pow2*py*oy;
	float g = ox * ox*rx_pow2 + oy * oy*ry_pow2 - 1;

	// STEP3 : 平行移動量の算出
	float tr_x = bx / (2 * ax);
	float tr_y = by / (2 * ay);

	// STEP4 : +1楕円を元に戻した式で当たり判定
	float kk = GetMinF(ax * tr_x*tr_x + ay * tr_y*tr_y - bx * tr_x - by * tr_y + g, 0);
	float ex = 1 + sqrtf(-kk / ax);
	float ey = 1 + sqrtf(-kk / ay);
	float JudgeValue = tr_x * tr_x / (ex*ex) + tr_y * tr_y / (ey*ey);

	return (JudgeValue <= 1);
}

// <楕円オブジェクト×点当たり判定> // ※未使用
static BOOL GameObject_IsHitOvalPoint(const GameObject* oval, const Vec2* p)
{
	// 点に楕円→真円変換行列を適用
	float tx = p->x - oval->pos.x;
	float ty = p->y - oval->pos.y;
	float rx = oval->size.x / 2;
	float ry = oval->size.y / 2;

	// 原点から移動後点までの距離を算出
	return  (tx * tx + ty * ty * (rx / ry) * (rx / ry) <= rx * rx);
}

// <円オブジェクト×円オブジェクト当たり判定>
static BOOL GameObject_IsHitCircle(const GameObject* obj1, const GameObject* obj2)
{
	float r1 = GetMinF(obj1->size.x, obj1->size.y) / 2;
	float r2 = GetMinF(obj2->size.x, obj2->size.y) / 2;

	return Vec2_LengthSquaredTo(&obj1->pos, &obj2->pos) < (r1 + r2)*(r1 + r2);
}

// <円オブジェクト×点当たり判定>
static BOOL GameObject_IsHitCirclePoint(const GameObject* circle, const Vec2* p)
{
	float r1 = GetMinF(circle->size.x, circle->size.y) / 2;

	return Vec2_LengthSquaredTo(&circle->pos, p) < r1*r1;
}

// <オブジェクト当たり判定>
BOOL GameObject_IsHit(const GameObject* obj1, const GameObject* obj2)
{
	if (obj1->shape == SHAPE_BOX && obj2->shape == SHAPE_BOX)
		return GameObject_IsHitBox(obj1, obj2);
	else if (obj1->shape == SHAPE_CIRCLE && obj2->shape == SHAPE_CIRCLE)
		return GameObject_IsHitBox(obj1, obj2) && GameObject_IsHitCircle(obj1, obj2);
	else
	{
		if (GameObject_IsHitBox(obj1, obj2))
		{
			if (obj1->shape == SHAPE_CIRCLE)
			{
				const GameObject* tmp = obj1;
				obj1 = obj2;
				obj2 = tmp;
			}
			if (GameObject_GetX(obj1, LEFT) <= obj2->pos.x && obj2->pos.x <= GameObject_GetX(obj1, RIGHT))
				return obj1->size.y / 2 + obj2->size.y / 2 > GetAbsF(obj2->pos.y - obj1->pos.y);
			else if (GameObject_GetY(obj1, TOP) <= obj2->pos.y && obj2->pos.y <= GameObject_GetY(obj1, BOTTOM))
				return obj1->size.x / 2 + obj2->size.x / 2 > GetAbsF(obj2->pos.x - obj1->pos.x);
			else
			{
				return (
					GameObject_IsHitCirclePoint(obj2, &Vec2_Create(GameObject_GetX(obj1, LEFT), GameObject_GetY(obj1, TOP))) ||
					GameObject_IsHitCirclePoint(obj2, &Vec2_Create(GameObject_GetX(obj1, RIGHT), GameObject_GetY(obj1, TOP))) ||
					GameObject_IsHitCirclePoint(obj2, &Vec2_Create(GameObject_GetX(obj1, LEFT), GameObject_GetY(obj1, BOTTOM))) ||
					GameObject_IsHitCirclePoint(obj2, &Vec2_Create(GameObject_GetX(obj1, RIGHT), GameObject_GetY(obj1, BOTTOM)))
					);
			}
		}

		return FALSE;
	}
}

// <オブジェクト描画>
void GameObject_Render(const GameObject* obj, const Vec2* translate)
{
	float box_xl = GameObject_GetX(obj, LEFT) + translate->x;
	float box_xc = GameObject_GetX(obj, CENTER_X) + translate->x;
	float box_xr = GameObject_GetX(obj, RIGHT) + translate->x;
	float box_yt = GameObject_GetY(obj, TOP) + translate->y;
	float box_ym = GameObject_GetY(obj, CENTER_Y) + translate->y;
	float box_yb = GameObject_GetY(obj, BOTTOM) + translate->y;
	Vec2 box_t = Vec2_Create(box_xc, box_ym);

	// テクスチャを確認
	if (obj->sprite.texture.texture != TEXTURE_NONE)
	{
		if (obj->sprite.texture.texture != TEXTURE_MISSING)
		{
			switch (obj->sprite_connection)
			{
			case CONNECTION_LOOP:
			case CONNECTION_BARRIER:
			{
				// リピートタイル (回転、テクスチャ中心座標 には未対応)
				Vec2 center_offset = Vec2_Scale(&obj->sprite.texture.center, obj->sprite.scale);
				Vec2 sp_pos = Vec2_Add(&box_t, &obj->sprite.offset);
				Vec2 sp_size = Vec2_Scale(&obj->sprite.texture.size, obj->sprite.scale);

				float go_left = box_xl;
				float go_right = box_xr;
				float go_top = box_yt;
				float go_bottom = box_yb;

				float sp_left = sp_pos.x - sp_size.x / 2;
				float sp_right = sp_pos.x + sp_size.x / 2;
				float sp_top = sp_pos.y - sp_size.y / 2;
				float sp_bottom = sp_pos.y + sp_size.y / 2;

				switch (obj->sprite_connection)
				{
				case CONNECTION_BARRIER:
					if (sp_left < go_right && go_left < sp_right && sp_top < go_bottom && go_top < sp_bottom)
						GameSprite_Render(&obj->sprite, &box_t);
					break;
				case CONNECTION_LOOP:
					float offset_x = GetLoopRangeF(go_left, sp_left, sp_right) - sp_left;
					float offset_y = GetLoopRangeF(go_top, sp_top, sp_bottom) - sp_top;

					if (sp_size.x >= 1.f && sp_size.y >= 1.f)
					{
						for (float iy = go_top + sp_size.y / 2 - offset_y - center_offset.y; iy < go_bottom; iy += sp_size.y)
						{
							for (float ix = go_left + sp_size.x / 2 - offset_x - center_offset.x; ix < go_right; ix += sp_size.x)
							{
								GameSprite_Render(&obj->sprite, &Vec2_Create(ix + sp_size.x / 2 - obj->sprite.offset.x, iy + sp_size.y / 2 - obj->sprite.offset.y));

								if (DEBUG_HITBOX)
									DrawBoxAA(ix, iy, ix + sp_size.x, iy + sp_size.y, obj->sprite.color, FALSE, .5f);
							}
						}
					}

					break;
				}

				break;
			}
			default:
				GameSprite_Render(&obj->sprite, &box_t);
				break;
			}
		}
		else
		{
			// NULLテクスチャを表示
			DrawBoxAA(box_xl, box_yt, box_xr, box_yb, COLOR_BLACK, TRUE);
			DrawBoxAA(box_xl, box_yt, box_xc, box_ym, COLOR_FUCHSIA, TRUE);
			DrawBoxAA(box_xc, box_ym, box_xr, box_yb, COLOR_FUCHSIA, TRUE);
			//DrawBoxAA(box_xl, box_yt, box_xr, box_yb, obj->sprite.color, FALSE, .5f);
		}
	}

	// 図形描画
	switch (obj->shape)
	{
	default:
	case SHAPE_BOX:
		// 矩形描画
		if (obj->fill)
			DrawBoxAA(box_xl, box_yt, box_xr, box_yb, obj->sprite.color, TRUE);
		if (obj->edge > 0)
			DrawBoxAA(box_xl, box_yt, box_xr, box_yb, obj->sprite.color, FALSE, obj->edge);
		if (DEBUG_HITBOX)
		{
			DrawBoxAA(box_xl, box_yt, box_xr, box_yb, obj->sprite.color, FALSE, .5f);
			DrawLineAA(box_xl, box_yt, box_xr, box_yb, obj->sprite.color, .5f);
			DrawLineAA(box_xr, box_yt, box_xl, box_yb, obj->sprite.color, .5f);
		}
		break;
	case SHAPE_CIRCLE:
	{
		float r1 = GetMinF(obj->size.x, obj->size.y) / 2;
		// 円
		if (obj->fill)
			DrawCircleAA(box_xc, box_ym, r1, 120, obj->sprite.color, TRUE);
		if (obj->edge > 0)
			DrawCircleAA(box_xc, box_ym, r1, 120, obj->sprite.color, FALSE, obj->edge);
		if (DEBUG_HITBOX)
		{
			DrawCircleAA(box_xc, box_ym, r1, 120, obj->sprite.color, FALSE, .5f);
			DrawBoxAA(box_xl, box_yt, box_xr, box_yb, obj->sprite.color, FALSE, .5f);
			DrawLineAA(box_xl, box_yt, box_xr, box_yb, obj->sprite.color, .5f);
			DrawLineAA(box_xr, box_yt, box_xl, box_yb, obj->sprite.color, .5f);
		}
		break;
	}
	}
}

// <<フィールドオブジェクト>> ------------------------------------------

// <フィールドオブジェクト作成>
GameObject GameObject_Field_Create(void)
{
	return GameObject_Create(Vec2_Create(SCREEN_CENTER_X, SCREEN_CENTER_Y), Vec2_Create(), Vec2_Create(SCREEN_WIDTH, SCREEN_HEIGHT));
}

// <フィールド上下衝突処理>
ObjectSide GameObject_Field_CollisionVertical(const GameObject* field, GameObject* obj, ObjectConnection connection, ObjectEdgeSide edge)
{
	// ヒットサイド
	ObjectSide side_hit = NONE;

	// コウラ・上下壁当たり判定
	{
		// 縁に応じてパディングを調整
		float padding_top = GameObject_GetY(field, TOP);
		float padding_bottom = GameObject_GetY(field, BOTTOM);
		switch (edge)
		{
		case EDGESIDE_INNER:
			padding_top = GameObject_OffsetY(obj, BOTTOM, padding_top);
			padding_bottom = GameObject_OffsetY(obj, TOP, padding_bottom);
			break;
		case EDGESIDE_OUTER:
			padding_top = GameObject_OffsetY(obj, TOP, padding_top);
			padding_bottom = GameObject_OffsetY(obj, BOTTOM, padding_bottom);
			break;
		}

		// 当たり判定
		if (obj->pos.y < padding_top)
			side_hit = TOP;
		else if (padding_bottom <= obj->pos.y)
			side_hit = BOTTOM;

		// フィールドのつながり
		switch (connection)
		{
		case CONNECTION_BARRIER:
			// 壁にあたったら調整
			obj->pos.y = ClampF(obj->pos.y, padding_top, padding_bottom);
			break;
		case CONNECTION_LOOP:
			// 壁にあたったらループ
			obj->pos.y = GetLoopRangeF(obj->pos.y, padding_top, padding_bottom);
			break;
		}
	}

	return side_hit;
}

// <フィールド左右衝突処理>
ObjectSide GameObject_Field_CollisionHorizontal(const GameObject* field, GameObject* obj, ObjectConnection connection, ObjectEdgeSide edge)
{
	// ヒットサイド
	ObjectSide side_hit = NONE;

	// コウラ・左右壁当たり判定
	{
		// 縁に応じてパディングを調整
		float padding_left = GameObject_GetX(field, LEFT);
		float padding_right = GameObject_GetX(field, RIGHT);
		switch (edge)
		{
		case EDGESIDE_INNER:
			padding_left = GameObject_OffsetX(obj, RIGHT, padding_left);
			padding_right = GameObject_OffsetX(obj, LEFT, padding_right);
			break;
		case EDGESIDE_OUTER:
			padding_left = GameObject_OffsetX(obj, LEFT, padding_left);
			padding_right = GameObject_OffsetX(obj, RIGHT, padding_right);
			break;
		}

		// 当たり判定
		if (obj->pos.x < padding_left)
			side_hit = LEFT;
		else if (padding_right <= obj->pos.x)
			side_hit = RIGHT;

		// フィールドのつながり
		switch (connection)
		{
		case CONNECTION_BARRIER:
			// 壁にあたったら調整
			obj->pos.x = ClampF(obj->pos.x, padding_left, padding_right);
			break;
		case CONNECTION_LOOP:
			// 壁にあたったらループ
			obj->pos.x = GetLoopRangeF(obj->pos.x, padding_left, padding_right);
			break;
		}
	}

	return side_hit;
}

// <フィールド描画>
void GameObject_Field_Render(const GameObject* field)
{
}
