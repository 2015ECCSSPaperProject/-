
#include "iextreme.h"
#include "../../system/System.h"
#include "../paper object.h"
#include "../../../../share_data/Enum_public.h"
#include "Poster.h"
//#include "../score/Score.h"
//#include "../player/Player.h"

#include	"../../sound/SoundManager.h"

Poster::Poster()
{}
Poster::~Poster()
{}

void Poster::Initialize(iexMesh *model)
{
	Paper_obj_Mesh::Initialize(model);
}

void Poster::Update()
{
	if (number == PLAYER_MAX) return;

	model->Update();
}

void Poster::Render(iexShader *shader, char *name)
{
	if (number == PLAYER_MAX) return;

	model->Update();

	if (shader)
	{
		model->Render(shader, name);
	}
	else
	{
		model->Render();
	}
}

//**************************************************

struct Receive_data
{
	BYTE user_number;
	int anim_no;
};

unsigned int Poster::Get_receive_data_size()
{
	return sizeof( Receive_data );
}

void Poster::Set_receive_data( char *in )
{
	Receive_data *data( ( Receive_data* ) in );
	number = data->user_number;
}
