
#include "Poster.h"
//#include "../score/Score.h"
#include "../player/BasePlayer.h"

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
		// 戻す me->score->Add(me->ADD_POINT, me->force);
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

	if (me->model->GetFrame() >= 47)
	{
		//me->mynumber = PLAYER_MAX;
		me->mynumber = me->reserve_number;
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

	range.forward = 10.0f;
	range.wide = 5.0f;
	range.min_y = -2.0f;
	range.max_y = 2.0f;
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

	// 貼る
	if (mynumber == PLAYER_MAX)
	{
		mynumber = number;
		Change_mode(MODE::WAITE);
	}
	// 破れる
	else
	{
		Change_mode(MODE::REND);
	}
}

void Poster::Rend(int number)
{
	reserve_number = number;	// 破れた後の色予約(貼る処理完成したら消す)
	//reserve_number = PLAYER_MAX;
	Change_mode(MODE::REND);
}

void Poster::Paste(int number)
{
	mynumber = number;
	Change_mode(MODE::WAITE);
}

bool Poster::Can_do(BasePlayer *player, int number)
{
	if (mode == MODE::REND) // 破れてる途中
		return false;

	if (mynumber == number) // 同じ色
		return false;

	// 位置と向き判定
	Vector3 ppos; // playerの位置

	ppos = player->Get_pos();

	Vector3 poster_player(ppos - position); // ポスターからプレイヤー
	if (poster_player.y < range.min_y || range.max_y < poster_player.y) return false; // y軸

	float length(0);
	length = Vector3Dot(poster_player, forward);
	if (length < 0 || length > range.forward) return false; // ポスターz軸

	Vector3 right(forward.z, forward.y, -forward.x);
	length = Vector3Dot(poster_player, right);
	if (length < -range.wide || length > range.wide) return false; // ポスターx軸

	// 向き
	if (Vector3Dot(player->Get_forward(), poster_player) >= 0.0f) return false;
	

	return true;
}

bool Poster::Can_dist(const Vector3 &pos, float dist, int number)
{
	if (mode == MODE::REND) // 破れてる途中
		return false;

	if (mynumber == number) // 同じ色
		return false;

	if (!Check_dist(pos, dist))	// 距離判定
		return false;

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

bool Poster::Check_dist(const Vector3 &pos, float dist)
{
	return ((pos - position).Length() < dist);
}

void Poster::Change_user(int number)
{
	mynumber = number;
	Change_mode(MODE::WAITE);
}
