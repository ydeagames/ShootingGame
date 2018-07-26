#include "ScenePlay.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "GameMain.h"
#include "GameObjects.h"
#include "GameController.h"
#include "GameResource.h"
#include "GameUtils.h"
#include <math.h>
#include "Vector.h"




// 定数の定義 ==============================================================

#define PLAYER_SHOOTING_INTERVAL .5f
#define ENEMY_APPEAR_INTERVAL .5f
#define ENEMY_SHOOTING_INTERVAL .5f




// グローバル変数の定義 ====================================================

GameResource g_resources;

GameObject g_field;
GameObject g_player;
Vector g_player_bullets;
GameController g_player_ctrl;

Vector g_enemies;
Vector g_enemy_bullets;

GameTimer g_enemy_appear_count;




// 関数の宣言 ==============================================================

void InitializePlay(void);  // ゲームの初期化処理
void UpdatePlay(void);      // ゲームの更新処理
void RenderPlay(void);      // ゲームの描画処理
void FinalizePlay(void);    // ゲームの終了処理

BOOL UpdatePlayerBullet(void);
BOOL ReloadPlayerBullet(int n_way);
BOOL GrowPlayerBullet(void);
BOOL ShotPlayerBullet(int n_way);

BOOL AppearEnemy(void);

BOOL ShotEnemyBullet(const GameObject* enemy);
BOOL UpdateEnemyBullet(void);




// 関数の定義 ==============================================================

//----------------------------------------------------------------------
//! @brief ゲームの初期化処理
//!
//! @param[in] なし
//!
//! @return なし
//----------------------------------------------------------------------
void InitializePlay(void)
{
	g_resources = GameResource_Create();

	g_field = GameObject_Field_Create();
	//g_field.size.x -= 80;
	//g_field.size.y -= 80;
	g_field.sprite = GameSprite_Create(GameTexture_Create(g_resources.texture_map, Vec2_Create(), Vec2_Create(64, 64)), 1.5f);
	//g_field.sprite = GameSprite_Create(GameTexture_Create(g_resources.texture_map, Vec2_Create(), Vec2_Create(256, 256)), 1.f);
	//g_field.sprite = GameSprite_Create(GameTexture_Create(g_resources.texture_map, Vec2_Create(), Vec2_Create(1024, 2048)));
	//g_field.sprite.texture.center.x += 20;
	g_field.sprite_connection = CONNECTION_LOOP;
	//g_field.sprite.offset.y = g_field.sprite.texture.size.y / 2 - g_field.size.y / 2;

	g_player = GameObject_Player_Create();
	g_player.sprite = GameSprite_Create(GameTexture_Create(g_resources.texture_player, Vec2_Create(), Vec2_Create(32, 32)), 1.5f);
	g_player.sprite.offset.y -= 7;
	g_player.pos = Vec2_Create(GameObject_GetX(&g_field, CENTER_X), GameObject_GetY(&g_field, CENTER_Y) + 200);
	g_player.shape = SHAPE_CIRCLE;

	g_player_ctrl = GameController_Player_Create(&g_player, PlayerKeySet_Default_Create());

	g_player_bullets = Vector_Create();
	g_enemies = Vector_Create();
	g_enemy_bullets = Vector_Create();

	g_enemy_appear_count = GameTimer_Create();
}



//----------------------------------------------------------------------
//! @brief ゲームの更新処理
//!
//! @param[in] なし
//!
//! @return なし
//----------------------------------------------------------------------
void UpdatePlay(void)
{
	{
		if (GameTimer_IsPaused(&g_enemy_appear_count) || GameTimer_IsFinished(&g_enemy_appear_count))
		{
			AppearEnemy();
			GameTimer_SetRemaining(&g_enemy_appear_count, ENEMY_APPEAR_INTERVAL);
			GameTimer_Resume(&g_enemy_appear_count);
		}
	}

	{
		if (IsKeyPressed(PAD_INPUT_2))
			RequestScene(SCENE_RESULT);

		GameController_Update(&g_player_ctrl);
		GameController_UpdateControl(&g_player_ctrl);

		if (IsKeyPressed(PAD_INPUT_1))
			ReloadPlayerBullet(3);
		if (IsKeyDown(PAD_INPUT_1))
			GrowPlayerBullet();
		if (IsKeyReleased(PAD_INPUT_1))
			ShotPlayerBullet(3);
	}

	{
		g_field.sprite.offset.x--;
		g_field.sprite.offset.y--;
		//g_field.sprite.angle += ToRadians(2);

		if (GameObject_IsAlive(&g_player))
			GameObject_UpdatePosition(&g_player);

		UpdatePlayerBullet();

		foreach_start(&g_enemies, obj)
		{
			//GameObject_Enemy_Update(&g_enemies[i]);
			GameObject_UpdatePosition(obj);
			GameSpriteAnimation_Update(&obj->sprite.animation);

			if (GameTimer_IsPaused(&obj->count) || GameTimer_IsFinished(&obj->count))
			{
				ShotEnemyBullet(obj);
				GameTimer_SetRemaining(&obj->count, ENEMY_SHOOTING_INTERVAL);
				GameTimer_Resume(&obj->count);
			}
		} foreach_end;

		UpdateEnemyBullet();
	}

	{
		foreach_start(&g_enemies, enemy)
		{
			foreach_start(&g_player_bullets, player_bullet)
			{
				if (GameObject_IsHit(player_bullet, enemy))
				{
					VectorIterator_Remove(&itr_player_bullet);
					VectorIterator_Remove(&itr_enemy);
					break;
				}
			} foreach_end;
		} foreach_end;

		if (GameObject_IsAlive(&g_player))
		{
			foreach_start(&g_enemies, obj)
			{
				if (GameObject_IsHit(obj, &g_player))
				{
					//GameObject_Dispose(&g_player);
					DrawFormatString((int)GameObject_GetX(&g_field, LEFT, -10), (int)GameObject_GetY(&g_field, TOP, -(i_obj + 1) * 10.f), COLOR_WHITE, "当たっている");
				}
				else
				{
					DrawFormatString((int)GameObject_GetX(&g_field, LEFT, -10), (int)GameObject_GetY(&g_field, TOP, -(i_obj + 1) * 10.f), COLOR_WHITE, "当たっていない");
				}
			} foreach_end;
		}

		if (GameObject_IsAlive(&g_player))
		{
			GameObject_Field_CollisionVertical(&g_field, &g_player, CONNECTION_BARRIER, EDGESIDE_INNER);
			GameObject_Field_CollisionHorizontal(&g_field, &g_player, CONNECTION_BARRIER, EDGESIDE_INNER);
		}
		foreach_start(&g_player_bullets, obj)
		{
			if (GameObject_Field_CollisionVertical(&g_field, obj, CONNECTION_NONE, EDGESIDE_OUTER) ||
				GameObject_Field_CollisionHorizontal(&g_field, obj, CONNECTION_NONE, EDGESIDE_OUTER))
				GameObject_Dispose(obj);
		} foreach_end;
		foreach_start(&g_enemies, obj)
		{
			if (GameObject_Field_CollisionVertical(&g_field, obj, CONNECTION_NONE, EDGESIDE_OUTER) ||
				GameObject_Field_CollisionHorizontal(&g_field, obj, CONNECTION_NONE, EDGESIDE_OUTER))
				VectorIterator_Remove(&itr_obj);
		} foreach_end;
		foreach_start(&g_enemy_bullets, obj)
		{
			if (GameObject_Field_CollisionVertical(&g_field, obj, CONNECTION_NONE, EDGESIDE_OUTER) ||
				GameObject_Field_CollisionHorizontal(&g_field, obj, CONNECTION_NONE, EDGESIDE_OUTER))
				GameObject_Dispose(obj);
		} foreach_end;
	}
}

BOOL ReloadPlayerBullet(int n_way)
{
	int i;
	for (i = 0; i < n_way; i++)
	{
		GameObject obj = GameObject_Bullet_Create();
		obj.shape = SHAPE_CIRCLE;
		obj.sprite = GameSprite_Create(GameTexture_Create(g_resources.texture_bullet, Vec2_Create(), Vec2_Create(32, 32)));
		GameObject_Bullet_SetPosDefault(&obj, &g_player);
		Vector_AddLast(&g_player_bullets, &obj);
	}

	return TRUE;
}

BOOL GrowPlayerBullet(void)
{
	foreach_start(&g_player_bullets, obj)
	{
		if (obj->state == 1)
		{
			GameObject_Bullet_Grow(obj);
			GameObject_Bullet_SetPosDefault(obj, &g_player);
		}
	} foreach_end;

	return TRUE;
}

BOOL ShotPlayerBullet(int n_way)
{
	int num_shot = 0;
	foreach_start(&g_player_bullets, obj)
	{
		if (obj->state == 1)
		{
			GameObject_Bullet_SetVelDefault(obj, num_shot++, n_way);
			obj->state = 2;
		}
	} foreach_end;

	return TRUE;
}

BOOL UpdatePlayerBullet(void)
{
	foreach_start(&g_player_bullets, obj)
	{
		GameObject_UpdatePosition(obj);
	} foreach_end;

	return TRUE;
}

BOOL ShotEnemyBullet(const GameObject* enemy)
{
	GameObject obj = GameObject_Bullet_Create();
	obj.pos = enemy->pos;

	{
		float angle = Vec2_Angle(&Vec2_Sub(&g_player.pos, &enemy->pos));
		obj.vel = Vec2_Create(cosf(angle) * 5, sinf(angle) * 5);
		GameTimer_SetRemaining(&obj.count, 3.f);
		GameTimer_Resume(&obj.count);
	}

	//Vector_AddLast(&g_enemy_bullets, &obj);
	return TRUE;
}

BOOL UpdateEnemyBullet(void)
{
	foreach_start(&g_enemy_bullets, obj)
	{
		GameObject_UpdatePosition(obj);

		if (!GameTimer_IsFinished(&obj->count))
		{
			float angle = Vec2_Angle(&obj->vel);
			float direction = Vec2_Angle(&Vec2_Sub(&g_player.pos, &obj->pos));

			float theta = GetLoopRangeF(direction - angle, -DX_PI_F, DX_PI_F);
			float angle_direction = angle + ClampF(theta, -ToRadians(2), ToRadians(2));

			obj->vel = Vec2_Create(cosf(angle_direction) * BULLET_VEL, sinf(angle_direction) * BULLET_VEL);
		}
	} foreach_end;

	return TRUE;
}

BOOL AppearEnemy(void)
{
	GameObject obj = GameObject_Enemy_Create();
	obj.shape = SHAPE_CIRCLE;
	obj.sprite = GameSprite_Create(GameTexture_Create(g_resources.texture_explosion, Vec2_Create(0, 0), Vec2_Create(64, 64)));
	obj.sprite.animation = GameSpriteAnimation_Create(16, 4, 8);
	obj.pos = Vec2_Create(GameObject_GetX(&g_field, CENTER_X), GameObject_GetY(&g_field, CENTER_Y));
	//GameObject_Enemy_SetPosDefault(&g_enemies[i], &g_field);
	//GameObject_Enemy_SetVelDefault(&g_enemies[i]);
	Vector_AddLast(&g_enemies, &obj);

	return TRUE;
}



//----------------------------------------------------------------------
//! @brief ゲームの描画処理
//!
//! @param[in] なし
//!
//! @return なし
//----------------------------------------------------------------------
void RenderPlay(void)
{
	GameObject_Render(&g_field);

	foreach_start(&g_player_bullets, obj)
	{
		GameObject_Render(obj);
	} foreach_end;

	foreach_start(&g_enemies, obj)
	{
		GameObject_Render(obj);
	} foreach_end;

	foreach_start(&g_enemy_bullets, obj)
	{
		GameObject_Render(obj);
	} foreach_end;

	if (GameObject_IsAlive(&g_player))
		GameObject_Render(&g_player);
}



//----------------------------------------------------------------------
//! @brief ゲームの終了処理
//!
//! @param[in] なし
//!
//! @return なし
//----------------------------------------------------------------------
void FinalizePlay(void)
{
	GameResource_Delete(&g_resources);

	Vector_Delete(&g_player_bullets);
	Vector_Delete(&g_enemies);
	Vector_Delete(&g_enemy_bullets);
}
