
#include "Poster.h"
#include "../score/Score.h"
#include "../player/Player.h"
//**************************************************
/// モード
//**************************************************

// 待機
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
		me->score->Add(me->ADD_POINT, me->force);
	}

}
void Poster::Mode_waite::Render()
{
	if (me->model != nullptr)
	{
		me->model->Update();
		me->model->Render();
	}
}

// 破れる
void Poster::Mode_rend::Initialize()
{
	me->model->SetMotion((int)MOTION_NUMBER::REND);
}
void Poster::Mode_rend::Update()
{
	if (me->model)
		me->model->Animation();

	if (me->model->GetFrame() >= 58)
	{
		me->force = TEAM_COLOR::NONE;
		me->Change_mode(MODE::WAITE);
	}
}
void Poster::Mode_rend::Render()
{
	if (me->model != nullptr)
	{
		me->model->Update();
		me->model->Render();
	}
}

void Poster::Change_mode(MODE m)
{
	mode = m;
	mode_list[(int)mode]->Initialize();
}



Poster::Poster() : score(nullptr), force(TEAM_COLOR::NONE), POINT_TIME(0), ADD_POINT(0), timer(0), model(nullptr), position(0, 0, 0), forward(0, 0, 1), angle(0), mode(MODE::WAITE)
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
	force = TEAM_COLOR::NONE;
	delete model;
	model = nullptr;
	position = Vector3(0, 0, 0);
	forward = Vector3(0, 0, 0);
	angle = 0;
}

void Poster::Update()
{
	if (force == TEAM_COLOR::NONE) return;

	mode_list[(int)mode]->Update();
}

void Poster::Render()
{
	if (force == TEAM_COLOR::NONE) return;

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



void Poster::Do_playeraction(Player *player, TEAM_COLOR color, Texture2D *tex)
{
	assert(player != nullptr);

	if (mode == MODE::REND) // 破れてる途中
		return;

	if (force == color) // 同じ色
		return;

	// 位置と向き判定
	Vector3 ppos; // playerの位置

	ppos = player->Get_pos();

	Vector3 poster_player(ppos - position); // ポスターからプレイヤー
	if (poster_player.y < range.min_y || range.max_y < poster_player.y) return; // y軸

	float length(0);
	length = Vector3Dot(poster_player, forward);
	if (length < 0 || length > range.forward) return; // ポスターz軸

	Vector3 right(forward.z, forward.y, -forward.x);
	length = Vector3Dot(poster_player, right);
	if (length < -range.wide || length > range.wide) return; // ポスターx軸

	// 向き
	if (Vector3Dot(player->Get_forward(), poster_player) >= 0.0f) return;


	// 貼る
	if (force == TEAM_COLOR::NONE)
	{
		force = color;
		Change_mode(MODE::WAITE);
		model->SetTexture(tex, 0);
		player->Set_do_flag(Player::DO_FLAG::PASTE);
	}
	// 破る
	else
	{
		Change_mode(MODE::REND);
		player->Set_do_flag(Player::DO_FLAG::REND);
	}
}



void Poster::Change_color(TEAM_COLOR color, Texture2D *tex)
{
	force = color;
	Change_mode(MODE::WAITE);
	model->SetTexture(tex, 0);
}
