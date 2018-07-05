#include "SceneResult.h"
#include "SceneManager.h"
#include "InputManager.h"

// �O���[�o���ϐ��̒�` ====================================================

int g_result_count;

// �֐��̒�` ==============================================================

// ���U���g�V�[���̏���������
void InitializeResult(void)
{
	g_result_count = 0;
}

// ���U���g�V�[���̍X�V����
void UpdateResult(void)
{
	//if (g_result_count++ >= 180)
	if (IsKeyPressed(PAD_INPUT_2))
		RequestScene(SCENE_TITLE);
}

// ���U���g�V�[���̕`�揈��
void RenderResult(void)
{
	DrawFormatString(10, 10, COLOR_WHITE, "���U���g�V�[��");
	//DrawFormatString(10, 25, COLOR_WHITE, "�J�E���g: %3d / 180", g_result_count);
	DrawFormatString(10, 25, COLOR_WHITE, "PRESS X TO BACK TO TITLE");
}

// ���U���g�V�[���̏I������
void FinalizeResult(void)
{

}