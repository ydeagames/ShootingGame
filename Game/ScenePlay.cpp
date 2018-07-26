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
#include "GameContents.h"



// 定数の定義 ==============================================================

#define PLAYER_SHOOTING_INTERVAL .5f
#define ENEMY_APPEAR_INTERVAL .5f
#define ENEMY_SHOOTING_INTERVAL .5f




// グローバル変数の定義 ====================================================

GameContents g_game;
GameController g_player_ctrl;

GameObject g_screen_field;
int g_screen;




// 関数の宣言 ==============================================================

void InitializePlay(void);  // ゲームの初期化処理
void UpdatePlay(void);      // ゲームの更新処理
void RenderPlay(void);      // ゲームの描画処理
void FinalizePlay(void);    // ゲームの終了処理




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
	SetMouseDispFlag(FALSE);

	{
		g_game.field = GameObject_Field_Create();
		g_game.field.size = Vec2_Create(2048, 2048);
		g_game.field.pos = Vec2_Scale(&g_game.field.size, .5f);
		//g_game.field.size.x -= 80;
		//g_game.field.size.y -= 80;
		//g_game.field.sprite = GameSprite_Create(GameTexture_Create(g_game.resources.texture_map, Vec2_Create(), Vec2_Create(64, 64)), 1.5f);
		//g_game.field.sprite = GameSprite_Create(GameTexture_Create(g_game.resources.texture_map, Vec2_Create(), Vec2_Create(256, 256)), 1.f);
		g_game.field.sprite = GameSprite_Create(GameTexture_Create(g_resources.texture_map, Vec2_Create(), Vec2_Create(1024, 2048)));
		//g_game.field.sprite.texture.center.x += 20;
		g_game.field.sprite_connection = CONNECTION_LOOP;
		//g_game.field.sprite.offset.y = g_game.field.sprite.texture.size.y / 2 - g_game.field.size.y / 2;

		g_game.field_cloud = GameSprite_Create(GameTexture_Create(g_resources.texture_cloud, Vec2_Create(), Vec2_Create(1024, 1024)));
	}

	g_game.player = GameObject_Player_Create();
	g_game.player.sprite = GameSprite_Create(GameTexture_Create(g_resources.texture_player, Vec2_Create(), Vec2_Create(32, 32)), 1.5f);
	g_game.player.sprite.texture.center.y += 7;
	g_game.player.pos = Vec2_Create(GameObject_GetX(&g_game.field, CENTER_X), GameObject_GetY(&g_game.field, CENTER_Y));
	g_game.player.shape = SHAPE_CIRCLE;

	g_player_ctrl = GameController_Player_Create(&g_game.field, &g_game.player, PlayerKeySet_Default_Create());

	g_game.player_bullets = Vector_Create();
	g_game.enemies = Vector_Create();
	g_game.enemy_bullets = Vector_Create();

	g_game.enemy_appear_count = GameTimer_Create();

	{
		float diagonal = Vec2_Length(&Vec2_Create(SCREEN_WIDTH, SCREEN_HEIGHT));
		g_screen_field = GameObject_Create();
		g_screen_field.size = Vec2_Create(diagonal, diagonal);
		g_screen_field.pos = Vec2_Create(SCREEN_CENTER_X, SCREEN_CENTER_Y);
		g_screen = MakeScreen((int)g_screen_field.size.x, (int)g_screen_field.size.y);
		g_screen_field.sprite = GameSprite_Create(GameTexture_Create(g_screen, Vec2_Create(), g_screen_field.size));
		//g_screen_field.sprite.texture.center = Vec2_Scale(&g_game.field.size, .5f);
		g_screen_field.sprite.angle = ToRadians(0);
	}
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
		if (GameTimer_IsPaused(&g_game.enemy_appear_count) || GameTimer_IsFinished(&g_game.enemy_appear_count))
		{
			GameContents_AppearEnemy(&g_game);
			GameTimer_SetRemaining(&g_game.enemy_appear_count, ENEMY_APPEAR_INTERVAL*1000000000);
			GameTimer_Resume(&g_game.enemy_appear_count);
		}
	}

	{
		if (IsKeyPressed(PAD_INPUT_2))
			RequestScene(SCENE_RESULT);

		GameController_Update(&g_player_ctrl);
		GameController_UpdateControl(&g_player_ctrl);

		if (IsKeyPressed(PAD_INPUT_1))
			GameContents_ReloadPlayerBullet(&g_game, 3);
		if (IsKeyDown(PAD_INPUT_1))
			GameContents_GrowPlayerBullet(&g_game);
		if (IsKeyReleased(PAD_INPUT_1))
			GameContents_ShotPlayerBullet(&g_game, 3);
	}

	{
		g_game.field.sprite.offset = Vec2_Scale(&g_game.player.pos, -.5f);
		//g_game.field.sprite.angle += ToRadians(2);

		if (GameObject_IsAlive(&g_game.player))
			GameObject_UpdatePosition(&g_game.player);

		GameContents_UpdatePlayerBullet(&g_game);

		foreach_start(&g_game.enemies, obj)
		{
			//GameObject_Enemy_Update(&g_game.enemies[i]);
			GameObject_UpdatePosition(obj);
			GameSpriteAnimation_Update(&obj->sprite.animation);

			if (GameTimer_IsPaused(&obj->count) || GameTimer_IsFinished(&obj->count))
			{
				GameContents_ShotEnemyBullet(&g_game, obj);
				GameTimer_SetRemaining(&obj->count, ENEMY_SHOOTING_INTERVAL);
				GameTimer_Resume(&obj->count);
			}
		} foreach_end;

		GameContents_UpdateEnemyBullet(&g_game);
	}

	{
		foreach_start(&g_game.enemies, enemy)
		{
			foreach_start(&g_game.player_bullets, player_bullet)
			{
				if (GameObject_IsHit(player_bullet, enemy))
				{
					VectorIterator_Remove(&itr_player_bullet);
					VectorIterator_Remove(&itr_enemy);
					break;
				}
			} foreach_end;
		} foreach_end;

		/*
		if (GameObject_IsAlive(&g_game.player))
		{
			foreach_start(&g_game.enemies, obj)
			{
				if (GameObject_IsHit(obj, &g_game.player))
				{
					//GameObject_Dispose(&g_game.player);
					DrawFormatString((int)GameObject_GetX(&g_game.field, LEFT, -10), (int)GameObject_GetY(&g_game.field, TOP, -(i_obj + 1) * 10.f), COLOR_WHITE, "当たっている");
				}
				else
				{
					DrawFormatString((int)GameObject_GetX(&g_game.field, LEFT, -10), (int)GameObject_GetY(&g_game.field, TOP, -(i_obj + 1) * 10.f), COLOR_WHITE, "当たっていない");
				}
			} foreach_end;
		}
		/**/

		if (GameObject_IsAlive(&g_game.player))
		{
			GameObject_Field_CollisionVertical(&g_game.field, &g_game.player, CONNECTION_BARRIER, EDGESIDE_INNER);
			GameObject_Field_CollisionHorizontal(&g_game.field, &g_game.player, CONNECTION_BARRIER, EDGESIDE_INNER);
		}
		foreach_start(&g_game.player_bullets, obj)
		{
			if (GameObject_Field_CollisionVertical(&g_game.field, obj, CONNECTION_NONE, EDGESIDE_OUTER) ||
				GameObject_Field_CollisionHorizontal(&g_game.field, obj, CONNECTION_NONE, EDGESIDE_OUTER))
				GameObject_Dispose(obj);
		} foreach_end;
		foreach_start(&g_game.enemies, obj)
		{
			if (GameObject_Field_CollisionVertical(&g_game.field, obj, CONNECTION_NONE, EDGESIDE_OUTER) ||
				GameObject_Field_CollisionHorizontal(&g_game.field, obj, CONNECTION_NONE, EDGESIDE_OUTER))
				VectorIterator_Remove(&itr_obj);
		} foreach_end;
		foreach_start(&g_game.enemy_bullets, obj)
		{
			if (GameObject_Field_CollisionVertical(&g_game.field, obj, CONNECTION_NONE, EDGESIDE_OUTER) ||
				GameObject_Field_CollisionHorizontal(&g_game.field, obj, CONNECTION_NONE, EDGESIDE_OUTER))
				GameObject_Dispose(obj);
		} foreach_end;
	}
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
	int current = GetDrawScreen();
	SetDrawScreen(g_screen);
	ClearDrawScreen();

	{
		Vec2 screen_center = g_screen_field.pos;
		Vec2 offset = Vec2_Sub(&Vec2_Scale(&g_screen_field.size, .5f), &g_game.player.pos);

		GameObject_Render(&g_game.field);
		{
			GameObject cloud = g_game.field;
			cloud.sprite = g_game.field_cloud;
			cloud.sprite.offset = Vec2_Scale(&g_game.field.sprite.offset, 1.5f);
			GameObject_Render(&cloud);
			cloud.sprite.offset = Vec2_Scale(&g_game.field.sprite.offset, 2.f);
			GameObject_Render(&cloud);
			cloud.sprite.offset = Vec2_Scale(&g_game.field.sprite.offset, 2.5f);
			GameObject_Render(&cloud);
		}
		{
			GameObject frame;
			frame.pos = g_game.field.pos;
			frame.size = g_game.field.size;
			GameObject_Render(&frame, &offset);
		}

		foreach_start(&g_game.player_bullets, obj)
		{
			GameObject_Render(obj, &offset);
		} foreach_end;

		foreach_start(&g_game.enemies, obj)
		{
			GameObject_Render(obj, &offset);
		} foreach_end;

		foreach_start(&g_game.enemy_bullets, obj)
		{
			GameObject_Render(obj, &offset);
		} foreach_end;

		if (GameObject_IsAlive(&g_game.player))
			GameObject_Render(&g_game.player, &offset);
	}

	SetDrawScreen(current);
	g_screen_field.sprite.angle = -g_game.player.sprite.angle;
	GameObject_Render(&g_screen_field);
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
	DeleteGraph(g_screen);
	SetMouseDispFlag(TRUE);

	Vector_Delete(&g_game.player_bullets);
	Vector_Delete(&g_game.enemies);
	Vector_Delete(&g_game.enemy_bullets);
}
