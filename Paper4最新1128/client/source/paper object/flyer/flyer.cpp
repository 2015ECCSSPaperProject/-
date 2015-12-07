
#include "iextreme.h"
#include "../paper object.h"

#include "flyer.h"

void Flyer::Initialize( iex3DObj *model )
{
	Paper_obj::Initialize( model );
}

void Flyer::Update()
{
	if( number == PLAYER_MAX )
		return;

	if( model == nullptr )
		return;

	model->SetPos( position );
	model->Animation();
	model->Update();
}

void Flyer::Render( iexShader *shader, char *name )
{
	if( number == PLAYER_MAX )
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
	number = data->user_number;
	model->SetFrame( data->anim_no );
	position = data->pos;
}
