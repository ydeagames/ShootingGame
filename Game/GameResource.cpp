#include "GameResource.h"
#include "GameObject.h"

// 定数の定義 ==============================================================

// <フォント> ----------------------------------------------------------
#define FONT_FILE_MAIN_CUSTOM "Resources\\Fonts\\TheSlavicFont-Regular.ttf"		// カスタムフォントファイル
#define FONT_NAME_MAIN_CUSTOM "The Slavic Font"									// カスタムフォントネーム
#define FONT_NAME_SPACERACE "HGP創英角ｺﾞｼｯｸUB"											// フォールバック用フォントネーム
#define FONT_NAME_MENU "HGP創英角ｺﾞｼｯｸUB"

// <サウンド> ----------------------------------------------------------

// <テクスチャ> --------------------------------------------------------
#define TEXTURE_PLAYER_PROTECTED "Resources\\Textures\\Protected\\Solvalou.png"		// リソース保護テクスチャ
#define TEXTURE_PLAYER "Resources\\Textures\\Solvalou.png"							// オープンテクスチャ
#define TEXTURE_BULLET_PROTECTED "Resources\\Textures\\Protected\\Bullet.png"		// リソース保護テクスチャ
#define TEXTURE_BULLET "Resources\\Textures\\Bullet.png"							// オープンテクスチャ
#define TEXTURE_ENEMY_PROTECTED "Resources\\Textures\\Protected\\Enemy.png"			// リソース保護テクスチャ
#define TEXTURE_ENEMY "Resources\\Textures\\Enemy.png"								// オープンテクスチャ
#define TEXTURE_EXPLOSION_PROTECTED "Resources\\Textures\\Protected\\Explosion.png"	// リソース保護テクスチャ
#define TEXTURE_EXPLOSION "Resources\\Textures\\Explosion.png"						// オープンテクスチャ
#define TEXTURE_MAP_PROTECTED "Resources\\Textures\\Protected\\Xevious_BG_map.png"	// リソース保護テクスチャ
#define TEXTURE_MAP "Resources\\Textures\\Xevious_BG_map.png"						// オープンテクスチャ

// <動画> --------------------------------------------------------------
#define MOVIE_LOGO "Resources\\Movies\\ydeagames.avi"								// ロゴムービー

// 関数の定義 ==============================================================

// <<リソース>> --------------------------------------------------

// <リソース作成読み込み>
GameResource GameResource_Create(void)
{
	GameResource res;

	// フォント
	// カスタムフォントファイルが読み込めたら使う
	if (AddFontResourceEx(FONT_FILE_MAIN_CUSTOM, FR_PRIVATE, NULL) > 0)
		res.font_main = CreateFontToHandle(FONT_NAME_MAIN_CUSTOM, FONT_SIZE_MAIN, 3, DX_FONTTYPE_ANTIALIASING_4X4);
	else
		res.font_main = CreateFontToHandle(FONT_NAME_SPACERACE, FONT_SIZE_MAIN, 3, DX_FONTTYPE_ANTIALIASING_4X4);
	res.font_menu = CreateFontToHandle(FONT_NAME_MENU, FONT_SIZE_MENU, 3, DX_FONTTYPE_ANTIALIASING_4X4);

	// サウンド

	// テクスチャ
	res.texture_player = LoadGraph(TEXTURE_PLAYER_PROTECTED);
	if (res.texture_player == TEXTURE_MISSING)										// リソース保護テクスチャが無かったら
		res.texture_player = LoadGraph(TEXTURE_PLAYER);								// オープンテクスチャを読み込む
	res.texture_bullet = LoadGraph(TEXTURE_BULLET_PROTECTED);
	if (res.texture_bullet == TEXTURE_MISSING)										// リソース保護テクスチャが無かったら
		res.texture_bullet = LoadGraph(TEXTURE_BULLET);								// オープンテクスチャを読み込む
	res.texture_enemy = LoadGraph(TEXTURE_ENEMY_PROTECTED);
	if (res.texture_enemy == TEXTURE_MISSING)										// リソース保護テクスチャが無かったら
		res.texture_enemy = LoadGraph(TEXTURE_ENEMY);								// オープンテクスチャを読み込む
	res.texture_explosion = LoadGraph(TEXTURE_EXPLOSION_PROTECTED);
	if (res.texture_explosion == TEXTURE_MISSING)									// リソース保護テクスチャが無かったら
		res.texture_explosion = LoadGraph(TEXTURE_EXPLOSION);						// オープンテクスチャを読み込む
	res.texture_map = LoadGraph(TEXTURE_MAP_PROTECTED);
	if (res.texture_map == TEXTURE_MISSING)											// リソース保護テクスチャが無かったら
		res.texture_map = LoadGraph(TEXTURE_MAP);									// オープンテクスチャを読み込む

	// 動画
	res.movie_logo = LoadGraph(MOVIE_LOGO);											// オープンテクスチャを読み込む

	return res;
}

// <リソース開放>
void GameResource_Delete(GameResource* res)
{
	// フォント
	DeleteFontToHandle(res->font_main);
	RemoveFontResourceEx(FONT_FILE_MAIN_CUSTOM, FR_PRIVATE, NULL);

	// サウンド

	// テクスチャ
	DeleteGraph(res->texture_player);
	DeleteGraph(res->texture_bullet);
	DeleteGraph(res->texture_enemy);
	DeleteGraph(res->texture_explosion);
	DeleteGraph(res->texture_map);

	// 動画
	DeleteGraph(res->movie_logo);
}
