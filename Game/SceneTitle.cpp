#include "SceneTitle.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "GameMain.h"
#include "GameObject.h"
#include "GameUtils.h"

// �萔�̒�` ==============================================================

#define FONT_DESC_SIZE 16

// �^�C�g���e�N�X�`��
#define TEXTURE_TITLE "Resources\\Textures\\Title\\title.png"
#define TEXTURE_TEXT "Resources\\Textures\\Title\\press_button.png"

// �O���[�o���ϐ��̒�` ====================================================

static int g_count;

static HFNT g_font_desc;

static HGRP g_texture_title;
static HGRP g_texture_text;


// �֐��̒�` ==============================================================

// �^�C�g���V�[���̏���������
void InitializeTitle(void)
{
	g_count = 0;

	// �t�H���g
	g_font_desc = CreateFontToHandle("HGP�n�p�p�޼��UB", FONT_DESC_SIZE, DX_FONTTYPE_ANTIALIASING_4X4);

	// �e�N�X�`����ǂݍ���
	g_texture_title = LoadGraph(TEXTURE_TITLE);
	g_texture_text = LoadGraph(TEXTURE_TEXT);
}

// �^�C�g���V�[���̍X�V����
void UpdateTitle(void)
{
	g_count++;

	if (IsKeyPressed(PAD_INPUT_2))
		RequestScene(SCENE_PLAY);
}

// �^�C�g���V�[���̕`�揈��
void RenderTitle(void)
{
	int bright = (int)GetEasingValue(ESG_INCUBIC, GetPercentageRange((float)g_count, 60 * 0, 60 * 0.5f), 255);		// �t�F�[�h�C��
	SetDrawBright(bright, bright, bright);
	{
		DrawRotaGraph(SCREEN_CENTER_X, SCREEN_CENTER_Y, 1, 0, g_texture_title, TRUE);
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
		//DrawFormatString(10, 25, COLOR_WHITE, "�J�E���g: %3d / 180", g_title_count);

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

		DrawFormatStringToHandle(SCREEN_LEFT + 70, SCREEN_CENTER_Y - 35 + 20 * 0, COLOR_WHITE, g_font_desc, "�@�}�E�X��WASD�L�[�ő��삵�܂��B");
		DrawFormatStringToHandle(SCREEN_LEFT + 70, SCREEN_CENTER_Y - 35 + 20 * 1, COLOR_WHITE, g_font_desc, "�A�e���������񂠂āA�l���̓G��|���܂��B");
		DrawFormatStringToHandle(SCREEN_LEFT + 70, SCREEN_CENTER_Y - 35 + 20 * 2, COLOR_WHITE, g_font_desc, "�B�l���̓G����G�������܂�");
		DrawFormatStringToHandle(SCREEN_LEFT + 70, SCREEN_CENTER_Y - 35 + 20 * 3, COLOR_WHITE, g_font_desc, "�C�N���b�N���Ēe���Ԃ��悤");
	}
	SetDrawBright(255, 255, 255);
}

// �^�C�g���V�[���̏I������
void FinalizeTitle(void)
{
	DeleteFontToHandle(g_font_desc);

	// �e�N�X�`���A�����[�h
	DeleteGraph(g_texture_title);
}