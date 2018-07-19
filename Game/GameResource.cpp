#include "GameResource.h"
#include "GameObject.h"

// �萔�̒�` ==============================================================

// <�t�H���g> ----------------------------------------------------------
#define FONT_FILE_MAIN_CUSTOM "Resources\\Fonts\\TheSlavicFont-Regular.ttf"		// �J�X�^���t�H���g�t�@�C��
#define FONT_NAME_MAIN_CUSTOM "The Slavic Font"									// �J�X�^���t�H���g�l�[��
#define FONT_NAME_SPACERACE "HGP�n�p�p�޼��UB"											// �t�H�[���o�b�N�p�t�H���g�l�[��
#define FONT_NAME_MENU "HGP�n�p�p�޼��UB"

// <�T�E���h> ----------------------------------------------------------

// <�e�N�X�`��> --------------------------------------------------------
#define TEXTURE_PLAYER_PROTECTED "Resources\\Textures\\Protected\\Solvalou.png"		// ���\�[�X�ی�e�N�X�`��
#define TEXTURE_PLAYER "Resources\\Textures\\Solvalou.png"							// �I�[�v���e�N�X�`��
#define TEXTURE_BULLET_PROTECTED "Resources\\Textures\\Protected\\Bullet.png"		// ���\�[�X�ی�e�N�X�`��
#define TEXTURE_BULLET "Resources\\Textures\\Bullet.png"							// �I�[�v���e�N�X�`��
#define TEXTURE_ENEMY_PROTECTED "Resources\\Textures\\Protected\\Enemy.png"			// ���\�[�X�ی�e�N�X�`��
#define TEXTURE_ENEMY "Resources\\Textures\\Enemy.png"								// �I�[�v���e�N�X�`��
#define TEXTURE_EXPLOSION_PROTECTED "Resources\\Textures\\Protected\\Explosion.png"	// ���\�[�X�ی�e�N�X�`��
#define TEXTURE_EXPLOSION "Resources\\Textures\\Explosion.png"						// �I�[�v���e�N�X�`��
#define TEXTURE_MAP_PROTECTED "Resources\\Textures\\Protected\\Xevious_BG_map.png"	// ���\�[�X�ی�e�N�X�`��
#define TEXTURE_MAP "Resources\\Textures\\Xevious_BG_map.png"						// �I�[�v���e�N�X�`��

// <����> --------------------------------------------------------------
#define MOVIE_LOGO "Resources\\Movies\\ydeagames.avi"								// ���S���[�r�[

// �֐��̒�` ==============================================================

// <<���\�[�X>> --------------------------------------------------

// <���\�[�X�쐬�ǂݍ���>
GameResource GameResource_Create(void)
{
	GameResource res;

	// �t�H���g
	// �J�X�^���t�H���g�t�@�C�����ǂݍ��߂���g��
	if (AddFontResourceEx(FONT_FILE_MAIN_CUSTOM, FR_PRIVATE, NULL) > 0)
		res.font_main = CreateFontToHandle(FONT_NAME_MAIN_CUSTOM, FONT_SIZE_MAIN, 3, DX_FONTTYPE_ANTIALIASING_4X4);
	else
		res.font_main = CreateFontToHandle(FONT_NAME_SPACERACE, FONT_SIZE_MAIN, 3, DX_FONTTYPE_ANTIALIASING_4X4);
	res.font_menu = CreateFontToHandle(FONT_NAME_MENU, FONT_SIZE_MENU, 3, DX_FONTTYPE_ANTIALIASING_4X4);

	// �T�E���h

	// �e�N�X�`��
	res.texture_player = LoadGraph(TEXTURE_PLAYER_PROTECTED);
	if (res.texture_player == TEXTURE_MISSING)										// ���\�[�X�ی�e�N�X�`��������������
		res.texture_player = LoadGraph(TEXTURE_PLAYER);								// �I�[�v���e�N�X�`����ǂݍ���
	res.texture_bullet = LoadGraph(TEXTURE_BULLET_PROTECTED);
	if (res.texture_bullet == TEXTURE_MISSING)										// ���\�[�X�ی�e�N�X�`��������������
		res.texture_bullet = LoadGraph(TEXTURE_BULLET);								// �I�[�v���e�N�X�`����ǂݍ���
	res.texture_enemy = LoadGraph(TEXTURE_ENEMY_PROTECTED);
	if (res.texture_enemy == TEXTURE_MISSING)										// ���\�[�X�ی�e�N�X�`��������������
		res.texture_enemy = LoadGraph(TEXTURE_ENEMY);								// �I�[�v���e�N�X�`����ǂݍ���
	res.texture_explosion = LoadGraph(TEXTURE_EXPLOSION_PROTECTED);
	if (res.texture_explosion == TEXTURE_MISSING)									// ���\�[�X�ی�e�N�X�`��������������
		res.texture_explosion = LoadGraph(TEXTURE_EXPLOSION);						// �I�[�v���e�N�X�`����ǂݍ���
	res.texture_map = LoadGraph(TEXTURE_MAP_PROTECTED);
	if (res.texture_map == TEXTURE_MISSING)											// ���\�[�X�ی�e�N�X�`��������������
		res.texture_map = LoadGraph(TEXTURE_MAP);									// �I�[�v���e�N�X�`����ǂݍ���

	// ����
	res.movie_logo = LoadGraph(MOVIE_LOGO);											// �I�[�v���e�N�X�`����ǂݍ���

	return res;
}

// <���\�[�X�J��>
void GameResource_Delete(GameResource* res)
{
	// �t�H���g
	DeleteFontToHandle(res->font_main);
	RemoveFontResourceEx(FONT_FILE_MAIN_CUSTOM, FR_PRIVATE, NULL);

	// �T�E���h

	// �e�N�X�`��
	DeleteGraph(res->texture_player);
	DeleteGraph(res->texture_bullet);
	DeleteGraph(res->texture_enemy);
	DeleteGraph(res->texture_explosion);
	DeleteGraph(res->texture_map);

	// ����
	DeleteGraph(res->movie_logo);
}
