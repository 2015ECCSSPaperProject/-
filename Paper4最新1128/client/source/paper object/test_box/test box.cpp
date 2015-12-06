
#include "iextreme.h"
#include "../paper object.h"
#include "test box.h"
#include "../../Player/BasePlayer.h"

void Test_box::Initialize(iex3DObj *model)
{
	Paper_obj::Initialize(model);
}

void Test_box::Update()
{
	if (number == PLAYER_MAX)
		return;

	if (model == nullptr)
		return;

	model->SetPos( position );
	model->iexMesh::Update();
}

void Test_box::Render(iexShader *shader, char *name)
{
	if (number == PLAYER_MAX)
		return;

	if (model == nullptr)
		return;

	if (shader)
	{
		model->iexMesh::Render(shader, name);
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

unsigned int Test_box::Get_receive_data_size()
{
	return sizeof( Receive_data );
}

void Test_box::Set_receive_data( char *in )
{
	Receive_data *data( ( Receive_data* ) in );
	number = data->user_number;
	model->SetFrame( data->anim_no );
	position = data->pos;
}
