#include "GameController.h"
#include "InputManager.h"

#include "Vector.h"
#include "Vec2.h"
#include "GameResource.h"
#include "GameUtils.h"
#include <math.h>

// 列挙型の定義 ============================================================

// <キーセット> --------------------------------------------------------
enum
{
	PLAYER_KEY_UP = PAD_INPUT_UP | PAD_INPUT_8,
	PLAYER_KEY_DOWN = PAD_INPUT_DOWN | PAD_INPUT_5,
	PLAYER_KEY_LEFT = PAD_INPUT_LEFT | PAD_INPUT_4,
	PLAYER_KEY_RIGHT = PAD_INPUT_RIGHT | PAD_INPUT_6
};

// 関数の宣言 ==============================================================

static void GameController_Default_Update(GameController* ctrl);
static void GameController_Default_UpdateControl(GameController* ctrl);
static void GameController_Player_UpdateControl(GameController* ctrl);

// 関数の定義 ==============================================================

// <<キーセット> -------------------------------------------------------

// <キーセット作成>
PlayerKeySet PlayerKeySet_Create(int key_up, int key_down, int key_left, int key_right)
{
	return{ key_up, key_down, key_left, key_right };
}

// <<デフォルトキーセット> ---------------------------------------------

// <キーセット作成>
PlayerKeySet PlayerKeySet_Default_Create(void)
{
	return PlayerKeySet_Create(PLAYER_KEY_UP, PLAYER_KEY_DOWN, PLAYER_KEY_LEFT, PLAYER_KEY_RIGHT);
}

// <<コントローラー>> --------------------------------------------------

// <コントローラー作成>
GameController GameController_Create(const GameObject* field, GameObject* object, void(*updateFunc)(GameController*), void(*updateCtrlFunc)(GameController*))
{
	return{ field, object, updateFunc, updateCtrlFunc };
}

// <コントローラー更新>
void GameController_Update(GameController* ctrl)
{
	ctrl->Update(ctrl);
}

// <コントローラー操作更新>
void GameController_UpdateControl(GameController* ctrl)
{
	ctrl->UpdateControl(ctrl);
}

// <<デフォルトコントローラー>> ----------------------------------------

// <デフォルトコントローラー作成>
GameController GameController_Default_Create(GameObject* field, GameObject* object)
{
	return GameController_Create(field, object, GameController_Default_Update, GameController_Default_UpdateControl);
}

void GameController_Default_Update(GameController* ctrl)
{
	ctrl->object->vel = Vec2_Create();
}

void GameController_Default_UpdateControl(GameController* ctrl)
{
}

// <<プレイヤーコントローラー>> ----------------------------------------

// <プレイヤーコントローラー作成>
GameController GameController_Player_Create(const GameObject* field, GameObject* object, PlayerKeySet keys)
{
	GameController ctrl = GameController_Create(field, object, GameController_Default_Update, GameController_Player_UpdateControl);
	ctrl.player_keys = keys;
	return ctrl;
}

// キー入力でパドルを操作
void GameController_Player_UpdateControl(GameController* ctrl)
{
	ctrl->object->vel = Vec2_Create();
	if (IsKeyDown(ctrl->player_keys.key_up))
		ctrl->object->vel.y += -1;
	if (IsKeyDown(ctrl->player_keys.key_down))
		ctrl->object->vel.y += 1;
	if (IsKeyDown(ctrl->player_keys.key_left))
		ctrl->object->vel.x += -1;
	if (IsKeyDown(ctrl->player_keys.key_right))
		ctrl->object->vel.x += 1;
	ctrl->object->vel = Vec2_Normalized(&ctrl->object->vel);
	ctrl->object->vel = Vec2_Scale(&ctrl->object->vel, PLAYER_VEL);
	ctrl->object->vel = Vec2_Rotate(&ctrl->object->vel, ctrl->object->sprite.angle);

	{
		Vec2 mouse;
		{
			int mouse_x, mouse_y;
			GetMousePoint(&mouse_x, &mouse_y);
			mouse = Vec2_Create((float)mouse_x, (float)mouse_y);
			mouse = Vec2_Sub(&mouse, &ctrl->field->pos);
		}
		SetMouseDispFlag(FALSE);
		SetMousePoint((int)ctrl->field->pos.x, (int)ctrl->field->pos.y);
		ctrl->object->sprite.angle += ToRadians(mouse.x / 5.f);
	}
}

extern GameObject g_field;
extern GameObject g_player;
extern Vector g_player_bullets;
extern Vector g_enemies;
extern Vector g_enemy_bullets;
extern GameResource g_resources;

BOOL ReloadPlayerBullet(int n_way)
{
	int i;
	for (i = 0; i < n_way; i++)
	{
		GameObject obj = GameObject_Bullet_Create();
		obj.shape = SHAPE_CIRCLE;
		obj.sprite = GameSprite_Create(GameTexture_Create(g_resources.texture_bullet, Vec2_Create(), Vec2_Create(32, 32)));
		GameObject_Bullet_SetPosDefault(&obj, &g_player);
		Vector_AddLast(&g_player_bullets, &obj);
	}

	return TRUE;
}

BOOL GrowPlayerBullet(void)
{
	foreach_start(&g_player_bullets, obj)
	{
		if (obj->state == 1)
		{
			GameObject_Bullet_Grow(obj);
			GameObject_Bullet_SetPosDefault(obj, &g_player);
		}
	} foreach_end;

	return TRUE;
}

BOOL ShotPlayerBullet(int n_way)
{
	int num_shot = 0;
	foreach_start(&g_player_bullets, obj)
	{
		if (obj->state == 1)
		{
			GameObject_Bullet_SetVelDefault(obj, g_player.sprite.angle, num_shot++, n_way);
			obj->state = 2;
		}
	} foreach_end;

	return TRUE;
}

BOOL UpdatePlayerBullet(void)
{
	foreach_start(&g_player_bullets, obj)
	{
		GameObject_UpdatePosition(obj);
	} foreach_end;

	return TRUE;
}

BOOL ShotEnemyBullet(const GameObject* enemy)
{
	GameObject obj = GameObject_Bullet_Create();
	obj.pos = enemy->pos;

	{
		float angle = Vec2_Angle(&Vec2_Sub(&g_player.pos, &enemy->pos));
		obj.vel = Vec2_Create(cosf(angle) * 5, sinf(angle) * 5);
		GameTimer_SetRemaining(&obj.count, 3.f);
		GameTimer_Resume(&obj.count);
	}

	//Vector_AddLast(&g_enemy_bullets, &obj);
	return TRUE;
}

BOOL UpdateEnemyBullet(void)
{
	foreach_start(&g_enemy_bullets, obj)
	{
		GameObject_UpdatePosition(obj);

		if (!GameTimer_IsFinished(&obj->count))
		{
			float angle = Vec2_Angle(&obj->vel);
			float direction = Vec2_Angle(&Vec2_Sub(&g_player.pos, &obj->pos));

			float theta = GetLoopRangeF(direction - angle, -DX_PI_F, DX_PI_F);
			float angle_direction = angle + ClampF(theta, -ToRadians(2), ToRadians(2));

			obj->vel = Vec2_Create(cosf(angle_direction) * BULLET_VEL, sinf(angle_direction) * BULLET_VEL);
		}
	} foreach_end;

	return TRUE;
}

BOOL AppearEnemy(void)
{
	GameObject obj = GameObject_Enemy_Create();
	obj.shape = SHAPE_CIRCLE;
	obj.sprite = GameSprite_Create(GameTexture_Create(g_resources.texture_explosion, Vec2_Create(0, 0), Vec2_Create(64, 64)));
	obj.sprite.animation = GameSpriteAnimation_Create(16, 4, 8);
	obj.pos = Vec2_Create(GameObject_GetX(&g_field, CENTER_X), GameObject_GetY(&g_field, CENTER_Y));
	//GameObject_Enemy_SetPosDefault(&g_enemies[i], &g_field);
	//GameObject_Enemy_SetVelDefault(&g_enemies[i]);
	Vector_AddLast(&g_enemies, &obj);

	return TRUE;
}
