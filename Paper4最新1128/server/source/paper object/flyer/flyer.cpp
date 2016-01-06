
#include "iextreme.h"
#include "../paper object.h"

#include "flyer.h"

#include "../../Player/BasePlayer.h"

const float Flyer::DIST = 10.0f;



Flyer::Flyer() : Paper_obj_3DObj()
{
	number = PLAYER_MAX;
	high = 50;
}

Flyer::~Flyer()
{}

//**************************************************

void Flyer::Update()
{
	if( number == PLAYER_MAX )
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
	if( number == PLAYER_MAX )
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
	return ( number != PLAYER_MAX );
}

bool Flyer::Can_dist(const Vector3 &pos, float dist)
{
	if( number == PLAYER_MAX )
		return false;

	if( ( pos - position ).LengthSq() > dist * dist )	// 距離判定
		return false;

	return true;
}

void Flyer::Rend()
{
	number = PLAYER_MAX;
}

int Flyer::Get_animation_frame()
{
	return 0;
}

//**************************************************

struct Send_data
{
	BYTE number;
	int anim_no;
	Vector3 position;
};

unsigned int Flyer::Get_send_data_size()
{
	return sizeof ( Send_data );
}

void Flyer::Get_send_data ( char *out )
{
	Send_data data { number, model->GetFrame (), position };
	memcpy_s( out, Get_send_data_size(), &data, Get_send_data_size() );
}
