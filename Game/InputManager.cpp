#include "InputManager.h"

// �O���[�o���ϐ��̒�` ====================================================

static int g_input_state;
static int s_input_state_last;

// �֐��̒�` ==============================================================

// �L�[�X�V
void UpdateInputManager(void)
{
	s_input_state_last = g_input_state;
	g_input_state = GetJoypadInputState(DX_INPUT_KEY_PAD1);
}

// �L�[��������Ă��邩
BOOL IsKeyDown(int key)
{
	return g_input_state & key;
}

// �L�[��������Ă��邩
BOOL IsKeyUp(int key)
{
	return !IsKeyDown(key);
}

// �L�[�����������ォ
BOOL IsKeyPressed(int key)
{
	return !(s_input_state_last & key) && (g_input_state & key);
}

// �L�[�𗣂������ォ
BOOL IsKeyReleased(int key)
{
	return (s_input_state_last & key) && !(g_input_state & key);
}
