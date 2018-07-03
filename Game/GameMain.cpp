//__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/
//! @file   GameMain.cpp
//!
//! @brief  ゲーム関連のソースファイル
//!
//! @date   日付
//!
//! @author 制作者名
//__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/

// ヘッダファイルの読み込み ================================================

#include "GameMain.h"
#include "GameObjects.h"
#include "GameController.h"
#include "GameResource.h"
#include "InputManager.h"
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

void InitializeGame(void);  // ゲームの初期化処理
void UpdateGame(void);      // ゲームの更新処理
void RenderGame(void);      // ゲームの描画処理
void FinalizeGame(void);    // ゲームの終了処理




// 関数の定義 ==============================================================

//----------------------------------------------------------------------
//! @brief ゲームの初期化処理
//!
//! @param[in] なし
//!
//! @return なし
//----------------------------------------------------------------------
void InitializeGame(void)
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
void UpdateGame(void)
{
	g_count++;

	UpdateInputManager();

	GameController_Update(&g_player_ctrl);
	GameController_UpdateControl(&g_player_ctrl);

	if (g_count % 5 == 0 && IsKeyDown(PAD_INPUT_1))
	{
		int i;
		for (i = 0; i < NUM_BULLETS; i++)
		{
			if (!GameObject_IsAlive(&g_player_bullets[i]))
			{
				g_player_bullets[i] = GameObject_Bullet_Create();
				g_player_bullets[i].sprite = GameSprite_Create(GameTexture_Create(g_resources.texture_bullet, Vec2_Create(), Vec2_Create(32, 32)));
				GameObject_Bullet_SetPosDefault(&g_player_bullets[i], &g_player);
				GameObject_Bullet_SetVelDefault(&g_player_bullets[i]);

				break;
			}
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



//----------------------------------------------------------------------
//! @brief ゲームの描画処理
//!
//! @param[in] なし
//!
//! @return なし
//----------------------------------------------------------------------
void RenderGame(void)
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
void FinalizeGame(void)
{
	GameResource_Delete(&g_resources);
}
