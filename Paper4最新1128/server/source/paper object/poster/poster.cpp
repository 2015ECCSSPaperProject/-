
#include "iextreme.h"
#include "../../../../share_data/Enum_public.h"
#include "../paper object.h"

#include "Poster.h"
//#include "../score/Score.h"
#include "../../player/BasePlayer.h"

//**************************************************
/// モード
//**************************************************

// 待機
void Poster::Mode_waite::Initialize()
{}
void Poster::Mode_waite::Update()
{}
void Poster::Mode_waite::Render()
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



Poster::Poster() : Paper_obj_Mesh(), mode(MODE::WAITE)
{
	mode_list[(int)MODE::WAITE] = new Mode_waite(this);
}

Poster::Poster( int model_type, iexMesh *model, int point ) : Paper_obj_Mesh(), mode( MODE::WAITE )
{
	mode_list[( int ) MODE::WAITE] = new Mode_waite( this );
	Initialize( model_type, model, point );
}

Poster::~Poster()
{
	for (int i = 0; i < (int)MODE::MAX; i++)
	{
		delete mode_list[i];
	}
	Release();
}

void Poster::Initialize( int model_type, iexMesh *model, int point )
{
	Paper_obj_Mesh::Initialize( model_type, model, point );

	range.forward = 10.0f;
	range.wide = 5.0f;
	range.min_y = -2.0f;
	range.max_y = 2.0f;
}

void Poster::Release()
{
	number = PLAYER_MAX;
	delete model;
	model = nullptr;
	position = Vector3(0, 0, 0);
	forward = Vector3(0, 0, 0);
	angle = 0;
}

void Poster::Update()
{
	if (number == PLAYER_MAX) return;

	mode_list[(int)mode]->Update();
}

void Poster::Render()
{
	if (number == PLAYER_MAX) return;

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
	if (this->number == PLAYER_MAX)
	{
		this->number = number;
		Change_mode(MODE::WAITE);
	}
	// 破れる
	else
	{
		this->number = PLAYER_MAX;
	}
}

void Poster::Rend(int number)
{
	this->number = PLAYER_MAX;
}

void Poster::Rend()
{
	this->number = PLAYER_MAX;
}

void Poster::Paste(int number)
{
	this->number = number;
	Change_mode(MODE::WAITE);
}

bool Poster::Can_do(BasePlayer *player, int number)
{
	if (this->number == number) // 同じ色
		return false;

	if( this->number == PLAYER_MAX )
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

bool Poster::Can_do(BasePlayer *player)
{
	if( this->number == PLAYER_MAX )
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
	if (this->number == PLAYER_MAX)
		return false;

	if (this->number == number) // 同じ色
		return false;

	if (!Check_dist(pos, dist))	// 距離判定
		return false;

	return true;
}

bool Poster::Can_dist(const Vector3 &pos, float dist)
{
	if( this->number == PLAYER_MAX ) // 破れてる途中
		return false;

	if (!Check_dist(pos, dist))	// 距離判定
		return false;

	return true;
}

bool Poster::Can_rend(int number)
{
	return (this->number != PLAYER_MAX && this->number != number);
}

bool Poster::Can_rend()
{
	return this->number != PLAYER_MAX;
}

bool Poster::Can_paste(int number)
{
	return (this->number == PLAYER_MAX);
}

bool Poster::Check_dist(const Vector3 &pos, float dist)
{
	return ((pos - position).Length() < dist);
}

void Poster::Change_user(int number)
{
	this->number = number;
	Change_mode(MODE::WAITE);
}

//**************************************************

struct Poster_send_data
{
	BYTE number;
	int anim_no;
};

unsigned int Poster::Get_send_data_size()
{
	return sizeof( Poster_send_data );
}

void Poster::Get_send_data( char *out )
{
	Poster_send_data *data = ( Poster_send_data* ) out;
	data->number = number;
	data->anim_no = 0;
}

KIND_PAPER_OBJECT Poster::Get_kind()
{
	return KIND_PAPER_OBJECT::POSTER;
}



// ポスターを継承したクラス

KIND_PAPER_OBJECT Calendar::Get_kind()
{
	return KIND_PAPER_OBJECT::CALENDAR;
}

KIND_PAPER_OBJECT Magazin::Get_kind()
{
	return KIND_PAPER_OBJECT::MAGAZIN;
}
