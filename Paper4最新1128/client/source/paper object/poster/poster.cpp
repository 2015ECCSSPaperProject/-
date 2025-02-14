
#include "iextreme.h"
#include "../../system/System.h"
#include "../paper object.h"
#include "Poster.h"
#include "../../../../share_data/Enum_public.h"
//#include "../score/Score.h"
//#include "../player/Player.h"

#include	"../../sound/SoundManager.h"

#include "../../timer/Timer.h"

Poster::Poster()
{}
Poster::~Poster()
{}

void Poster::Initialize( iexMesh *model, int point )
{
	Paper_obj_Mesh::Initialize( model, point );
}

void Poster::Subclass_update()
{
	if (broken) return;

	if (rend_delay_time > 0)
	{
		if (--rend_delay_time <= 0)
			broken = true;
	}

	model->Update();
}

void Poster::Subclass_render( iexShader *shader, char *name )
{
	if (broken) return;

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

KIND_PAPER_OBJECT Poster::Get_kind()
{
	return KIND_PAPER_OBJECT::POSTER;
}

void Poster::Rend()
{
	rend_delay_time = 20;
}

void Kaopanel::Rend()
{
	rend_delay_time = 1;
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
	/*number = data->user_number*/;
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

KIND_PAPER_OBJECT Huusenn::Get_kind()
{
	return KIND_PAPER_OBJECT::HUUSENN;
}
void Huusenn::Initialize( iexMesh *model, int point, int start_time )
{
	Paper_obj_Mesh::Initialize( model, point );
	broken = true;
	this->start_time = start_time;
}
void Huusenn::Subclass_update()
{
	if( broken )
	{
		// 出現
		if( start_time >= timer->Get_limit_time() )
		{
			broken = false;
			start_time = UINT_MAX;
		}
	}

	Poster::Subclass_update();
}

KIND_PAPER_OBJECT Kaopanel::Get_kind()
{
	return KIND_PAPER_OBJECT::KAOPANEL;
}
