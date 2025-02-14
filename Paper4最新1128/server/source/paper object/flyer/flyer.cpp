
#include "iextreme.h"
#include "../paper object.h"

#include "flyer.h"

#include "../../Player/BasePlayer.h"

const float Flyer::DIST = 10.0f;



Flyer::Flyer() : Paper_obj_3DObj()
{}

Flyer::~Flyer()
{}

void Flyer::Initialize( int model_type, iex3DObj *model, int point )
{
	Paper_obj_3DObj::Initialize( model_type, model, point );
	broken = true;
	high = 50;
}

//**************************************************

void Flyer::Update()
{
	if( broken )
		return;

	high -= 0.1f;
	if( high < 0 )
		high = 0;
	model->SetPos( position.x, position.y + high, position.z );
	model->Animation();
	model->Update();
}


void Flyer::Render()
{
	model->Render();
}

bool Flyer::Can_do(BasePlayer *player)
{
	if( broken )
		return false;

	// 位置と向き判定
	Vector3 ppos; // playerの位置

	ppos = player->Get_pos();

	Vector3 poster_player( ppos - position ); // ポスターからプレイヤー

	// 距離
	if( poster_player.LengthSq() > DIST * DIST ) return false;

	// 向き
	if( Vector3Dot( player->Get_forward(), poster_player ) >= 0.0f ) return false;

	return true;
}

bool Flyer::Can_rend()
{
	return !broken;
}

bool Flyer::Can_dist(const Vector3 &pos, float dist)
{
	if (broken)
		return false;

	Vector3 p[2] = { pos, position };
	p[0].y = p[1].y = 0;

	float Y = position.y - pos.y;
	Y = sqrtf(Y*Y);

	if ((p[0] - p[1]).Length() < dist && Y < dist * .75f)	// 距離判定
		return true;

	return false;
}

void Flyer::Rend()
{
	broken = true;
}

int Flyer::Get_animation_frame()
{
	return 0;
}

//**************************************************

struct Send_data
{
	bool broken;
	int anim_no;
	Vector3 position;
};

unsigned int Flyer::Get_send_data_size()
{
	return sizeof ( Send_data );
}

void Flyer::Get_send_data ( char *out )
{
	Send_data data { broken, model->GetFrame (), position };
	memcpy_s( out, Get_send_data_size(), &data, Get_send_data_size() );
}

KIND_PAPER_OBJECT Flyer::Get_kind()
{
	return KIND_PAPER_OBJECT::FLYER;
}
