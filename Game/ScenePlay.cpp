#include "ScenePlay.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "GameMain.h"
#include "GameObjects.h"
#include "GameController.h"
#include "GameResource.h"
#include "GameUtils.h"



// 定数の定義 ==============================================================

#define NUM_BULLETS 20



// グローバル変数の定義 ====================================================

int g_count;

GameResource g_resources;

GameObject g_field;
GameObject g_player;
GameController g_player_ctrl;

GameObject g_player_bullets[NUM_BULLETS];

GameObject g_screen_field;
int g_screen;


// 関数の宣言 ==============================================================

void InitializePlay(void);  // ゲームの初期化処理
void UpdatePlay(void);      // ゲームの更新処理
void RenderPlay(void);      // ゲームの描画処理
void FinalizePlay(void);    // ゲームの終了処理

BOOL ReloadPlayerBullet(int n_way);
BOOL GrowPlayerBullet(int n_way);
BOOL ShotPlayerBullet(int n_way);




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
	g_count = 0;

	g_resources = GameResource_Create();

	g_field = GameObject_Field_Create();

	g_player = GameObject_Player_Create();
	g_player.sprite = GameSprite_Create(GameTexture_Create(g_resources.texture_player, Vec2_Create(), Vec2_Create(32, 32)), 1.5f);
	g_player.sprite.offset.y -= 7;
	g_player.pos = g_field.pos;

	g_player_ctrl = GameController_Player_Create(&g_player, PlayerKeySet_Default_Create());

	{
		g_screen_field = GameObject_Create();
		g_screen_field.size = Vec2_Create(GetMaxF(SCREEN_WIDTH, SCREEN_HEIGHT), GetMaxF(SCREEN_WIDTH, SCREEN_HEIGHT));
		g_screen = MakeScreen((int)g_screen_field.size.x, (int)g_screen_field.size.y);
		g_screen_field.sprite = GameSprite_Create(GameTexture_Create(g_screen, Vec2_Create(), g_screen_field.size));
		g_screen_field.sprite.texture.center = Vec2_Scale(&g_field.size, .5f);
		g_screen_field.pos = g_field.pos;
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
	g_count++;

	if (IsKeyPressed(PAD_INPUT_2))
		RequestScene(SCENE_RESULT);

	GameController_Update(&g_player_ctrl);
	GameController_UpdateControl(&g_player_ctrl);

	if (IsKeyPressed(PAD_INPUT_1))
		ReloadPlayerBullet(3);
	if (IsKeyDown(PAD_INPUT_1))
		GrowPlayerBullet(3);
	if (IsKeyReleased(PAD_INPUT_1))
		ShotPlayerBullet(3);

	GameObject_UpdatePosition(&g_player);
	{
		int i;
		for (i = 0; i < NUM_BULLETS; i++)
		{
			if (GameObject_IsAlive(&g_player_bullets[i]))
				GameObject_UpdatePosition(&g_player_bullets[i]);
		}
	}

	GameObject_Field_CollisionVertical(&g_field, &g_player, CONNECTION_BARRIER, EDGESIDE_INNER);
	GameObject_Field_CollisionHorizontal(&g_field, &g_player, CONNECTION_BARRIER, EDGESIDE_INNER);
	{
		int i;
		for (i = 0; i < NUM_BULLETS; i++)
		{
			if (GameObject_Field_CollisionVertical(&g_field, &g_player_bullets[i], CONNECTION_NONE, EDGESIDE_OUTER) ||
				GameObject_Field_CollisionHorizontal(&g_field, &g_player_bullets[i], CONNECTION_NONE, EDGESIDE_OUTER))
				GameObject_Dispose(&g_player_bullets[i]);
		}
	}
}

BOOL ReloadPlayerBullet(int n_way)
{
	GameObject* bullets[NUM_BULLETS];
	int num_bullets = 0;
	int i;

	for (i = 0; i < n_way; i++)
		bullets[i] = NULL;

	for (i = 0; i < NUM_BULLETS; i++)
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
			bullets[i]->sprite = GameSprite_Create(GameTexture_Create(g_resources.texture_bullet, Vec2_Create(), Vec2_Create(32, 32)));
			GameObject_Bullet_SetPosDefault(bullets[i], &g_player);
		}

		return TRUE;
	}

	return FALSE;
}

BOOL GrowPlayerBullet(int n_way)
{
	int i;
	for (i = 0; i < NUM_BULLETS; i++)
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
	for (i = 0; i < NUM_BULLETS; i++)
	{
		if (GameObject_IsAlive(&g_player_bullets[i]) && g_player_bullets[i].state == 1)
		{
			GameObject_Bullet_SetVelDefault(&g_player_bullets[i], num_shot++, n_way);
			g_player_bullets[i].state = 2;
		}
	}

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
	int current = GetDrawScreen();
	SetDrawScreen(g_screen);
	ClearDrawScreen();

	{
		{
			int i;
			for (i = 0; i < NUM_BULLETS; i++)
			{
				if (GameObject_IsAlive(&g_player_bullets[i]))
					GameObject_Render(&g_player_bullets[i]);
			}
		}

		GameObject_Render(&g_player);
	}

	SetDrawScreen(current);
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
	GameResource_Delete(&g_resources);
}
