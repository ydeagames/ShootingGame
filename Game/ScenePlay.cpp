#include "ScenePlay.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "GameMain.h"
#include "GameObjects.h"
#include "GameController.h"
#include "GameResource.h"
#include "GameUtils.h"




// 定数の定義 ==============================================================

#define NUM_PLAYER_BULLETS 20
#define NUM_ENEMIES 1
#define NUM_ENEMY_BULLETS 100

#define PLAYER_SHOOTING_INTERVAL .5f
#define ENEMY_APPEAR_INTERVAL .5f
#define ENEMY_SHOOTING_INTERVAL .5f




// グローバル変数の定義 ====================================================

GameResource g_resources;

GameObject g_field;
GameObject g_player;
GameObject g_player_bullets[NUM_PLAYER_BULLETS];
GameController g_player_ctrl;

GameObject g_enemies[NUM_ENEMIES];
GameObject g_enemy_bullets[NUM_ENEMY_BULLETS];

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

	g_player = GameObject_Player_Create();
	g_player.sprite = GameSprite_Create(GameTexture_Create(g_resources.texture_player, Vec2_Create(), Vec2_Create(32, 32)), 1.5f);
	g_player.sprite.offset.y -= 7;
	g_player.pos = Vec2_Create(GameObject_GetX(&g_field, CENTER_X), GameObject_GetY(&g_field, CENTER_Y) + 200);
	g_player.shape = SHAPE_CIRCLE;

	g_player_ctrl = GameController_Player_Create(&g_player, PlayerKeySet_Default_Create());

	{
		int i;
		for (i = 0; i < NUM_PLAYER_BULLETS; i++)
			GameObject_Dispose(&g_player_bullets[i]);
	}

	{
		int i;
		for (i = 0; i < NUM_ENEMY_BULLETS; i++)
			GameObject_Dispose(&g_enemy_bullets[i]);
	}

	{
		int i;
		for (i = 0; i < NUM_ENEMIES; i++)
			GameObject_Dispose(&g_enemies[i]);
	}

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
		if (GameObject_IsAlive(&g_player))
			GameObject_UpdatePosition(&g_player);
		UpdatePlayerBullet();

		{
			int i;
			for (i = 0; i < NUM_ENEMIES; i++)
			{
				if (GameObject_IsAlive(&g_enemies[i]))
				{
					//GameObject_Enemy_Update(&g_enemies[i]);
					GameObject_UpdatePosition(&g_enemies[i]);
					if (GameTimer_IsPaused(&g_enemies[i].count) || GameTimer_IsFinished(&g_enemies[i].count))
					{
						ShotEnemyBullet(&g_enemies[i]);
						GameTimer_SetRemaining(&g_enemies[i].count, ENEMY_SHOOTING_INTERVAL);
						GameTimer_Resume(&g_enemies[i].count);
					}
				}
			}
		}

		UpdateEnemyBullet();
	}

	{
		{
			int i;
			for (i = 0; i < NUM_PLAYER_BULLETS; i++)
			{
				if (GameObject_IsAlive(&g_player_bullets[i]))
				{
					int j;
					for (j = 0; j < NUM_ENEMIES; j++)
					{
						if (GameObject_IsAlive(&g_enemies[j]))
							if (GameObject_IsHit(&g_player_bullets[i], &g_enemies[j]))
							{
								GameObject_Dispose(&g_player_bullets[i]);
								GameObject_Dispose(&g_enemies[j]);
							}
					}
				}
			}
		}

		if (GameObject_IsAlive(&g_player))
		{
			int i;
			for (i = 0; i < NUM_ENEMIES; i++)
			{
				if (GameObject_IsAlive(&g_enemies[i]))
				{
					if (GameObject_IsHit(&g_enemies[i], &g_player))
					{
						//GameObject_Dispose(&g_player);
						DrawFormatString((int) GameObject_GetX(&g_field, LEFT, -10), (int) GameObject_GetY(&g_field, TOP, -(i + 1) * 10), COLOR_WHITE, "当たっている");
					}
					else
					{
						DrawFormatString((int) GameObject_GetX(&g_field, LEFT, -10), (int) GameObject_GetY(&g_field, TOP, -(i + 1) * 10), COLOR_WHITE, "当たっていない");
					}
				}
			}
		}

		if (GameObject_IsAlive(&g_player))
		{
			GameObject_Field_CollisionVertical(&g_field, &g_player, CONNECTION_BARRIER, EDGESIDE_INNER);
			GameObject_Field_CollisionHorizontal(&g_field, &g_player, CONNECTION_BARRIER, EDGESIDE_INNER);
		}
		{
			int i;
			for (i = 0; i < NUM_PLAYER_BULLETS; i++)
			{
				if (GameObject_Field_CollisionVertical(&g_field, &g_player_bullets[i], CONNECTION_NONE, EDGESIDE_OUTER) ||
					GameObject_Field_CollisionHorizontal(&g_field, &g_player_bullets[i], CONNECTION_NONE, EDGESIDE_OUTER))
					GameObject_Dispose(&g_player_bullets[i]);
			}
		}
		{
			int i;
			for (i = 0; i < NUM_ENEMIES; i++)
			{
				if (GameObject_Field_CollisionVertical(&g_field, &g_enemies[i], CONNECTION_NONE, EDGESIDE_OUTER) ||
					GameObject_Field_CollisionHorizontal(&g_field, &g_enemies[i], CONNECTION_NONE, EDGESIDE_OUTER))
					GameObject_Dispose(&g_enemies[i]);
			}
		}
	}
}

BOOL ReloadPlayerBullet(int n_way)
{
	GameObject* bullets[NUM_PLAYER_BULLETS];
	int num_bullets = 0;
	int i;

	for (i = 0; i < n_way; i++)
		bullets[i] = NULL;

	for (i = 0; i < NUM_PLAYER_BULLETS; i++)
	{
		if (!GameObject_IsAlive(&g_player_bullets[i]))
		{
			bullets[num_bullets++] = &g_player_bullets[i];
			if (num_bullets == n_way)
				break;
		}
	}

	if (num_bullets == n_way)
	{
		int i;
		for (i = 0; i < n_way; i++)
		{
			*bullets[i] = GameObject_Bullet_Create();
			bullets[i]->shape = SHAPE_CIRCLE;
			bullets[i]->sprite = GameSprite_Create(GameTexture_Create(g_resources.texture_bullet, Vec2_Create(), Vec2_Create(32, 32)));
			GameObject_Bullet_SetPosDefault(bullets[i], &g_player);
		}

		return TRUE;
	}

	return FALSE;
}

BOOL GrowPlayerBullet(void)
{
	int i;
	for (i = 0; i < NUM_PLAYER_BULLETS; i++)
	{
		if (g_player_bullets[i].state == 1)
		{
			GameObject_Bullet_Grow(&g_player_bullets[i]);
			GameObject_Bullet_SetPosDefault(&g_player_bullets[i], &g_player);
		}
	}

	return TRUE;
}

BOOL ShotPlayerBullet(int n_way)
{
	int num_shot = 0;
	int i;
	for (i = 0; i < NUM_PLAYER_BULLETS; i++)
	{
		if (GameObject_IsAlive(&g_player_bullets[i]) && g_player_bullets[i].state == 1)
		{
			GameObject_Bullet_SetVelDefault(&g_player_bullets[i], num_shot++, n_way);
			g_player_bullets[i].state = 2;
		}
	}

	return TRUE;
}

BOOL UpdatePlayerBullet(void)
{
	int i;
	for (i = 0; i < NUM_PLAYER_BULLETS; i++)
	{
		if (GameObject_IsAlive(&g_player_bullets[i]))
			GameObject_UpdatePosition(&g_player_bullets[i]);
	}

	return TRUE;
}

BOOL ShotEnemyBullet(const GameObject* enemy)
{
	int i;
	for (i = 0; i < NUM_ENEMY_BULLETS; i++)
	{
		if (!GameObject_IsAlive(&g_enemy_bullets[i]))
		{
			g_enemy_bullets[i] = GameObject_Bullet_Create();
			g_enemy_bullets[i].pos = enemy->pos;
			g_enemy_bullets[i].vel = Vec2_Create(0, 5);

			return TRUE;
		}
	}

	return FALSE;
}

BOOL UpdateEnemyBullet(void)
{
	int i;
	for (i = 0; i < NUM_ENEMY_BULLETS; i++)
	{
		if (GameObject_IsAlive(&g_enemy_bullets[i]))
			GameObject_UpdatePosition(&g_enemy_bullets[i]);
	}

	return TRUE;
}

BOOL AppearEnemy(void)
{
	int i;
	for (i = 0; i < NUM_ENEMIES; i++)
	{
		if (!GameObject_IsAlive(&g_enemies[i]))
		{
			g_enemies[i] = GameObject_Enemy_Create();
			g_enemies[i].shape = SHAPE_CIRCLE;
			g_enemies[i].sprite = GameSprite_Create(GameTexture_Create(g_resources.texture_enemy, Vec2_Create(0, GetRand(9) * 32.f), Vec2_Create(32, 32)));
			g_enemies[i].pos = Vec2_Create(GameObject_GetX(&g_field, CENTER_X), GameObject_GetY(&g_field, CENTER_Y));
			//GameObject_Enemy_SetPosDefault(&g_enemies[i], &g_field);
			//GameObject_Enemy_SetVelDefault(&g_enemies[i]);
			return TRUE;
		}
	}

	return FALSE;
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
	{
		int i;
		for (i = 0; i < NUM_PLAYER_BULLETS; i++)
		{
			if (GameObject_IsAlive(&g_player_bullets[i]))
				GameObject_Render(&g_player_bullets[i]);
		}
	}

	{
		int i;
		for (i = 0; i < NUM_ENEMIES; i++)
		{
			if (GameObject_IsAlive(&g_enemies[i]))
				GameObject_Render(&g_enemies[i]);
		}
	}

	{
		int i;
		for (i = 0; i < NUM_ENEMY_BULLETS; i++)
		{
			if (GameObject_IsAlive(&g_enemy_bullets[i]))
				GameObject_Render(&g_enemy_bullets[i]);
		}
	}

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
}
