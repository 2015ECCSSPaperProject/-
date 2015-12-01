
#include "iextreme.h"
#include "../paper object.h"
#include "test box.h"
#include "../../Player/BasePlayer.h"

void Test_box::Initialize(int model_type, iex3DObj *model, int point)
{
	Paper_obj::Initialize(model_type, model, point);
	mode = MODE::WAITE;
}

void Test_box::Update()
{}

void Test_box::Render()
{
	if (mode == MODE::MAX)
		return;

	if (model == nullptr)
		return;

	model->iexMesh::Update();
	model->iexMesh::Render();
}


bool Test_box::Can_do(BasePlayer *player)
{
	if (mode == MODE::REND) // 破れてる途中
		return false;

	// 位置と向き判定
	Vector3 ppos; // playerの位置

	ppos = player->Get_pos();

	Vector3 poster_player(ppos - position); // ポスターからプレイヤー

	// 距離
	if (poster_player.LengthSq() > 5 * 5) return false;

	// 向き
	if (Vector3Dot(player->Get_forward(), poster_player) >= 0.0f) return false;

	return true;
}

bool Test_box::Can_rend()
{
	return mode == MODE::WAITE;
}

bool Test_box::Can_dist(const Vector3 &pos, float dist)
{
	if (mode == MODE::REND) // 破れてる途中
		return false;

	if ((pos - position).LengthSq() > dist * dist)	// 距離判定
		return false;

	return true;
}

void Test_box::Rend()
{
	mode = MODE::MAX;
}

//**************************************************

int Test_box::Get_number()
{
	if (mode == MODE::MAX)
		return PLAYER_MAX;

	return 0;
}

int Test_box::Get_animation_frame()
{
	return 0;
}

float Test_box::Get_angle()
{
	return angle;
}

const Vector3 &Test_box::Get_pos()
{
	return position;
}

void Test_box::Set_pose(const float angle, const Vector3& pos)
{
	forward.x = sinf(angle);
	forward.y = 0;
	forward.z = cosf(angle);

	this->angle = angle;
	model->SetAngle(angle);

	this->position = pos;
	model->SetPos(position);

	model->SetScale(1);
}
