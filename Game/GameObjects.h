#pragma once
#include "GameObject.h"

// 定数の定義 ==============================================================

// <プレイヤー>
#define PLAYER_VEL 4.f		// プレイヤーの速度
// <敵>
#define ENEMY_VEL 2			// 敵の速度
// <弾>
#define BULLET_VEL 10		// 敵の速度

// 関数の宣言 ==============================================================

// <<敵オブジェクト>> ----------------------------------------------

// <敵オブジェクト作成>
GameObject GameObject_Enemy_Create(Vec2 pos);

// <敵オブジェクト座標Xデフォルト>
void GameObject_Enemy_SetPosXDefault(GameObject* obj, GameObject* field);

// <敵オブジェクト座標Yデフォルト>
void GameObject_Enemy_SetPosYDefault(GameObject* obj, GameObject* field);

// <敵オブジェクト移動Xデフォルト>
void GameObject_Enemy_SetVelXDefault(GameObject* obj);

// <<プレイヤーオブジェクト>> ----------------------------------------------

// <プレイヤーオブジェクト作成>
GameObject GameObject_Player_Create(void);

// <プレイヤーオブジェクト座標Yデフォルト>
void GameObject_Player_SetPosYDefault(GameObject* obj, GameObject* field);

// <<弾オブジェクト>> ----------------------------------------------

// <弾オブジェクト作成>
GameObject GameObject_Bullet_Create(void);

// <弾オブジェクト座標デフォルト>
void GameObject_Bullet_SetPosDefault(GameObject* obj, GameObject* player);

// <弾オブジェクト移動デフォルト>
void GameObject_Bullet_SetVelDefault(GameObject* obj);

// <弾オブジェクトサイズ変更>
void GameObject_Bullet_SetSize(GameObject* obj, float scale);

// <弾オブジェクト成長>
void GameObject_Bullet_Grow(GameObject* obj);
