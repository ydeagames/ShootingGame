#include "SceneTitle.h"
#include "SceneManager.h"
#include "InputManager.h"

// グローバル変数の定義 ====================================================

int g_title_count;

// 関数の定義 ==============================================================

// タイトルシーンの初期化処理
void InitializeTitle(void)
{
	g_title_count = 0;
}

// タイトルシーンの更新処理
void UpdateTitle(void)
{
	//if (g_title_count++ >= 180)
	//if (IsButtonDown(PAD_INPUT_2))
	if (IsKeyPressed(PAD_INPUT_2))
		RequestScene(SCENE_PLAY);
}

// タイトルシーンの描画処理
void RenderTitle(void)
{
	DrawFormatString(10, 10, COLOR_WHITE, "タイトルシーン");
	DrawFormatString(10, 25, COLOR_WHITE, "PRESS X TO START");
	//DrawFormatString(10, 25, COLOR_WHITE, "カウント: %3d / 180", g_title_count);
}

// タイトルシーンの終了処理
void FinalizeTitle(void)
{

}