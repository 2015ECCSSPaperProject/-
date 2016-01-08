
#include <vector>
#include "iextreme.h"
#include "../../../share_data/Enum_public.h"
#include "paper object manager.h"

#include "../Player/BasePlayer.h"
#include "paper object.h"

Paper_obj_mng::Paper_obj_mng() : original_model(nullptr), number_of_objects(0)
{
	obj_array.clear();
}

Paper_obj_mng::~Paper_obj_mng()
{
	delete[] original_model;
	delete original_flyer;
	for (unsigned i = 0; i < obj_array.size(); i++)
		delete obj_array[i];
	SAFE_DELETE(mark);
}

void Paper_obj_mng::Initialize()
{
	Load();
	mark = new iex2DObj("DATA/Camera/mark.png");
}

void Paper_obj_mng::Release()
{
	delete[] original_model; original_model = nullptr;
	for (unsigned i = 0; i < obj_array.size(); i++) delete obj_array[i];
	obj_array.clear();
	delete original_flyer;
	//SAFE_DELETE(mark);
}

void Paper_obj_mng::Update()
{
	for (auto it = obj_array.begin(); it != obj_array.end(); it++)
	{
		(*it)->Update();
	}
}

void Paper_obj_mng::Render(BasePlayer *player, iexShader *shader, char *name)
{
	int i = 0;
	if (shader)
	{
		for (auto it : obj_array)
		{
			it->Render(shader, name);
			bool isTarget = (player->Get_action() == BasePlayer::ACTION_PART::REND && player->Get_poster_num() == i);
			if(mark)it->Render_mark(mark, isTarget);
			i++;
		}
	}
	else
	{
		for (auto it : obj_array)
		{
			it->Render();
			bool isTarget = (player->Get_action() == BasePlayer::ACTION_PART::REND && player->Get_poster_num() == i);
			if (mark)it->Render_mark(mark, isTarget);
			i++;
		}
	}
}

//**************************************************

int Paper_obj_mng::Get_numof()
{
	return number_of_objects;
}

void Paper_obj_mng::Set_user(int index, int number)
{
	obj_array[index]->Set_user(number);
}

void Paper_obj_mng::Set_animframe(int index, int frame)
{
	obj_array[index]->Set_animframe(frame);
}

const Vector3& Paper_obj_mng::Get_pos(int index)
{
	return obj_array[index]->Get_pos();
}

KIND_PAPER_OBJECT Paper_obj_mng::Get_kind( int index )
{
	return obj_array[index]->Get_kind();
}

//**************************************************

unsigned int Paper_obj_mng::Get_receive_data_size()
{
	unsigned int size( 0 );

	for( auto it : obj_array )
	{
		size += it->Get_receive_data_size();
	}

	return size;
}

void Paper_obj_mng::Set_receive_data( char *in )
{
	for( auto it : obj_array )
	{
		it->Set_receive_data( in );
		in += it->Get_receive_data_size();
	}
}

//**************************************************

#include "poster/Poster.h"
#include "test_box/test box.h"
#include "flyer/flyer.h"
#include "../fstream/fstream_paper.h"
#include "IEX_Math2.h"
#include <string>

void Paper_obj_mng::Load()
{
	number_of_objects = 0;

	//**************************************************
	/// 3DObjのファイルをロード
	//**************************************************
	//char work[256];

	// モデルロード
	original_flyer = new iex3DObj( "DATA/paper object/flyer/flyer.IEM" );
	unsigned int num_models = 2;							// モデルの種類
	original_model = new iexMesh[num_models];
	original_model[0].LoadIMO("DATA/paper object/Poster/posuta-.IMO");
	original_model[1].LoadIMO( "DATA/paper object/calendar/calendar.IMO" );
	// 位置ロード
	Load_flyer();
	Load_poster_tmp<Poster>( "DATA/MATI/poster_pos.txt", &original_model[0] );
	Load_poster_tmp<Calendar>( "DATA/MATI/calendar_pos.txt", &original_model[1] );
}

void Paper_obj_mng::Load_flyer()
{
	std::ifstream infs( "DATA/MATI/flyer pos.txt" );
	std::string str;

	unsigned int time = 0;
	while( !infs.eof() )
	{
		infs >> str; // 文字列読み込み
		if( str == "time" ) // 発生時間
		{
			infs >> time;
		}
		else
		{
			// 角度
			float angle = std::stof( str, 0 );
			// 位置
			Vector3 pos;
			infs >> pos.x;
			infs >> pos.y;
			infs >> pos.z;
			// フライヤー作成
			Flyer *p = new Flyer;
			p->Initialize( original_flyer, time );
			p->Set_pose( angle, pos );
			obj_array.push_back( p );
			this->number_of_objects++;
		}
	}
}

template<class POSTERCLASS>void Paper_obj_mng::Load_poster_tmp( char *filename, iexMesh *model )
{
	std::ifstream infs( filename );

	// ポスターの位置とか
	POSTERCLASS *p( nullptr );
	float angle( 0 );
	Vector3 pos( 0, 0, 0 );
	int point( 0 );
	while( !infs.eof() )
	{
		infs >> angle;
		angle = Degree_to_radian( angle );
		infs >> pos;
		infs >> point;
		p = new POSTERCLASS;
		p->Initialize( model );
		p->Set_pose( angle, pos );
		obj_array.push_back( p );
		this->number_of_objects++;
	}
	infs.close();
}

Paper_obj_mng *paper_obj_mng = nullptr;
