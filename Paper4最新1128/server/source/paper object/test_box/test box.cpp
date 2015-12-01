
#include "iextreme.h"
#include "../paper object.h"
#include "test box.h"
#include "../../Player/BasePlayer.h"

void Test_box::Initialize(int model_type, iex3DObj *model, int point)
{
	Paper_obj::Initialize(model_type, model, point);
	mode = MODE::WAITE;
	number = 0;
}

void Test_box::Update()
{}

void Test_box::Render()
{
	if (mode == MODE::MAX)
		return;

	if (number == PLAYER_MAX)
		return;

	if (model == nullptr)
		return;

	model->iexMesh::Update();
	model->iexMesh::Render();
}


bool Test_box::Can_do(BasePlayer *player)
{
	if (mode == MODE::REND) // �j��Ă�r��
		return false;

	if (number == PLAYER_MAX)
		return false;

	// �ʒu�ƌ�������
	Vector3 ppos; // player�̈ʒu

	ppos = player->Get_pos();

	Vector3 poster_player(ppos - position); // �|�X�^�[����v���C���[

	// ����
	if (poster_player.LengthSq() > 5 * 5) return false;

	// ����
	if (Vector3Dot(player->Get_forward(), poster_player) >= 0.0f) return false;

	return true;
}

bool Test_box::Can_rend()
{
	return (mode == MODE::WAITE && number != PLAYER_MAX);
}

bool Test_box::Can_dist(const Vector3 &pos, float dist)
{
	if (mode == MODE::REND) // �j��Ă�r��
		return false;

	if (number == PLAYER_MAX)
		return false;

	if ((pos - position).LengthSq() > dist * dist)	// ��������
		return false;

	return true;
}

void Test_box::Rend()
{
	mode = MODE::MAX;
	number = PLAYER_MAX;
}

int Test_box::Get_animation_frame()
{
	return 0;
}
