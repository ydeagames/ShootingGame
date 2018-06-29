#include "GameObjects.h"
#include "GameMain.h"
#include "GameUtils.h"

// �萔�̒�` ==============================================================

// <�G>
#define ENEMY_WIDTH 32
#define ENEMY_HEIGHT 32

// <�v���C���[>
#define PLAYER_WIDTH  48
#define PLAYER_HEIGHT 48

// �֐��̒�` ==============================================================

// <<�G�I�u�W�F�N�g>> ----------------------------------------------

// <�G�I�u�W�F�N�g�쐬>
GameObject GameObject_Enemy_Create(Vec2 vec)
{
	GameObject obj = GameObject_Create(vec, Vec2_Create(), Vec2_Create(ENEMY_WIDTH, ENEMY_HEIGHT));
	obj.shape = SHAPE_CIRCLE;
	return obj;
}

// <�G�I�u�W�F�N�g���WX�f�t�H���g>
void GameObject_Enemy_SetPosXDefault(GameObject* obj, GameObject* field)
{
	obj->pos.x = GetRandRangeF(GameObject_GetX(field, LEFT), GameObject_GetX(field, RIGHT));
}

// <�G�I�u�W�F�N�g���WY�f�t�H���g>
void GameObject_Enemy_SetPosYDefault(GameObject* obj, GameObject* field)
{
	obj->pos.y = GetRandRangeF(GameObject_GetY(field, TOP), GameObject_GetY(field, BOTTOM, -85));
}

// <�G�I�u�W�F�N�g�ړ�X�f�t�H���g>
void GameObject_Enemy_SetVelXDefault(GameObject* obj)
{
	obj->vel.x = (float)(GetRand(1) * 2 - 1)*ENEMY_VEL;
}

// <<�v���C���[�I�u�W�F�N�g>> ----------------------------------------------

// <�v���C���[�I�u�W�F�N�g�쐬>
GameObject GameObject_Player_Create(void)
{
	return GameObject_Create(Vec2_Create(), Vec2_Create(), Vec2_Create(PLAYER_WIDTH, PLAYER_HEIGHT));
}

// <�v���C���[�I�u�W�F�N�g���WY�f�t�H���g>
void GameObject_Player_SetPosYDefault(GameObject* obj, GameObject* field)
{
	obj->pos.y = GameObject_OffsetY(obj, TOP, GameObject_GetY(field, BOTTOM), 16);
}
