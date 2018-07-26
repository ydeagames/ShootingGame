#include "GameObjects.h"
#include "GameMain.h"
#include "GameUtils.h"
#include <math.h>

// �萔�̒�` ==============================================================

// <�G>
#define ENEMY_WIDTH 32
#define ENEMY_HEIGHT 32

// <�v���C���[>
#define PLAYER_WIDTH  42
#define PLAYER_HEIGHT 36

// <�e>
#define BULLET_WIDTH  6
#define BULLET_HEIGHT 6
#define BULLET_GROW_SPEED .1f

// �֐��̒�` ==============================================================

// <<�G�I�u�W�F�N�g>> ----------------------------------------------

// <�G�I�u�W�F�N�g�쐬>
GameObject GameObject_Enemy_Create(void)
{
	GameObject obj = GameObject_Create(Vec2_Create(), Vec2_Create(), Vec2_Create(ENEMY_WIDTH, ENEMY_HEIGHT));
	obj.type = GetRand(1) ? TYPE_ENEMY1 : TYPE_ENEMY2;
	return obj;
}

// <�G�I�u�W�F�N�g���WX�f�t�H���g>
void GameObject_Enemy_SetPosDefault(GameObject* obj, const GameObject* field)
{
	obj->pos.x = GetRandRangeF(GameObject_GetX(field, LEFT), GameObject_GetX(field, RIGHT));
	obj->pos.y = GameObject_GetY(field, TOP, 20);
}

// <�G�I�u�W�F�N�g�ړ�X�f�t�H���g>
void GameObject_Enemy_SetVelDefault(GameObject* obj)
{
	obj->vel = Vec2_Create(0, 5);
}

// <�G�I�u�W�F�N�g�X�V>
void GameObject_Enemy_Update(GameObject* obj)
{
	if (obj->type == TYPE_ENEMY2)
	{
		float radius = 15;
		obj->vel.x = cosf(obj->pos.y / radius) * radius;
	}
}

// <<�v���C���[�I�u�W�F�N�g>> ----------------------------------------------

// <�v���C���[�I�u�W�F�N�g�쐬>
GameObject GameObject_Player_Create(void)
{
	GameObject obj = GameObject_Create(Vec2_Create(), Vec2_Create(), Vec2_Create(PLAYER_WIDTH, PLAYER_HEIGHT));
	obj.type = TYPE_PLAYER;
	return obj;
}

// <�v���C���[�I�u�W�F�N�g���WY�f�t�H���g>
void GameObject_Player_SetPosYDefault(GameObject* obj, const GameObject* field)
{
	obj->pos.y = GameObject_OffsetY(obj, TOP, GameObject_GetY(field, BOTTOM), 16);
}

// <<�e�I�u�W�F�N�g>> ----------------------------------------------

// <�e�I�u�W�F�N�g�쐬>
GameObject GameObject_Bullet_Create(void)
{
	GameObject obj = GameObject_Create(Vec2_Create(), Vec2_Create(), Vec2_Create(BULLET_WIDTH, BULLET_HEIGHT));
	obj.type = TYPE_PLAYER_BULLET;
	return obj;
}

// <�e�I�u�W�F�N�g���W�f�t�H���g>
void GameObject_Bullet_SetPosDefault(GameObject* obj, const GameObject* player)
{
	obj->pos = player->pos;
	//obj->pos.y = GameObject_OffsetY(obj, BOTTOM, GameObject_GetY(player, BOTTOM));
}

// <�e�I�u�W�F�N�g�ړ��f�t�H���g>
void GameObject_Bullet_SetVelDefault(GameObject* obj, float offset_angle, int num, int total)
{
	int n = 270 - (total - 1) * 10 + num * 20;
	float angle = ToRadians((float)n);
	obj->vel.x = BULLET_VEL * cosf(offset_angle + angle);
	obj->vel.y = BULLET_VEL * sinf(offset_angle + angle);
}

// <�e�I�u�W�F�N�g�T�C�Y�ύX>
void GameObject_Bullet_SetSize(GameObject* obj, float scale)
{
	obj->sprite.scale = scale;
	obj->size = Vec2_Create(BULLET_WIDTH * scale, BULLET_HEIGHT * scale);
}

// <�e�I�u�W�F�N�g����>
void GameObject_Bullet_Grow(GameObject* obj)
{
	GameObject_Bullet_SetSize(obj, obj->sprite.scale + BULLET_GROW_SPEED);
}
