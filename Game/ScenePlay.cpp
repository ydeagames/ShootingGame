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




// グローバル変数の定義 ====================================================

GameContents g_game;
GameController g_player_ctrl;

GameObject g_window_field;
GameObject g_screen_field;
int g_screen;

BOOL g_cleared;


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

	g_cleared = FALSE;

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
		g_window_field = GameObject_Create();
		g_window_field.size = Vec2_Create(SCREEN_WIDTH, SCREEN_HEIGHT);
		g_window_field.pos = Vec2_Create(SCREEN_CENTER_X, SCREEN_CENTER_Y);
	}
	{
		float diagonal = Vec2_Length(&g_window_field.size);
		g_screen_field = g_window_field;
		g_screen_field.size = Vec2_Create(diagonal, diagonal);
		g_screen = MakeScreen((int)g_screen_field.size.x, (int)g_screen_field.size.y);
		g_screen_field.sprite = GameSprite_Create(GameTexture_Create(g_screen, Vec2_Create(), g_screen_field.size));
		//g_screen_field.sprite.texture.center = Vec2_Scale(&g_game.field.size, .5f);
		g_screen_field.sprite.angle = ToRadians(0);
	}

	g_game.explosion = GameSprite_Create(GameTexture_Create(g_resources.texture_explosion, Vec2_Create(0, 0), Vec2_Create(64, 64)));
	g_game.explosion.animation = GameSpriteAnimation_Create(0, 15, 4, 8);
	g_game.msg_show = GameTimer_Create();
	GameTimer_SetRemaining(&g_game.msg_show, 2);
	GameTimer_Resume(&g_game.msg_show);

	{
		GameObject enemy;
		enemy = GameObject_Enemy_Create(9);
		enemy.sprite.scale *= 4;
		enemy.size = Vec2_Scale(&enemy.size, 4);
		enemy.type = TYPE_ENEMY2;
		enemy.pos = Vec2_Create(GameObject_GetX(&g_game.field, LEFT, -200), GameObject_GetY(&g_game.field, TOP, -200));
		Vector_AddLast(&g_game.enemies, &enemy);
		enemy.pos = Vec2_Create(GameObject_GetX(&g_game.field, RIGHT, -200), GameObject_GetY(&g_game.field, TOP, -200));
		Vector_AddLast(&g_game.enemies, &enemy);
		enemy.pos = Vec2_Create(GameObject_GetX(&g_game.field, LEFT, -200), GameObject_GetY(&g_game.field, BOTTOM, -200));
		Vector_AddLast(&g_game.enemies, &enemy);
		enemy.pos = Vec2_Create(GameObject_GetX(&g_game.field, RIGHT, -200), GameObject_GetY(&g_game.field, BOTTOM, -200));
		Vector_AddLast(&g_game.enemies, &enemy);
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
	GameContents_Update(&g_game);

	{
		if (IsKeyPressed(PAD_INPUT_2))
			RequestScene(SCENE_RESULT);

		GameController_Update(&g_player_ctrl);
		GameController_UpdateControl(&g_player_ctrl);

		if (IsMousePressed(MOUSE_INPUT_1))
			GameContents_ReloadPlayerBullet(&g_game, 3);
		if (IsMouseDown(MOUSE_INPUT_1))
			GameContents_GrowPlayerBullet(&g_game);
		if (IsMouseReleased(MOUSE_INPUT_1))
			GameContents_ShotPlayerBullet(&g_game, 3);
	}

	{
		g_game.field.sprite.offset = Vec2_Scale(&g_game.player.pos, -.5f);
		//g_game.field.sprite.angle += ToRadians(2);

		if (GameObject_IsAlive(&g_game.player))
			GameObject_UpdatePosition(&g_game.player);

		GameContents_UpdatePlayerBullet(&g_game);

		GameContents_UpdateEnemies(&g_game);

		GameContents_UpdateEnemyBullet(&g_game);
	}

	{
		foreach_start(&g_game.enemies, enemy)
		{
			foreach_start(&g_game.player_bullets, player_bullet)
			{
				if (GameObject_IsHit(player_bullet, enemy))
				{
					if (enemy->type == TYPE_ENEMY2)
					{
						if (Vec2_LengthSquaredTo(&g_game.player.pos, &enemy->pos) < Vec2_LengthSquared(&Vec2_Scale(&g_window_field.size, .5f)))
							if (enemy->state++ > 10)
							{
								enemy->sprite = g_game.explosion;
								GameTimer_SetRemaining(&g_game.msg_show, 2);
							}
					}
					else
						VectorIterator_Remove(&itr_enemy);
					VectorIterator_Remove(&itr_player_bullet);
					break;
				}
			} foreach_end;
		} foreach_end;

		g_game.num_enemy2 = 0;
		foreach_start(&g_game.enemies, enemy)
		{
			if (enemy->type == TYPE_ENEMY2)
			{
				if (enemy->state > 10)
				{
					if (enemy->sprite.animation.result == ANIMATION_FINISHED)
					{
						VectorIterator_Remove(&itr_enemy);
					}
				}
				else
					g_game.num_enemy2++;
			}
		} foreach_end;

		//*
		if (GameObject_IsAlive(&g_game.player))
		{
			foreach_start(&g_game.enemies, obj)
			{
				if (GameObject_IsHit(obj, &g_game.player))
				{
					RequestScene(SCENE_RESULT);
				}
			} foreach_end;
			foreach_start(&g_game.enemy_bullets, obj)
			{
				if (GameObject_IsHit(obj, &g_game.player))
				{
					RequestScene(SCENE_RESULT);
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
				VectorIterator_Remove(&itr_obj);
		} foreach_end;
		foreach_start(&g_game.enemies, obj)
		{
			if (GameObject_Field_CollisionVertical(&g_game.field, obj, CONNECTION_NONE, EDGESIDE_INNER))
				obj->vel.y *= -1;
			if (GameObject_Field_CollisionHorizontal(&g_game.field, obj, CONNECTION_NONE, EDGESIDE_INNER))
				obj->vel.x *= -1;
			obj->pos.x = ClampF(obj->pos.x, GameObject_GetX(obj, LEFT), GameObject_GetX(obj, RIGHT));
			obj->pos.y = ClampF(obj->pos.y, GameObject_GetY(obj, TOP), GameObject_GetY(obj, BOTTOM));
		} foreach_end;
		foreach_start(&g_game.enemy_bullets, obj)
		{
			if (GameObject_Field_CollisionVertical(&g_game.field, obj, CONNECTION_NONE, EDGESIDE_OUTER) ||
				GameObject_Field_CollisionHorizontal(&g_game.field, obj, CONNECTION_NONE, EDGESIDE_OUTER))
				VectorIterator_Remove(&itr_obj);
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
		GameObject tilearea = g_window_field;

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
			static float color = 0;
			GameObject frame = GameObject_Create();
			frame.edge = 4;
			frame.sprite.color = GetColorHSV(color, 1, 1);
			frame.pos = g_game.field.pos;
			frame.size = g_game.field.size;
			GameObject_Render(&frame, &offset);
			color = GetLoopF(color + 1, 360);
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

	if (!GameTimer_IsFinished(&g_game.msg_show))
	{
		char* msg = g_game.num_enemy2 >= 4 ? "四隅の敵を倒せ！" : "のこり%d体！";
		int width = GetDrawFormatStringWidth(msg, g_game.num_enemy2);
		DrawFormatStringToHandle((int)(GameObject_GetX(&g_window_field, CENTER_X) - width / 2), (int)(GameObject_GetY(&g_window_field, CENTER_Y) + 20), COLOR_RED,
			g_resources.font_menu, msg, g_game.num_enemy2);
	}
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
