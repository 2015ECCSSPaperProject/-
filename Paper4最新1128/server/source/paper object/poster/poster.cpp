
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
	broken = false;
	delete model;
	model = nullptr;
	position = Vector3(0, 0, 0);
	forward = Vector3(0, 0, 0);
	angle = 0;
}

void Poster::Update()
{
	if (broken) return;

	mode_list[(int)mode]->Update();
}

void Poster::Render()
{
	if (broken) return;

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



bool Poster::Can_do(BasePlayer *player)
{
	if( broken )
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

bool Poster::Can_dist(const Vector3 &pos, float dist)
{
	if( broken ) // 破れてる途中
		return false;

	if (!Check_dist(pos, dist))	// 距離判定
		return false;

	return true;
}

bool Poster::Can_rend()
{
	return !broken;
}

bool Poster::Check_dist(const Vector3 &pos, float dist)
{
	Vector3 p[2] = { pos, position };
	p[0].y = p[1].y = 0;

	float Y = position.y - pos.y;
	Y = sqrtf(Y*Y);

	return ((p[0] - p[1]).Length() < dist && Y < dist * .75f);
}

//**************************************************

struct Poster_send_data
{
	bool broken;
	int anim_no;
};

unsigned int Poster::Get_send_data_size()
{
	return sizeof( Poster_send_data );
}

void Poster::Get_send_data( char *out )
{
	Poster_send_data *data = ( Poster_send_data* ) out;
	data->broken = broken;
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

KIND_PAPER_OBJECT Money::Get_kind()
{
	return KIND_PAPER_OBJECT::MONEY;
}

KIND_PAPER_OBJECT Seisho::Get_kind()
{
	return KIND_PAPER_OBJECT::SEISHO;
}

KIND_PAPER_OBJECT Shinbun::Get_kind()
{
	return KIND_PAPER_OBJECT::SHINBUN;
}

KIND_PAPER_OBJECT Sign::Get_kind()
{
	return KIND_PAPER_OBJECT::SIGN;
}

KIND_PAPER_OBJECT Toilet_paper::Get_kind()
{
	return KIND_PAPER_OBJECT::TOILET_PAPER;
}

KIND_PAPER_OBJECT Zasshi::Get_kind()
{
	return KIND_PAPER_OBJECT::ZASSHI;
}

KIND_PAPER_OBJECT Shoji::Get_kind()
{
	return KIND_PAPER_OBJECT::SHOJI;
}

bool Shoji::Collision( const Vector3 &pos, Vector3 *move, float radius, int recursive_counter, Vector3 *n )
{
	// 当たり範囲
	const float rangeX( 5 ), rangeY( 5 );
	// 上下
	if( pos.y <= this->position.y - 1 || this->position.y + 5 <= pos.y )
		return false;
	// 左右
	Vector3 vec( pos - this->position );

	Vector3 right;
	Vector3Cross( right, Vector3( 0, 1, 0 ), forward );
	float rightLen( Vector3Dot( vec, right ) );
	if( rightLen < -rangeX || rangeX < rightLen )
		return false;
	
	float forwardLen( Vector3Dot( vec, forward ) );
	return false;

	return true;
}

KIND_PAPER_OBJECT Huusenn::Get_kind()
{
	return KIND_PAPER_OBJECT::HUUSENN;
}

KIND_PAPER_OBJECT Kaopanel::Get_kind()
{
	return KIND_PAPER_OBJECT::KAOPANEL;
}
