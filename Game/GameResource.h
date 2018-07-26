#pragma once

// �\���̂̐錾 ============================================================

// <�Q�[�����\�[�X>
typedef struct
{
	// <�t�H���g>
	HFNT font_main;
	HFNT font_menu;

	// <�T�E���h>

	// <�e�N�X�`��>
	HGRP texture_player;
	HGRP texture_bullet;
	HGRP texture_enemy;
	HGRP texture_explosion;
	HGRP texture_map;
	HGRP texture_cloud;

	// <����>
	HGRP movie_logo;
} GameResource;

// �萔�̒�` ==============================================================

// <�t�H���g> ----------------------------------------------------------

#define FONT_SIZE_MAIN 80			// �t�H���g�T�C�Y
#define FONT_SIZE_MENU 20

// �֐��̐錾 ==============================================================

// <<���\�[�X>> --------------------------------------------------

// <���\�[�X�쐬�ǂݍ���>
GameResource GameResource_Create(void);

// <���\�[�X�J��>
void GameResource_Delete(GameResource* res);
