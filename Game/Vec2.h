#pragma once

// �\���̂̐錾 ============================================================

// <�x�N�g��>
typedef struct
{
	float x;	// X���W
	float y;	// Y���W
} Vec2;

// �֐��̐錾 ==============================================================

// <�x�N�g���쐬>
Vec2 Vec2_Create(float x = 0.f, float y = 0.f);

// <�x�N�g���̒���>
float Vec2_Length(Vec2* vec);

// <�x�N�g���̒����̓��>
float Vec2_LengthSquared(Vec2* vec);

// <��������̃x�N�g���Ƃ̓���>
float Vec2_Dot(Vec2* vec, Vec2* other);

// <��������̃x�N�g���Ƃ̋���>
float Vec2_LengthTo(Vec2* vec, Vec2* other);

// <��������̃x�N�g���Ƃ̋����̓��>
float Vec2_LengthSquaredTo(Vec2* vec, Vec2* other);

// <���K���i������1�ɂ����j�x�N�g��>
Vec2 Vec2_Normalized(Vec2* vec);

// <���l�̃x�N�g����>
BOOL Vec2_EqualsEpsilon(Vec2* vec, Vec2* other, float epsilon);

// <���l�̃x�N�g����>
BOOL Vec2_Equals(Vec2* vec, Vec2* other);

// <0�x�N�g����>
BOOL Vec2_IsZero(Vec2* vec);

// <�x�N�g���̊p�x>
float Vec2_Angle(Vec2* vec);

// <�x�N�g���𕪉�>
void Vec2_Decompose(Vec2* vec, Vec2* angle, Vec2* vec_a, Vec2* vec_b);

// <�x�N�g�������Z>
Vec2 Vec2_Add(Vec2* vec_a, Vec2* vec_b);

// <�x�N�g�������Z>
Vec2 Vec2_Sub(Vec2* vec_a, Vec2* vec_b);

// <�x�N�g�������Z>
Vec2 Vec2_Scale(Vec2* vec, float scale);

// <�x�N�g���𔽓]>
Vec2 Vec2_Negate(Vec2* vec);

// <�x�N�g����`��>
void Vec2_Render(Vec2* vec, Vec2* base, unsigned int color, float Thickness = 1.f);
