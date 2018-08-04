#include "GameContents.h"
#include "GameObjects.h"
#include "GameUtils.h"
#include "GameMain.h"
#include <math.h>
#include "SceneManager.h"

// �萔�̒�` ==============================================================

#define PLAYER_SHOOTING_INTERVAL 3.5f		// �v���C���[�̔��ˊԊu
#define ENEMY_APPEAR_INTERVAL .5f			// �G�̏o���Ԋu
#define ENEMY_SHOOTING_INTERVAL 3.5f		// �G�̔��ˊԊu
#define ENEMY_BOSS_SHOOTING_INTERVAL 2.f	// �{�X�̔��ˊԊu

// �֐��̒�` ==============================================================

// �X�V
void GameContents_Update(GameContents* game)
{
	if (GameTimer_IsPaused(&game->enemy_appear_count) || GameTimer_IsFinished(&game->enemy_appear_count))
	{
		if (!GameContents_AppearEnemy(game))
		{
			g_cleared = TRUE;
			RequestScene(SCENE_RESULT);
		}
		GameTimer_SetRemaining(&game->enemy_appear_count, ENEMY_APPEAR_INTERVAL);
		GameTimer_Resume(&game->enemy_appear_count);
	}
}

// �v���C���[�e�����[�h
BOOL GameContents_ReloadPlayerBullet(GameContents* game, int n_way)
{
	int i;
	for (i = 0; i < n_way; i++)
	{
		GameObject obj = GameObject_Bullet_Create();
		obj.sprite = GameSprite_Create(GameTexture_Create(g_resources.texture_bullet, Vec2_Create(32, 0), Vec2_Create(32, 32)));
		GameObject_Bullet_SetPosDefault(&obj, &game->player);
		Vector_AddLast(&game->player_bullets, &obj);
	}

	return TRUE;
}

// �v���C���[�̒e��傫������
BOOL GameContents_GrowPlayerBullet(GameContents* game)
{
	foreach_start(&game->player_bullets, GameObject, obj)
	{
		if (obj->state == 1)
		{
			BOOL last = obj->sprite.scale > BULLET_CHARGE_SIZE;
			GameObject_Bullet_Grow(obj);
			if (!last && obj->sprite.scale > BULLET_CHARGE_SIZE)
				PlaySoundMem(g_resources.sound_se_charge, DX_PLAYTYPE_LOOP);
			GameObject_Bullet_SetPosDefault(obj, &game->player);
		}
	} foreach_end;

	return TRUE;
}

// �v���C���[�̒e����
BOOL GameContents_ShotPlayerBullet(GameContents* game, int n_way)
{
	int num_shot = 0;
	foreach_start(&game->player_bullets, GameObject, obj)
	{
		if (obj->state == 1)
		{
			GameObject_Bullet_SetVelDefault(obj, game->player.sprite.angle, num_shot++, n_way);
			obj->vel = Vec2_Add(&obj->vel, &game->player.vel);
			obj->state = 2;

			if (obj->sprite.scale > BULLET_CHARGE_SIZE)
				PlaySoundMem(g_resources.sound_se_shootbig, DX_PLAYTYPE_BACK);
			else
				PlaySoundMem(g_resources.sound_se_shoot, DX_PLAYTYPE_BACK);
		}
	} foreach_end;

	StopSoundMem(g_resources.sound_se_charge);

	return TRUE;
}

// �v���C���[�̒e�X�V
BOOL GameContents_UpdatePlayerBullet(GameContents* game)
{
	foreach_start(&game->player_bullets, GameObject, obj)
	{
		GameObject_UpdatePosition(obj);
	} foreach_end;

	return TRUE;
}

// �G�̒e����
BOOL GameContents_ShotEnemyBullet(GameContents* game, GameObject* enemy)
{
	GameObject obj = GameObject_Bullet_Create();
	obj.pos = enemy->pos;

	//if (Vec2_LengthSquaredTo(&game->player.pos, &enemy->pos) < Vec2_LengthSquared(&Vec2_Create(SCREEN_WIDTH, SCREEN_HEIGHT)))
	{
		float angle;
		if (enemy->type == TYPE_ENEMY2)
		{
			angle = Vec2_Angle(&Vec2_Sub(&game->player.pos, &enemy->pos));
			GameTimer_SetRemaining(&enemy->count, ENEMY_BOSS_SHOOTING_INTERVAL);
			GameObject_Bullet_SetSize(&obj, 4);
		}
		else
		{
			angle = GetRandF(DX_PI_F * 2);
			GameTimer_SetRemaining(&enemy->count, ENEMY_SHOOTING_INTERVAL);
			GameObject_Bullet_SetSize(&obj, 2);
		}
		obj.vel = Vec2_Create(cosf(angle) * 5, sinf(angle) * 5);
		obj.vel = Vec2_Add(&obj.vel, &enemy->vel);
		GameObject_Bullet_SetSize(&obj, 2);
		GameTimer_Resume(&enemy->count);
	}

	Vector_AddLast(&game->enemy_bullets, &obj);
	return TRUE;
}

// �G���X�V
BOOL GameContents_UpdateEnemies(GameContents* game)
{
	foreach_start(&game->enemies, GameObject, obj)
	{
		//GameObject_Enemy_Update(&g_game.enemies[i]);
		GameObject_UpdatePosition(obj);
		GameSpriteAnimation_Update(&obj->sprite.animation);

		if (GameTimer_IsPaused(&obj->count) || GameTimer_IsFinished(&obj->count))
		{
			GameContents_ShotEnemyBullet(game, obj);
		}
	} foreach_end;

	return TRUE;
}

// �G�̒e�X�V
BOOL GameContents_UpdateEnemyBullet(GameContents* game)
{
	foreach_start(&game->enemy_bullets, GameObject, obj)
	{
		GameObject_UpdatePosition(obj);

		if (!GameTimer_IsFinished(&obj->count))
		{
			//float angle = Vec2_Angle(&obj->vel);
			//float direction = Vec2_Angle(&Vec2_Sub(&game->player.pos, &obj->pos));

			//float theta = GetLoopRangeF(direction - angle, -DX_PI_F, DX_PI_F);
			//float angle_direction = angle + ClampF(theta, -ToRadians(2), ToRadians(2));

			//obj->vel = Vec2_Create(cosf(angle_direction) * BULLET_VEL, sinf(angle_direction) * BULLET_VEL);
		}
	} foreach_end;

	return TRUE;
}

// �G�o��
BOOL GameContents_AppearEnemy(GameContents* game)
{
	// �e�쐬
	GameObject obj = GameObject_Enemy_Create(GetRand(15));
	{
		// �{�X�擾�p���X�g
		Vector tmp = Vector_Create(sizeof(GameObject));
		// �{�X���擾
		foreach_start(&game->enemies, GameObject, obj)
		{
			// �{�X�������烊�X�g�ɒǉ�
			if (obj->type == TYPE_ENEMY2)
				Vector_AddLast(&tmp, obj);
		} foreach_end;
		// �{�X�������
		if (Vector_GetSize(&tmp) > 0)
		{
			// �{�X����U�R�o��
			obj.pos = ((GameObject*)Vector_Get(&tmp, GetRand(Vector_GetSize(&tmp) - 1)))->pos;
			// ���X�g�폜
			Vector_Delete(&tmp);
		}
		else
		{
			// ���X�g�폜
			Vector_Delete(&tmp);

			return FALSE;
		}
	}
	//do {
	//	obj.pos = Vec2_Create(GetRandRangeF(GameObject_GetX(&game->field, LEFT), GameObject_GetX(&game->field, RIGHT)),
	//		GetRandRangeF(GameObject_GetY(&game->field, TOP), GameObject_GetY(&game->field, BOTTOM)));
	//} while (Vec2_LengthSquaredTo(&obj.pos, &game->player.pos) < Vec2_LengthSquared(&Vec2_Create(SCREEN_WIDTH, SCREEN_HEIGHT)));
	// �G�̓����_���̑��x�ŏo��
	obj.vel = Vec2_Create(GetRandRangeF(-ENEMY_VEL, ENEMY_VEL), GetRandRangeF(-ENEMY_VEL, ENEMY_VEL));
	//GameObject_Enemy_SetPosDefault(&game->enemies[i], &game->field);
	//GameObject_Enemy_SetVelDefault(&game->enemies[i]);
	// �J�E���g
	GameTimer_SetRemaining(&obj.count, ENEMY_SHOOTING_INTERVAL);
	GameTimer_Resume(&obj.count);
	// �G���X�g�ɒǉ�
	Vector_AddLast(&game->enemies, &obj);

	return TRUE;
}
