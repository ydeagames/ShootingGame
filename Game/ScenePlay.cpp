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

BOOL ShotPlayerBullet(GameObject* bullet);




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

	UpdateInputManager();

	GameController_Update(&g_player_ctrl);
	GameController_UpdateControl(&g_player_ctrl);

	if (g_count % 5 == 0 && IsKeyDown(PAD_INPUT_1))
	{
		int i;
		for (i = 0; i < NUM_BULLETS; i++)
		{
			if (ShotPlayerBullet(&g_player_bullets[i]))
				break;
		}
	}

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

BOOL ShotPlayerBullet(GameObject* bullet)
{
	if (!GameObject_IsAlive(bullet))
	{
		*bullet = GameObject_Bullet_Create();
		bullet->sprite = GameSprite_Create(GameTexture_Create(g_resources.texture_bullet, Vec2_Create(), Vec2_Create(32, 32)));
		GameObject_Bullet_SetPosDefault(bullet, &g_player);
		GameObject_Bullet_SetVelDefault(bullet);

		return TRUE;
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
	int current = GetDrawScreen();
	SetDrawScreen(g_screen);
	ClearDrawScreen();

	{
		GameObject_Render(&g_player);

		{
			int i;
			for (i = 0; i < NUM_BULLETS; i++)
			{
				if (GameObject_IsAlive(&g_player_bullets[i]))
					GameObject_Render(&g_player_bullets[i]);
			}
		}
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
