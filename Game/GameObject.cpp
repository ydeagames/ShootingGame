#include "GameObject.h"
#include "GameUtils.h"
#include "GameMain.h"
#include <math.h>

// �萔�̒�` ==============================================================

// <�f�o�b�O�p�����蔻��\��>
#define DEBUG_HITBOX FALSE

// �֐��̐錾 ==============================================================

static BOOL GameObject_IsHitOvalPoint(GameObject* oval, Vec2* p);
static BOOL GameObject_IsHitOval(GameObject* obj1, GameObject* obj2);
static BOOL GameObject_IsHitCirclePoint(GameObject* oval, Vec2* p);
static BOOL GameObject_IsHitCircle(GameObject* obj1, GameObject* obj2);
static BOOL GameObject_IsHitBox(GameObject* obj1, GameObject* obj2);

// �֐��̒�` ==============================================================

// <<�e�N�X�`��>> ------------------------------------------------------

// <�e�N�X�`���쐬>
GameTexture GameTexture_Create(HGRP texture, Vec2 anchor, Vec2 size)
{
	return { texture, anchor, size, Vec2_Create(size.x / 2, size.y / 2) };
}

// <�e�N�X�`���Ȃ�>
GameTexture GameTexture_CreateNone()
{
	return GameTexture_Create(TEXTURE_NONE, Vec2_Create(), Vec2_Create());
}

// <<�X�v���C�g>> ------------------------------------------------------

// <�X�v���C�g�쐬>
GameSprite GameSprite_Create(GameTexture texture, float scale, float angle)
{
	return { COLOR_WHITE, texture, Vec2_Create(), scale, angle };
}

// <�X�v���C�g�Ȃ�>
GameSprite GameSprite_CreateNone()
{
	return GameSprite_Create(GameTexture_CreateNone(), 0, 0);
}

// <<�I�u�W�F�N�g>> ----------------------------------------------------

// <�I�u�W�F�N�g�쐬>
GameObject GameObject_Create(Vec2 pos, Vec2 vel, Vec2 size)
{
	return{ pos, vel, size, SHAPE_BOX, GameSprite_CreateNone(), TRUE, 1, 0 };
}

// <�I�u�W�F�N�g�폜>
void GameObject_Dispose(GameObject* obj)
{
	obj->alive = FALSE;
}

// <�I�u�W�F�N�g�m�F>
BOOL GameObject_IsAlive(GameObject* obj)
{
	return obj->alive;
}

// <�I�u�W�F�N�g���W�X�V>
void GameObject_UpdatePosition(GameObject* obj)
{
	obj->pos.x += obj->vel.x;
	obj->pos.y += obj->vel.y;
}

// <�I�u�W�F�N�gX�I�t�Z�b�g>
float GameObject_OffsetX(GameObject* obj, ObjectSide side, float pos, float padding)
{
	float offset = 0;
	switch (side)
	{
	case LEFT:
		offset = -(padding + obj->size.x / 2.f);
		break;
	case RIGHT:
		offset = (padding + obj->size.x / 2.f);
		break;
	}
	return pos + offset;
}

// <�I�u�W�F�N�gX�I�t�Z�b�g>
float GameObject_OffsetY(GameObject* obj, ObjectSide side, float pos, float padding)
{
	float offset = 0;
	switch (side)
	{
	case TOP:
		offset = -(padding + obj->size.y / 2.f);
		break;
	case BOTTOM:
		offset = (padding + obj->size.y / 2.f);
		break;
	}
	return pos + offset;
}

// <�I�u�W�F�N�gX�ʒu�Q�b�g>
float GameObject_GetX(GameObject* obj, ObjectSide side, float padding)
{
	return GameObject_OffsetX(obj, side, obj->pos.x, padding);
}

// <�I�u�W�F�N�gY�ʒu�Q�b�g>
float GameObject_GetY(GameObject* obj, ObjectSide side, float padding)
{
	return GameObject_OffsetY(obj, side, obj->pos.y, padding);
}

// <��`�I�u�W�F�N�g�~��`�I�u�W�F�N�g�����蔻��>
static BOOL GameObject_IsHitBox(GameObject* obj1, GameObject* obj2)
{
	return (
		GameObject_GetX(obj2, LEFT) < GameObject_GetX(obj1, RIGHT) &&
		GameObject_GetX(obj1, LEFT) < GameObject_GetX(obj2, RIGHT) &&
		GameObject_GetY(obj2, TOP) < GameObject_GetY(obj1, BOTTOM) &&
		GameObject_GetY(obj1, TOP) < GameObject_GetY(obj2, BOTTOM)
		);
}

// <�ȉ~�I�u�W�F�N�g�~�ȉ~�I�u�W�F�N�g�����蔻��> // �����g�p
static BOOL GameObject_IsHitOval(GameObject* obj1, GameObject* obj2)
{
	// STEP1 : obj2��P�ʉ~�ɂ���ϊ���obj1�Ɏ{��
	float nx = obj1->size.x / 2;
	float ny = obj1->size.y / 2;
	float px = obj2->size.x / 2;
	float py = obj2->size.y / 2;
	float ox = (obj2->pos.x - obj1->pos.x);
	float oy = (obj2->pos.y - obj1->pos.y);

	// STEP2 : ��ʎ��̎Z�o
	float rx_pow2 = 1 / (nx*nx);
	float ry_pow2 = 1 / (ny*ny);
	float ax = rx_pow2 * px*px;
	float ay = ry_pow2 * py*py;
	float bx = 2 * rx_pow2*px*ox;
	float by = 2 * ry_pow2*py*oy;
	float g = ox * ox*rx_pow2 + oy * oy*ry_pow2 - 1;

	// STEP3 : ���s�ړ��ʂ̎Z�o
	float tr_x = bx / (2 * ax);
	float tr_y = by / (2 * ay);

	// STEP4 : +1�ȉ~�����ɖ߂������œ����蔻��
	float kk = GetMinF(ax * tr_x*tr_x + ay * tr_y*tr_y - bx * tr_x - by * tr_y + g, 0);
	float ex = 1 + sqrtf(-kk / ax);
	float ey = 1 + sqrtf(-kk / ay);
	float JudgeValue = tr_x * tr_x / (ex*ex) + tr_y * tr_y / (ey*ey);

	return (JudgeValue <= 1);
}

// <�ȉ~�I�u�W�F�N�g�~�_�����蔻��> // �����g�p
static BOOL GameObject_IsHitOvalPoint(GameObject* oval, Vec2* p)
{
	// �_�ɑȉ~���^�~�ϊ��s���K�p
	float tx = p->x - oval->pos.x;
	float ty = p->y - oval->pos.y;
	float rx = oval->size.x / 2;
	float ry = oval->size.y / 2;

	// ���_����ړ���_�܂ł̋������Z�o
	return  (tx * tx + ty * ty * (rx / ry) * (rx / ry) <= rx * rx);
}

// <�~�I�u�W�F�N�g�~�~�I�u�W�F�N�g�����蔻��>
static BOOL GameObject_IsHitCircle(GameObject* obj1, GameObject* obj2)
{
	float r1 = GetMinF(obj1->size.x, obj1->size.y) / 2;
	float r2 = GetMinF(obj2->size.x, obj2->size.y) / 2;

	return Vec2_LengthSquaredTo(&obj1->pos, &obj2->pos) < (r1 + r2)*(r1 + r2);
}

// <�~�I�u�W�F�N�g�~�_�����蔻��>
static BOOL GameObject_IsHitCirclePoint(GameObject* circle, Vec2* p)
{
	float r1 = GetMinF(circle->size.x, circle->size.y) / 2;

	return Vec2_LengthSquaredTo(&circle->pos, p) < r1*r1;
}

// <�I�u�W�F�N�g�����蔻��>
BOOL GameObject_IsHit(GameObject* obj1, GameObject* obj2)
{
	if (obj1->shape == SHAPE_BOX && obj2->shape == SHAPE_BOX)
		return GameObject_IsHitBox(obj1, obj2);
	else if (obj1->shape == SHAPE_CIRCLE && obj2->shape == SHAPE_CIRCLE)
		return GameObject_IsHitBox(obj1, obj2) && GameObject_IsHitCircle(obj1, obj2);
	else
	{
		if (GameObject_IsHitBox(obj1, obj2))
		{
			if (obj1->shape == SHAPE_CIRCLE)
			{
				GameObject* tmp = obj1;
				obj1 = obj2;
				obj2 = tmp;
			}
			if (GameObject_GetX(obj1, LEFT) <= obj2->pos.x && obj2->pos.x <= GameObject_GetX(obj1, RIGHT))
				return obj1->size.y / 2 + obj2->size.y / 2 > GetAbsF(obj2->pos.y - obj1->pos.y);
			else if (GameObject_GetY(obj1, TOP) <= obj2->pos.y && obj2->pos.y <= GameObject_GetY(obj1, BOTTOM))
				return obj1->size.x / 2 + obj2->size.x / 2 > GetAbsF(obj2->pos.x - obj1->pos.x);
			else
			{
				return (
					GameObject_IsHitCirclePoint(obj2, &Vec2_Create(GameObject_GetX(obj1, LEFT), GameObject_GetY(obj1, TOP))) ||
					GameObject_IsHitCirclePoint(obj2, &Vec2_Create(GameObject_GetX(obj1, RIGHT), GameObject_GetY(obj1, TOP))) ||
					GameObject_IsHitCirclePoint(obj2, &Vec2_Create(GameObject_GetX(obj1, LEFT), GameObject_GetY(obj1, BOTTOM))) ||
					GameObject_IsHitCirclePoint(obj2, &Vec2_Create(GameObject_GetX(obj1, RIGHT), GameObject_GetY(obj1, BOTTOM)))
					);
			}
		}

		return FALSE;
	}
}

// <�I�u�W�F�N�g�`��>
void GameObject_Render(GameObject* obj)
{
	// �e�N�X�`�����m�F
	if (obj->sprite.texture.texture == TEXTURE_NONE)
	{
		switch (obj->shape)
		{
		default:
		case SHAPE_BOX:
			// ��`�`��
			if (DEBUG_HITBOX)
				DrawBoxAA(GameObject_GetX(obj, LEFT), GameObject_GetY(obj, TOP), GameObject_GetX(obj, RIGHT), GameObject_GetY(obj, BOTTOM), obj->sprite.color, FALSE, .5f);
			else
				DrawBoxAA(GameObject_GetX(obj, LEFT), GameObject_GetY(obj, TOP), GameObject_GetX(obj, RIGHT), GameObject_GetY(obj, BOTTOM), obj->sprite.color, TRUE);
			break;
		case SHAPE_CIRCLE:
		{
			float r1 = GetMinF(obj->size.x, obj->size.y) / 2;
			// �~
			if (DEBUG_HITBOX)
			{
				DrawCircleAA(GameObject_GetX(obj, CENTER_X), GameObject_GetY(obj, CENTER_Y), r1, 120, obj->sprite.color, FALSE, .5f);
				DrawBoxAA(GameObject_GetX(obj, LEFT), GameObject_GetY(obj, TOP), GameObject_GetX(obj, RIGHT), GameObject_GetY(obj, BOTTOM), obj->sprite.color, FALSE, .5f);
			}
			else
				DrawCircleAA(GameObject_GetX(obj, CENTER_X), GameObject_GetY(obj, CENTER_Y), r1, 120, obj->sprite.color, TRUE);
			break;
		}
		}
	}
	else
	{
		if (obj->sprite.texture.texture != TEXTURE_MISSING)
		{
			// �f�o�b�O�����蔻��g��\��
			if (DEBUG_HITBOX)
				switch (obj->shape)
				{
				default:
				case SHAPE_BOX:
					DrawBoxAA(GameObject_GetX(obj, LEFT), GameObject_GetY(obj, TOP), GameObject_GetX(obj, RIGHT), GameObject_GetY(obj, BOTTOM), obj->sprite.color, FALSE, .5f);
					break;
				case SHAPE_CIRCLE:
				{
					float r1 = GetMinF(obj->size.x, obj->size.y) / 2;
					DrawCircleAA(GameObject_GetX(obj, CENTER_X), GameObject_GetY(obj, CENTER_Y), r1, 120, obj->sprite.color, FALSE, .5f);
					DrawBoxAA(GameObject_GetX(obj, LEFT), GameObject_GetY(obj, TOP), GameObject_GetX(obj, RIGHT), GameObject_GetY(obj, BOTTOM), obj->sprite.color, FALSE, .5f);
					break;
				}
				}
			// �X�v���C�g�`��
			DrawRectRotaGraph2F(
				GameObject_GetX(obj, CENTER_X) + obj->sprite.offset.x, GameObject_GetY(obj, CENTER_Y) + obj->sprite.offset.y,
				(int)obj->sprite.texture.anchor.x, (int)obj->sprite.texture.anchor.y,
				(int)obj->sprite.texture.size.x, (int)obj->sprite.texture.size.y,
				obj->sprite.texture.center.x, obj->sprite.texture.center.y,
				(double)obj->sprite.scale,
				(double)obj->sprite.angle,
				obj->sprite.texture.texture,
				TRUE
			);
		}
		else
		{
			// NULL�e�N�X�`����\��
			DrawBoxAA(GameObject_GetX(obj, LEFT), GameObject_GetY(obj, TOP), GameObject_GetX(obj, RIGHT), GameObject_GetY(obj, BOTTOM), COLOR_BLACK, TRUE);
			DrawBoxAA(GameObject_GetX(obj, LEFT), GameObject_GetY(obj, TOP), GameObject_GetX(obj, CENTER_X), GameObject_GetY(obj, CENTER_Y), COLOR_FUCHSIA, TRUE);
			DrawBoxAA(GameObject_GetX(obj, CENTER_X), GameObject_GetY(obj, CENTER_Y), GameObject_GetX(obj, RIGHT), GameObject_GetY(obj, BOTTOM), COLOR_FUCHSIA, TRUE);
			DrawBoxAA(GameObject_GetX(obj, LEFT), GameObject_GetY(obj, TOP), GameObject_GetX(obj, RIGHT), GameObject_GetY(obj, BOTTOM), obj->sprite.color, FALSE, .5f);
		}
	}
}

// <<�t�B�[���h�I�u�W�F�N�g>> ------------------------------------------

// <�t�B�[���h�I�u�W�F�N�g�쐬>
GameObject GameObject_Field_Create(void)
{
	return GameObject_Create(Vec2_Create(SCREEN_CENTER_X, SCREEN_CENTER_Y), Vec2_Create(), Vec2_Create(SCREEN_WIDTH, SCREEN_HEIGHT));
}

// <�t�B�[���h�㉺�Փˏ���>
ObjectSide GameObject_Field_CollisionVertical(GameObject* field, GameObject* obj, ObjectConnection connection, ObjectEdgeSide edge)
{
	// �q�b�g�T�C�h
	ObjectSide side_hit = NONE;

	// �R�E���E�㉺�Ǔ����蔻��
	{
		// ���ɉ����ăp�f�B���O�𒲐�
		float padding_top = GameObject_GetY(field, TOP);
		float padding_bottom = GameObject_GetY(field, BOTTOM);
		switch (edge)
		{
		case EDGESIDE_INNER:
			padding_top = GameObject_OffsetY(obj, BOTTOM, padding_top);
			padding_bottom = GameObject_OffsetY(obj, TOP, padding_bottom);
			break;
		case EDGESIDE_OUTER:
			padding_top = GameObject_OffsetY(obj, TOP, padding_top);
			padding_bottom = GameObject_OffsetY(obj, BOTTOM, padding_bottom);
			break;
		}

		// �����蔻��
		if (obj->pos.y < padding_top)
			side_hit = TOP;
		else if (padding_bottom <= obj->pos.y)
			side_hit = BOTTOM;

		// �t�B�[���h�̂Ȃ���
		switch (connection)
		{
		case CONNECTION_BARRIER:
			// �ǂɂ��������璲��
			obj->pos.y = ClampF(obj->pos.y, padding_top, padding_bottom);
			break;
		case CONNECTION_LOOP:
			// �ǂɂ��������烋�[�v
			obj->pos.y = GetLoopRangeF(obj->pos.y, padding_top, padding_bottom);
			break;
		}
	}

	return side_hit;
}

// <�t�B�[���h���E�Փˏ���>
ObjectSide GameObject_Field_CollisionHorizontal(GameObject* field, GameObject* obj, ObjectConnection connection, ObjectEdgeSide edge)
{
	// �q�b�g�T�C�h
	ObjectSide side_hit = NONE;

	// �R�E���E���E�Ǔ����蔻��
	{
		// ���ɉ����ăp�f�B���O�𒲐�
		float padding_left = GameObject_GetX(field, LEFT);
		float padding_right = GameObject_GetX(field, RIGHT);
		switch (edge)
		{
		case EDGESIDE_INNER:
			padding_left = GameObject_OffsetX(obj, RIGHT, padding_left);
			padding_right = GameObject_OffsetX(obj, LEFT, padding_right);
			break;
		case EDGESIDE_OUTER:
			padding_left = GameObject_OffsetX(obj, LEFT, padding_left);
			padding_right = GameObject_OffsetX(obj, RIGHT, padding_right);
			break;
		}

		// �����蔻��
		if (obj->pos.x < padding_left)
			side_hit = LEFT;
		else if (padding_right <= obj->pos.x)
			side_hit = RIGHT;

		// �t�B�[���h�̂Ȃ���
		switch (connection)
		{
		case CONNECTION_BARRIER:
			// �ǂɂ��������璲��
			obj->pos.x = ClampF(obj->pos.x, padding_left, padding_right);
			break;
		case CONNECTION_LOOP:
			// �ǂɂ��������烋�[�v
			obj->pos.x = GetLoopRangeF(obj->pos.x, padding_left, padding_right);
			break;
		}
	}

	return side_hit;
}

// <�t�B�[���h�`��>
void GameObject_Field_Render(GameObject* field)
{
}
