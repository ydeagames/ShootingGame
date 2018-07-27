#include "GameContents.h"
#include "GameObjects.h"
#include "GameUtils.h"
#include "GameMain.h"
#include <math.h>

// 関数の定義 ==============================================================

BOOL GameContents_ReloadPlayerBullet(GameContents* game, int n_way)
{
	int i;
	for (i = 0; i < n_way; i++)
	{
		GameObject obj = GameObject_Bullet_Create();
		obj.shape = SHAPE_CIRCLE;
		obj.sprite = GameSprite_Create(GameTexture_Create(g_resources.texture_bullet, Vec2_Create(), Vec2_Create(32, 32)));
		GameObject_Bullet_SetPosDefault(&obj, &game->player);
		Vector_AddLast(&game->player_bullets, &obj);
	}

	return TRUE;
}

BOOL GameContents_GrowPlayerBullet(GameContents* game)
{
	foreach_start(&game->player_bullets, obj)
	{
		if (obj->state == 1)
		{
			GameObject_Bullet_Grow(obj);
			GameObject_Bullet_SetPosDefault(obj, &game->player);
		}
	} foreach_end;

	return TRUE;
}

BOOL GameContents_ShotPlayerBullet(GameContents* game, int n_way)
{
	int num_shot = 0;
	foreach_start(&game->player_bullets, obj)
	{
		if (obj->state == 1)
		{
			GameObject_Bullet_SetVelDefault(obj, game->player.sprite.angle, num_shot++, n_way);
			obj->vel = Vec2_Add(&obj->vel, &game->player.vel);
			obj->state = 2;
		}
	} foreach_end;

	return TRUE;
}

BOOL GameContents_UpdatePlayerBullet(GameContents* game)
{
	foreach_start(&game->player_bullets, obj)
	{
		GameObject_UpdatePosition(obj);
	} foreach_end;

	return TRUE;
}

BOOL GameContents_ShotEnemyBullet(GameContents* game, const GameObject* enemy)
{
	GameObject obj = GameObject_Bullet_Create();
	obj.pos = enemy->pos;

	{
		float angle = Vec2_Angle(&Vec2_Sub(&game->player.pos, &enemy->pos));
		obj.vel = Vec2_Create(cosf(angle) * 5, sinf(angle) * 5);
		obj.vel = Vec2_Add(&obj.vel, &enemy->vel);
		GameTimer_SetRemaining(&obj.count, 3.f);
		GameTimer_Resume(&obj.count);
	}

	//Vector_AddLast(&game->enemy_bullets, &obj);
	return TRUE;
}

BOOL GameContents_UpdateEnemyBullet(GameContents* game)
{
	foreach_start(&game->enemy_bullets, obj)
	{
		GameObject_UpdatePosition(obj);

		if (!GameTimer_IsFinished(&obj->count))
		{
			float angle = Vec2_Angle(&obj->vel);
			float direction = Vec2_Angle(&Vec2_Sub(&game->player.pos, &obj->pos));

			float theta = GetLoopRangeF(direction - angle, -DX_PI_F, DX_PI_F);
			float angle_direction = angle + ClampF(theta, -ToRadians(2), ToRadians(2));

			obj->vel = Vec2_Create(cosf(angle_direction) * BULLET_VEL, sinf(angle_direction) * BULLET_VEL);
		}
	} foreach_end;

	return TRUE;
}

BOOL GameContents_AppearEnemy(GameContents* game)
{
	struct FrameSet {
		int start;
		int end;
	};
	struct FrameSet sets[16] = {
		{0 + 0,6 + 0},
		{7 + 1,14 + 1},
		{15 + 1,21 + 1 },{22 + 1,22 + 1 },
		{23 + 1,29 + 1 },{30 + 1,30 + 1 },
		{31 + 1,34 + 1 },{35 + 1,38 + 1 },
		{39 + 1,42 + 1 },{43 + 1,46 + 1 },
		{47 + 1,52 + 1 },
		{53 + 3,58 + 3 },{59 + 3,59 + 3 },
		{60 + 4,63 + 4 },{64 + 4,67 + 4 },
		{68 + 4,73 + 4 }
	};

	GameObject obj = GameObject_Enemy_Create();
	obj.shape = SHAPE_CIRCLE;
	obj.sprite = GameSprite_Create(GameTexture_Create(g_resources.texture_enemy, Vec2_Create(0, 0), Vec2_Create(36, 36)));
	{
		int type = GetRand(15);
		obj.sprite.animation = GameSpriteAnimation_Create(sets[type].start, sets[type].end, 8, 8);
	}
	obj.pos = Vec2_Create(GetRandRangeF(GameObject_GetX(&game->field, LEFT), GameObject_GetX(&game->field, RIGHT)),
		GetRandRangeF(GameObject_GetY(&game->field, TOP), GameObject_GetY(&game->field, BOTTOM)));
	obj.vel = Vec2_Create(GetRandRangeF(-ENEMY_VEL, ENEMY_VEL), GetRandRangeF(-ENEMY_VEL, ENEMY_VEL));
	//GameObject_Enemy_SetPosDefault(&game->enemies[i], &game->field);
	//GameObject_Enemy_SetVelDefault(&game->enemies[i]);
	Vector_AddLast(&game->enemies, &obj);

	return TRUE;
}
