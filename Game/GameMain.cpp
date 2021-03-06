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

#include "InputManager.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "GameResource.h"



// 定数の定義 ==============================================================




// グローバル変数の定義 ====================================================

GameResource g_resources;




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
	//SetUpMemoryLeakDetector();

	// F3キーをデバッグ描画に設定
	SetJoypadInputToKeyInput(DX_INPUT_KEY_PAD1, PAD_INPUT_11, KEY_INPUT_F3);

	// リソースを読み込み
	g_resources = GameResource_Create();

	// シーンマネージャーを初期化
	InitializeSceneManager(SCENE_LOGO);
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
	// 入力マネージャーを更新
	UpdateInputManager();
	// ティックの更新
	GameTick_Update();

	// F11キーが押されていたら
	if (IsKeyPressed(PAD_INPUT_11))
	{
		// デバッグモードをトグル
		extern BOOL g_debug_mode;
		g_debug_mode = !g_debug_mode;
	}

	// シーンマネージャーを更新
	UpdateSceneManager();
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
	// シーンマネージャーを描画
	RenderSceneManager();
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
	// シーンマネージャーをファイナライズ
	FinalizeSceneManager();

	// リソースをアンロード
	GameResource_Delete(&g_resources);
}
