#include "SceneTitle.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "GameMain.h"
#include "GameObject.h"
#include "GameUtils.h"
#include "Easings.h"

// �萔�̒�` ==============================================================

#define FONT_DESC_SIZE 72

// �^�C�g���e�N�X�`��
#define TEXTURE_RESULT "Resources\\Textures\\Result\\result.png"
#define TEXTURE_TEXT "Resources\\Textures\\Result\\press_button_back.png"

// �O���[�o���ϐ��̒�` ====================================================

static int g_count;

static HFNT g_font_desc;

static HGRP g_texture_result;
static HGRP g_texture_text;


// �֐��̒�` ==============================================================

// �^�C�g���V�[���̏���������
void InitializeResult(void)
{
	g_count = 0;

	// �t�H���g
	g_font_desc = CreateFontToHandle("HGP�n�p�p�޼��UB", FONT_DESC_SIZE, DX_FONTTYPE_ANTIALIASING_4X4);

	// �e�N�X�`����ǂݍ���
	g_texture_result = LoadGraph(TEXTURE_RESULT);
	g_texture_text = LoadGraph(TEXTURE_TEXT);
}

// �^�C�g���V�[���̍X�V����
void UpdateResult(void)
{
	g_count++;

	if (IsKeyPressed(PAD_INPUT_2))
		RequestScene(SCENE_TITLE);
}

// �^�C�g���V�[���̕`�揈��
void RenderResult(void)
{
	int bright = (int)GetEasingValue(ESG_INCUBIC, GetPercentageRange((float)g_count, 60 * 0, 60 * 0.5f), 255);		// �t�F�[�h�C��
	SetDrawBright(bright, bright, bright);
	{
		DrawRotaGraph(SCREEN_CENTER_X, SCREEN_CENTER_Y, 1, 0, g_texture_result, TRUE);
		{
			int duration = 90;
			int idle = 30;

			DrawRotaGraph(SCREEN_CENTER_X, SCREEN_CENTER_Y + 96, 1, 0, g_texture_text, TRUE);
			{
				SetDrawBlendMode(DX_BLENDMODE_ALPHA,
					(int)GetEasingValueRange(ESG_INCIRC, GetPercentageRange((float)(g_count % (duration + idle)), 0, (float)duration), 255, 32));
				DrawRotaGraph(SCREEN_CENTER_X, SCREEN_CENTER_Y + 96,
					GetEasingValueRange(ESG_LINEAR, GetPercentageRange((float)(g_count % (duration + idle)), 0, (float)duration), 1.1f, 1.f),
					0, g_texture_text, TRUE
				);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
		}

		//DrawFormatString(10, 10, COLOR_WHITE, "�^�C�g���V�[��");
		//DrawFormatString(10, 25, COLOR_WHITE, "�J�E���g: %3d / 180", g_result_count);

		{
			GameObject box = GameObject_Field_Create();
			box.sprite.color = COLOR_BLACK;
			box.pos.y += 5;
			box.size.x -= 100;
			box.size.y = 110;
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 64);
			GameObject_Render(&box);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}

		DrawFormatStringToHandle(SCREEN_LEFT + 70, SCREEN_CENTER_Y - 35 + 20 * 0, COLOR_WHITE, g_font_desc, g_cleared ? "�N���A!" : "Game Over");
	}
	SetDrawBright(255, 255, 255);
}

// �^�C�g���V�[���̏I������
void FinalizeResult(void)
{
	DeleteFontToHandle(g_font_desc);

	// �e�N�X�`���A�����[�h
	DeleteGraph(g_texture_result);
}