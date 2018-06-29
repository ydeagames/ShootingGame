#pragma once
#include "GameObject.h"

// �萔�̒�` ==============================================================

// <�v���C���[>
#define PLAYER_VEL 4.f		// �v���C���[�̑��x
// <�G>
#define ENEMY_VEL 2			// �G�̑��x

// �֐��̐錾 ==============================================================

// <<�G�I�u�W�F�N�g>> ----------------------------------------------

// <�G�I�u�W�F�N�g�쐬>
GameObject GameObject_Enemy_Create(Vec2 pos);

// <�G�I�u�W�F�N�g���WX�f�t�H���g>
void GameObject_Enemy_SetPosXDefault(GameObject* obj, GameObject* field);

// <�G�I�u�W�F�N�g���WY�f�t�H���g>
void GameObject_Enemy_SetPosYDefault(GameObject* obj, GameObject* field);

// <�G�I�u�W�F�N�g�ړ�X�f�t�H���g>
void GameObject_Enemy_SetVelXDefault(GameObject* obj);

// <<�v���C���[�I�u�W�F�N�g>> ----------------------------------------------

// <�v���C���[�I�u�W�F�N�g�쐬>
GameObject GameObject_Player_Create(void);

// <�v���C���[�I�u�W�F�N�g���WY�f�t�H���g>
void GameObject_Player_SetPosYDefault(GameObject* obj, GameObject* field);
