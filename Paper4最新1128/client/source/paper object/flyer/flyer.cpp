
#include "iextreme.h"
#include "../paper object.h"

#include "flyer.h"
#include "../../timer/Timer.h"
#include "../../../../share_data/Enum_public.h"

void Flyer::Initialize( iex3DObj *model, int start_time )
{
	Paper_obj_3DObj::Initialize( model );
	broken = true;
	high = 50;
	this->start_time = start_time;
}

void Flyer::Update()
{
	if( broken )
	{
		// oŒ»
		if( start_time >= timer->Get_limit_time() )
		{
			broken = false;
			start_time = UINT_MAX;
		}
		else
			return;
	}

	if( model == nullptr )
		return;

	high -= 0.1f;
	if( high < 0 )
		high = 0;
	model->SetPos( position.x, position.y + high, position.z );
	model->Animation();
	model->Update();
}

void Flyer::Subclass_render( iexShader *shader, char *name )
{
	if( broken )
		return;

	if( model == nullptr )
		return;

	if( shader )
	{
		model->iexMesh::Render( shader, name );
	}
	else
	{
		model->iexMesh::Render();
	}
}

KIND_PAPER_OBJECT Flyer::Get_kind()
{
	return KIND_PAPER_OBJECT::FLYER;
}

//**************************************************

struct Receive_data
{
	BYTE user_number;
	int anim_no;
	Vector3 pos;
};

unsigned int Flyer::Get_receive_data_size()
{
	return sizeof( Receive_data );
}

void Flyer::Set_receive_data( char *in )
{
	Receive_data *data( ( Receive_data* ) in );
	/*number = data->user_number;*/
	model->SetFrame( data->anim_no );
	position = data->pos;
}
