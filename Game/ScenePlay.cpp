#include "ScenePlay.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "GameMain.h"
#include "GameObjects.h"
#include "GameController.h"
#include "GameResource.h"
#include "GameUtils.h"
#include <math.h>
#include "Array.h"




// �萔�̒�` ==============================================================

#define NUM_PLAYER_BULLETS 20
#define NUM_ENEMIES 1
#define NUM_ENEMY_BULLETS 100

#define PLAYER_SHOOTING_INTERVAL .5f
#define ENEMY_APPEAR_INTERVAL .5f
#define ENEMY_SHOOTING_INTERVAL .5f




// �O���[�o���ϐ��̒�` ====================================================

GameResource g_resources;

GameObject g_field;
GameObject g_player;
GameObject g_player_bullets[NUM_PLAYER_BULLETS];
GameController g_player_ctrl;

Array g_enemies = Array_Create(NUM_ENEMIES);
GameObject g_enemy_bullets[NUM_ENEMY_BULLETS];

GameTimer g_enemy_appear_count;




// �֐��̐錾 ==============================================================

void InitializePlay(void);  // �Q�[���̏���������
void UpdatePlay(void);      // �Q�[���̍X�V����
void RenderPlay(void);      // �Q�[���̕`�揈��
void FinalizePlay(void);    // �Q�[���̏I������

BOOL UpdatePlayerBullet(void);
BOOL ReloadPlayerBullet(int n_way);
BOOL GrowPlayerBullet(void);
BOOL ShotPlayerBullet(int n_way);

BOOL AppearEnemy(void);

BOOL ShotEnemyBullet(const GameObject* enemy);
BOOL UpdateEnemyBullet(void);




// �֐��̒�` ==============================================================

//----------------------------------------------------------------------
//! @brief �Q�[���̏���������
//!
//! @param[in] �Ȃ�
//!
//! @return �Ȃ�
//----------------------------------------------------------------------
void InitializePlay(void)
{
	g_resources = GameResource_Create();

	g_field = GameObject_Field_Create();
	g_field.size.x -= 80;
	g_field.size.y -= 80;
	//g_field.sprite = GameSprite_Create(GameTexture_Create(g_resources.texture_map, Vec2_Create(), Vec2_Create(64, 64)), 1.5f);
	g_field.sprite = GameSprite_Create(GameTexture_Create(g_resources.texture_map, Vec2_Create(), Vec2_Create(256, 256)), 1.f);
	//g_field.sprite = GameSprite_Create(GameTexture_Create(g_resources.texture_map, Vec2_Create(), Vec2_Create(1024, 2048)));
	//g_field.sprite.texture.center.x += 20;
	g_field.sprite_connection = CONNECTION_LOOP;
	//g_field.sprite.offset.y = g_field.sprite.texture.size.y / 2 - g_field.size.y / 2;

	g_player = GameObject_Player_Create();
	g_player.sprite = GameSprite_Create(GameTexture_Create(g_resources.texture_player, Vec2_Create(), Vec2_Create(32, 32)), 1.5f);
	g_player.sprite.offset.y -= 7;
	g_player.pos = Vec2_Create(GameObject_GetX(&g_field, CENTER_X), GameObject_GetY(&g_field, CENTER_Y) + 200);
	g_player.shape = SHAPE_CIRCLE;

	g_player_ctrl = GameController_Player_Create(&g_player, PlayerKeySet_Default_Create());

	{
		int i;
		for (i = 0; i < NUM_PLAYER_BULLETS; i++)
			GameObject_Dispose(&g_player_bullets[i]);
	}

	{
		int i;
		for (i = 0; i < NUM_ENEMY_BULLETS; i++)
			GameObject_Dispose(&g_enemy_bullets[i]);
	}

	{
		int i;
		for (i = 0; i < Array_GetSize(&g_enemies); i++)
		{
			Array_Set(&g_enemies, i, GameObject_Create());
			GameObject_Dispose(Array_Get(&g_enemies, i));
		}
	}

	g_enemy_appear_count = GameTimer_Create();
}



//----------------------------------------------------------------------
//! @brief �Q�[���̍X�V����
//!
//! @param[in] �Ȃ�
//!
//! @return �Ȃ�
//----------------------------------------------------------------------
void UpdatePlay(void)
{
	{
		if (GameTimer_IsPaused(&g_enemy_appear_count) || GameTimer_IsFinished(&g_enemy_appear_count))
		{
			AppearEnemy();
			GameTimer_SetRemaining(&g_enemy_appear_count, ENEMY_APPEAR_INTERVAL);
			GameTimer_Resume(&g_enemy_appear_count);
		}
	}

	{
		if (IsKeyPressed(PAD_INPUT_2))
			RequestScene(SCENE_RESULT);

		GameController_Update(&g_player_ctrl);
		GameController_UpdateControl(&g_player_ctrl);

		if (IsKeyPressed(PAD_INPUT_1))
			ReloadPlayerBullet(3);
		if (IsKeyDown(PAD_INPUT_1))
			GrowPlayerBullet();
		if (IsKeyReleased(PAD_INPUT_1))
			ShotPlayerBullet(3);
	}

	{
		g_field.sprite.offset.x++;
		g_field.sprite.offset.y++;
		//g_field.sprite.angle += ToRadians(2);

		if (GameObject_IsAlive(&g_player))
			GameObject_UpdatePosition(&g_player);

		UpdatePlayerBullet();

		{
			int i;
			for (i = 0; i < Array_GetSize(&g_enemies); i++)
			{
				GameObject* enemy = Array_Get(&g_enemies, i);
				if (GameObject_IsAlive(enemy))
				{
					//GameObject_Enemy_Update(&g_enemies[i]);
					GameObject_UpdatePosition(enemy);
					GameSpriteAnimation_Update(&enemy->sprite.animation);

					if (GameTimer_IsPaused(&enemy->count) || GameTimer_IsFinished(&enemy->count))
					{
						ShotEnemyBullet(enemy);
						GameTimer_SetRemaining(&enemy->count, ENEMY_SHOOTING_INTERVAL);
						GameTimer_Resume(&enemy->count);
					}
				}
			}
		}

		UpdateEnemyBullet();
	}

	{
		{
			int i;
			for (i = 0; i < NUM_PLAYER_BULLETS; i++)
			{
				if (GameObject_IsAlive(&g_player_bullets[i]))
				{
					int j;
					for (j = 0; j < Array_GetSize(&g_enemies); j++)
					{
						GameObject* enemy = Array_Get(&g_enemies, j);
						if (GameObject_IsAlive(enemy))
							if (GameObject_IsHit(&g_player_bullets[i], enemy))
							{
								GameObject_Dispose(&g_player_bullets[i]);
								GameObject_Dispose(enemy);
							}
					}
				}
			}
		}

		if (GameObject_IsAlive(&g_player))
		{
			int i;
			for (i = 0; i < Array_GetSize(&g_enemies); i++)
			{
				GameObject* enemy = Array_Get(&g_enemies, i);
				if (GameObject_IsAlive(enemy))
				{
					if (GameObject_IsHit(enemy, &g_player))
					{
						//GameObject_Dispose(&g_player);
						DrawFormatString((int) GameObject_GetX(&g_field, LEFT, -10), (int) GameObject_GetY(&g_field, TOP, -(i + 1) * 10.f), COLOR_WHITE, "�������Ă���");
					}
					else
					{
						DrawFormatString((int) GameObject_GetX(&g_field, LEFT, -10), (int) GameObject_GetY(&g_field, TOP, -(i + 1) * 10.f), COLOR_WHITE, "�������Ă��Ȃ�");
					}
				}
			}
		}

		if (GameObject_IsAlive(&g_player))
		{
			GameObject_Field_CollisionVertical(&g_field, &g_player, CONNECTION_BARRIER, EDGESIDE_INNER);
			GameObject_Field_CollisionHorizontal(&g_field, &g_player, CONNECTION_BARRIER, EDGESIDE_INNER);
		}
		{
			int i;
			for (i = 0; i < NUM_PLAYER_BULLETS; i++)
			{
				if (GameObject_Field_CollisionVertical(&g_field, &g_player_bullets[i], CONNECTION_NONE, EDGESIDE_OUTER) ||
					GameObject_Field_CollisionHorizontal(&g_field, &g_player_bullets[i], CONNECTION_NONE, EDGESIDE_OUTER))
					GameObject_Dispose(&g_player_bullets[i]);
			}
		}
		{
			int i;
			for (i = 0; i < Array_GetSize(&g_enemies); i++)
			{
				GameObject* enemy = Array_Get(&g_enemies, i);
				if (GameObject_Field_CollisionVertical(&g_field, enemy, CONNECTION_NONE, EDGESIDE_OUTER) ||
					GameObject_Field_CollisionHorizontal(&g_field, enemy, CONNECTION_NONE, EDGESIDE_OUTER))
					GameObject_Dispose(enemy);
			}
		}
		{
			int i;
			for (i = 0; i < NUM_ENEMY_BULLETS; i++)
			{
				if (GameObject_Field_CollisionVertical(&g_field, &g_enemy_bullets[i], CONNECTION_NONE, EDGESIDE_OUTER) ||
					GameObject_Field_CollisionHorizontal(&g_field, &g_enemy_bullets[i], CONNECTION_NONE, EDGESIDE_OUTER))
					GameObject_Dispose(&g_enemy_bullets[i]);
			}
		}
	}
}

BOOL ReloadPlayerBullet(int n_way)
{
	GameObject* bullets[NUM_PLAYER_BULLETS];
	int num_bullets = 0;
	int i;

	for (i = 0; i < n_way; i++)
		bullets[i] = NULL;

	for (i = 0; i < NUM_PLAYER_BULLETS; i++)
	{
		if (!GameObject_IsAlive(&g_player_bullets[i]))
		{
			bullets[num_bullets++] = &g_player_bullets[i];
			if (num_bullets == n_way)
				break;
		}
	}

	if (num_bullets == n_way)
	{
		int i;
		for (i = 0; i < n_way; i++)
		{
			*bullets[i] = GameObject_Bullet_Create();
			bullets[i]->shape = SHAPE_CIRCLE;
			bullets[i]->sprite = GameSprite_Create(GameTexture_Create(g_resources.texture_bullet, Vec2_Create(), Vec2_Create(32, 32)));
			GameObject_Bullet_SetPosDefault(bullets[i], &g_player);
		}

		return TRUE;
	}

	return FALSE;
}

BOOL GrowPlayerBullet(void)
{
	int i;
	for (i = 0; i < NUM_PLAYER_BULLETS; i++)
	{
		if (g_player_bullets[i].state == 1)
		{
			GameObject_Bullet_Grow(&g_player_bullets[i]);
			GameObject_Bullet_SetPosDefault(&g_player_bullets[i], &g_player);
		}
	}

	return TRUE;
}

BOOL ShotPlayerBullet(int n_way)
{
	int num_shot = 0;
	int i;
	for (i = 0; i < NUM_PLAYER_BULLETS; i++)
	{
		if (GameObject_IsAlive(&g_player_bullets[i]) && g_player_bullets[i].state == 1)
		{
			GameObject_Bullet_SetVelDefault(&g_player_bullets[i], num_shot++, n_way);
			g_player_bullets[i].state = 2;
		}
	}

	return TRUE;
}

BOOL UpdatePlayerBullet(void)
{
	int i;
	for (i = 0; i < NUM_PLAYER_BULLETS; i++)
	{
		if (GameObject_IsAlive(&g_player_bullets[i]))
			GameObject_UpdatePosition(&g_player_bullets[i]);
	}

	return TRUE;
}

BOOL ShotEnemyBullet(const GameObject* enemy)
{
	int i;
	for (i = 0; i < NUM_ENEMY_BULLETS; i++)
	{
		if (!GameObject_IsAlive(&g_enemy_bullets[i]))
		{
			g_enemy_bullets[i] = GameObject_Bullet_Create();
			g_enemy_bullets[i].pos = enemy->pos;

			{
				float angle = Vec2_Angle(&Vec2_Sub(&g_player.pos, &enemy->pos));
				g_enemy_bullets[i].vel = Vec2_Create(cosf(angle) * 5, sinf(angle) * 5);
				GameTimer_SetRemaining(&g_enemy_bullets[i].count, 3.f);
				GameTimer_Resume(&g_enemy_bullets[i].count);
			}

			return TRUE;
		}
	}

	return FALSE;
}

BOOL UpdateEnemyBullet(void)
{
	int i;
	for (i = 0; i < NUM_ENEMY_BULLETS; i++)
	{
		if (GameObject_IsAlive(&g_enemy_bullets[i]))
		{
			GameObject_UpdatePosition(&g_enemy_bullets[i]);

			if (!GameTimer_IsFinished(&g_enemy_bullets[i].count))
			{
				float angle = Vec2_Angle(&g_enemy_bullets[i].vel);
				float direction = Vec2_Angle(&Vec2_Sub(&g_player.pos, &g_enemy_bullets[i].pos));

				float theta = GetLoopRangeF(direction - angle, -DX_PI_F, DX_PI_F);
				float angle_direction = angle + ClampF(theta, -ToRadians(2), ToRadians(2));

				g_enemy_bullets[i].vel = Vec2_Create(cosf(angle_direction) * BULLET_VEL, sinf(angle_direction) * BULLET_VEL);
			}
		}
	}

	return TRUE;
}

BOOL AppearEnemy(void)
{
	int i;
	for (i = 0; i < Array_GetSize(&g_enemies); i++)
	{
		GameObject* enemy = Array_Get(&g_enemies, i);
		if (!GameObject_IsAlive(enemy))
		{
			*enemy = GameObject_Enemy_Create();
			enemy->shape = SHAPE_CIRCLE;
			enemy->sprite = GameSprite_Create(GameTexture_Create(g_resources.texture_explosion, Vec2_Create(0, 0), Vec2_Create(64, 64)));
			enemy->sprite.animation = GameSpriteAnimation_Create(16, 4, 8);
			enemy->pos = Vec2_Create(GameObject_GetX(&g_field, CENTER_X), GameObject_GetY(&g_field, CENTER_Y));
			//GameObject_Enemy_SetPosDefault(&g_enemies[i], &g_field);
			//GameObject_Enemy_SetVelDefault(&g_enemies[i]);
			return TRUE;
		}
	}

	return FALSE;
}



//----------------------------------------------------------------------
//! @brief �Q�[���̕`�揈��
//!
//! @param[in] �Ȃ�
//!
//! @return �Ȃ�
//----------------------------------------------------------------------
void RenderPlay(void)
{
	GameObject_Render(&g_field);

	{
		int i;
		for (i = 0; i < NUM_PLAYER_BULLETS; i++)
		{
			if (GameObject_IsAlive(&g_player_bullets[i]))
				GameObject_Render(&g_player_bullets[i]);
		}
	}

	{
		int i;
		for (i = 0; i < Array_GetSize(&g_enemies); i++)
		{
			GameObject* enemy = Array_Get(&g_enemies, i);
			if (GameObject_IsAlive(enemy))
				GameObject_Render(enemy);
		}
	}

	{
		int i;
		for (i = 0; i < NUM_ENEMY_BULLETS; i++)
		{
			if (GameObject_IsAlive(&g_enemy_bullets[i]))
				GameObject_Render(&g_enemy_bullets[i]);
		}
	}

	if (GameObject_IsAlive(&g_player))
		GameObject_Render(&g_player);
}



//----------------------------------------------------------------------
//! @brief �Q�[���̏I������
//!
//! @param[in] �Ȃ�
//!
//! @return �Ȃ�
//----------------------------------------------------------------------
void FinalizePlay(void)
{
	GameResource_Delete(&g_resources);
}
