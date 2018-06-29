//__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/
//! @file   GameMain.cpp
//!
//! @brief  �Q�[���֘A�̃\�[�X�t�@�C��
//!
//! @date   ���t
//!
//! @author ����Җ�
//__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/

// �w�b�_�t�@�C���̓ǂݍ��� ================================================

#include "GameMain.h"
#include "GameObjects.h"
#include "GameController.h"
#include "GameResource.h"
#include "InputManager.h"



// �萔�̒�` ==============================================================




// �O���[�o���ϐ��̒�` ====================================================

GameResource g_resources;

GameObject g_field;
GameObject g_player;
GameController g_player_ctrl;

GameObject g_player_bullet;




// �֐��̐錾 ==============================================================

void InitializeGame(void);  // �Q�[���̏���������
void UpdateGame(void);      // �Q�[���̍X�V����
void RenderGame(void);      // �Q�[���̕`�揈��
void FinalizeGame(void);    // �Q�[���̏I������




// �֐��̒�` ==============================================================

//----------------------------------------------------------------------
//! @brief �Q�[���̏���������
//!
//! @param[in] �Ȃ�
//!
//! @return �Ȃ�
//----------------------------------------------------------------------
void InitializeGame(void)
{
	g_resources = GameResource_Create();

	g_field = GameObject_Field_Create();

	g_player = GameObject_Player_Create();
	g_player.sprite = GameSprite_Create(GameTexture_Create(g_resources.texture_player, Vec2_Create(), Vec2_Create(32, 32)), 1.5f);
	g_player.sprite.offset.y -= 7;
	g_player.pos = g_field.pos;

	g_player_ctrl = GameController_Player_Create(&g_player, PlayerKeySet_Default_Create());
}



//----------------------------------------------------------------------
//! @brief �Q�[���̍X�V����
//!
//! @param[in] �Ȃ�
//!
//! @return �Ȃ�
//----------------------------------------------------------------------
void UpdateGame(void)
{
	UpdateInputManager();

	GameController_Update(&g_player_ctrl);
	GameController_UpdateControl(&g_player_ctrl);
	if (!GameObject_IsAlive(&g_player_bullet)&&IsKeyDown(PAD_INPUT_1))
	{
		g_player_bullet = GameObject_Bullet_Create();
		g_player_bullet.sprite = GameSprite_Create(GameTexture_Create(g_resources.texture_bullet, Vec2_Create(), Vec2_Create(32, 32)));
		GameObject_Bullet_SetPosDefault(&g_player_bullet, &g_player);
		GameObject_Bullet_SetVelDefault(&g_player_bullet);
	}

	GameObject_UpdatePosition(&g_player);
	if (GameObject_IsAlive(&g_player_bullet))
		GameObject_UpdatePosition(&g_player_bullet);

	GameObject_Field_CollisionVertical(&g_field, &g_player, CONNECTION_BARRIER, EDGESIDE_INNER);
	GameObject_Field_CollisionHorizontal(&g_field, &g_player, CONNECTION_BARRIER, EDGESIDE_INNER);

	if (GameObject_Field_CollisionVertical(&g_field, &g_player_bullet, CONNECTION_NONE, EDGESIDE_OUTER) ||
		GameObject_Field_CollisionHorizontal(&g_field, &g_player_bullet, CONNECTION_NONE, EDGESIDE_OUTER))
		GameObject_Dispose(&g_player_bullet);
}



//----------------------------------------------------------------------
//! @brief �Q�[���̕`�揈��
//!
//! @param[in] �Ȃ�
//!
//! @return �Ȃ�
//----------------------------------------------------------------------
void RenderGame(void)
{
	GameObject_Render(&g_player);
	if (GameObject_IsAlive(&g_player_bullet))
		GameObject_Render(&g_player_bullet);
}



//----------------------------------------------------------------------
//! @brief �Q�[���̏I������
//!
//! @param[in] �Ȃ�
//!
//! @return �Ȃ�
//----------------------------------------------------------------------
void FinalizeGame(void)
{
	GameResource_Delete(&g_resources);
}
