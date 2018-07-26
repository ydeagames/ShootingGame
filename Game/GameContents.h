#pragma once
#include "Vec2.h"
#include "Vector.h"
#include "GameResource.h"

// �\���̂̐錾 ============================================================

// <�Q�[���R���e���c>
typedef struct
{
	GameObject field;
	GameObject player;
	Vector player_bullets;
	Vector enemies;
	Vector enemy_bullets;
	GameTimer enemy_appear_count;
	GameResource resources;
} GameContents;

// �֐��̐錾 ==============================================================

BOOL GameContents_ReloadPlayerBullet(GameContents* game, int n_way);

BOOL GameContents_GrowPlayerBullet(GameContents* game);

BOOL GameContents_ShotPlayerBullet(GameContents* game, int n_way);

BOOL GameContents_UpdatePlayerBullet(GameContents* game);

BOOL GameContents_ShotEnemyBullet(GameContents* game, const GameObject* enemy);

BOOL GameContents_UpdateEnemyBullet(GameContents* game);

BOOL GameContents_AppearEnemy(GameContents* game);