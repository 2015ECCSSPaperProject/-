#include "../system/System.h"
#include "Poster.h"
//#include "../score/Score.h"
//#include "../player/Player.h"

//**************************************************
/// ���[�h
//**************************************************

// �ҋ@
void Poster::Mode_waite::Initialize()
{
	me->timer = me->POINT_TIME;
	me->model->SetMotion((int)MOTION_NUMBER::WAITE);
}
void Poster::Mode_waite::Update()
{
	me->timer--;

	if (me->timer <= 0)
	{
		me->timer = me->POINT_TIME;
		// �߂� me->score->Add(me->ADD_POINT, me->mynumber);
	}

}
void Poster::Mode_waite::Render()
{
	if (me->model != nullptr)
	{
		me->model->Update();
		//�����̃e�N�X�`���[��K�p�����V�F�[�_�[��
		char ShaderName[64];
		sprintf(ShaderName, "poster_%d", me->mynumber);

		me->model->Render(shader, ShaderName);
	}
}

// �j���
void Poster::Mode_rend::Initialize()
{
	me->model->SetMotion((int)MOTION_NUMBER::REND);
}
void Poster::Mode_rend::Update()
{
	if (me->model)
		me->model->Animation();

	if (me->model->GetFrame() >= 47)
	{
		me->mynumber = PLAYER_MAX;
		me->Change_mode(MODE::WAITE);
	}
}
void Poster::Mode_rend::Render()
{
	if (me->model != nullptr)
	{
		me->model->Update();
		//�����̃e�N�X�`���[��K�p�����V�F�[�_�[��
		char ShaderName[64];
		sprintf(ShaderName, "poster_%d", me->mynumber);
	
		me->model->Render(shader, ShaderName);
	}
}

void Poster::Change_mode(MODE m)
{
	mode = m;
	mode_list[(int)mode]->Initialize();
}



Poster::Poster() : score(nullptr), mynumber(PLAYER_MAX), POINT_TIME(0), ADD_POINT(0), timer(0), model(nullptr), position(0, 0, 0), forward(0, 0, 1), angle(0), mode(MODE::WAITE)
{
	mode_list[(int)MODE::WAITE] = new Mode_waite(this);
	mode_list[(int)MODE::REND] = new Mode_rend(this);
}
Poster::~Poster()
{
	for (int i = 0; i < (int)MODE::MAX; i++)
	{
		delete mode_list[i];
	}
	Release();
}

void Poster::Initialize(iex3DObj *model, Score *score, int point)
{
	this->score = score;
	ADD_POINT = point;
	POINT_TIME = 60;

	this->model = model->Clone();
	this->model->SetScale(1.0f);
	this->model->SetAngle(PI);
	this->model->SetPos(0, 0, 0);
	this->model->Update();

	range.forward = 4.0f;
	range.wide = 2.0f;
	range.min_y = -1.0f;
	range.max_y = 1.0f;
}

void Poster::Release()
{
	mynumber = PLAYER_MAX;
	delete model;
	model = nullptr;
	position = Vector3(0, 0, 0);
	forward = Vector3(0, 0, 0);
	angle = 0;
}

void Poster::Update()
{
	if (mynumber == PLAYER_MAX) return;

	mode_list[(int)mode]->Update();
}

void Poster::Render()
{
	if (mynumber == PLAYER_MAX) return;

	mode_list[(int)mode]->Render();
}



void Poster::Set_pose(const Vector3& forward, const Vector3& pos)
{
	this->forward = forward;

	angle = Vector3Dot(this->forward, Vector3(0, 0, 1));
	if (angle < -1.0f) angle = -1.0f;
	else if (angle > 1.0f) angle = 1.0f;
	angle = acosf(angle);

	model->SetAngle(angle);

	this->position = pos;
	model->SetPos(position);
}
void Poster::Set_pose(float angle, const Vector3& pos)
{
	forward.x = sinf(angle);
	forward.y = 0;
	forward.z = cosf(angle);

	this->angle = angle;
	model->SetAngle(angle);

	this->position = pos;
	model->SetPos(position);
}



void Poster::Do_playeraction(BasePlayer *player, int number)
{
	assert(player != nullptr);

	if (!Can_do(player, number)) return;

	// �\��
	if (mynumber == PLAYER_MAX)
	{
		mynumber = number;
		Change_mode(MODE::WAITE);
	}
	// �j��
	else
	{
		Change_mode(MODE::REND);
	}
}

void Poster::Rend(int number)
{
	Change_mode(MODE::REND);
}

void Poster::Paste(int number)
{
	mynumber = number;
	Change_mode(MODE::WAITE);
}

bool Poster::Can_do(BasePlayer *player, int number)
{
	if (mode == MODE::REND) // �j��Ă�r��
		return false;

	if (mynumber == number) // �����F
		return false;

	/* �߂�
	// �ʒu�ƌ�������
	Vector3 ppos; // player�̈ʒu

	ppos = player->Get_pos();

	Vector3 poster_player(ppos - position); // �|�X�^�[����v���C���[
	if (poster_player.y < range.min_y || range.max_y < poster_player.y) return false; // y��

	float length(0);
	length = Vector3Dot(poster_player, forward);
	if (length < 0 || length > range.forward) return false; // �|�X�^�[z��

	Vector3 right(forward.z, forward.y, -forward.x);
	length = Vector3Dot(poster_player, right);
	if (length < -range.wide || length > range.wide) return false; // �|�X�^�[x��

	// ����
	if (Vector3Dot(player->Get_forward(), poster_player) >= 0.0f) return false;
	*/

	return true;
}

bool Poster::Can_rend(int number)
{
	return (mynumber != PLAYER_MAX && mynumber != number);
}

bool Poster::Can_paste(int number)
{
	return (mynumber == PLAYER_MAX);
}



void Poster::Change_user(int number)
{
	if (mynumber == number) return;
	mynumber = number;
	Change_mode(MODE::WAITE);
}
