#pragma once
#include "Vec2.h"
#include "Vector.h"
#include "GameResource.h"

// �萔�̒�` ==============================================================

#define BULLET_CHARGE_SIZE 3.f

// �\���̂̐錾 ============================================================

// <�Q�[���R���e���c>
typedef struct
{
	GameObject field;
	GameSprite field_cloud;
	GameSprite explosion;
	GameObject player;
	Vector player_bullets;
	Vector enemies;
	Vector enemy_bullets;
	GameTimer enemy_appear_count;
	GameTimer msg_show;
	int num_enemy2;
} GameContents;

// �֐��̐錾 ==============================================================

void GameContents_Update(GameContents* game);

BOOL GameContents_ReloadPlayerBullet(GameContents* game, int n_way);

BOOL GameContents_GrowPlayerBullet(GameContents* game);

BOOL GameContents_ShotPlayerBullet(GameContents* game, int n_way);

BOOL GameContents_UpdatePlayerBullet(GameContents* game);

BOOL GameContents_ShotEnemyBullet(GameContents* game, const GameObject* enemy);

BOOL GameContents_UpdateEnemies(GameContents* game);

BOOL GameContents_UpdateEnemyBullet(GameContents* game);

BOOL GameContents_AppearEnemy(GameContents* game);
