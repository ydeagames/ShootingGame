#pragma once

// 構造体の宣言 ============================================================

// <ゲームリソース>
typedef struct
{
	// <フォント>
	HFNT font_main;
	HFNT font_menu;

	// <サウンド>

	// <テクスチャ>
	HGRP texture_player;
	HGRP texture_bullet;
	HGRP texture_enemy;
	HGRP texture_explosion;
	HGRP texture_map;
	HGRP texture_cloud;

	// <動画>
	HGRP movie_logo;
} GameResource;

// 定数の定義 ==============================================================

// <フォント> ----------------------------------------------------------

#define FONT_SIZE_MAIN 80			// フォントサイズ
#define FONT_SIZE_MENU 20

// 関数の宣言 ==============================================================

// <<リソース>> --------------------------------------------------

// <リソース作成読み込み>
GameResource GameResource_Create(void);

// <リソース開放>
void GameResource_Delete(GameResource* res);
