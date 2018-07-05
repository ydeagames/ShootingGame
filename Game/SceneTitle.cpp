#include "SceneTitle.h"
#include "SceneManager.h"
#include "InputManager.h"

// �O���[�o���ϐ��̒�` ====================================================

int g_title_count;

// �֐��̒�` ==============================================================

// �^�C�g���V�[���̏���������
void InitializeTitle(void)
{
	g_title_count = 0;
}

// �^�C�g���V�[���̍X�V����
void UpdateTitle(void)
{
	//if (g_title_count++ >= 180)
	//if (IsButtonDown(PAD_INPUT_2))
	if (IsKeyPressed(PAD_INPUT_2))
		RequestScene(SCENE_PLAY);
}

// �^�C�g���V�[���̕`�揈��
void RenderTitle(void)
{
	DrawFormatString(10, 10, COLOR_WHITE, "�^�C�g���V�[��");
	DrawFormatString(10, 25, COLOR_WHITE, "PRESS X TO START");
	//DrawFormatString(10, 25, COLOR_WHITE, "�J�E���g: %3d / 180", g_title_count);
}

// �^�C�g���V�[���̏I������
void FinalizeTitle(void)
{

}