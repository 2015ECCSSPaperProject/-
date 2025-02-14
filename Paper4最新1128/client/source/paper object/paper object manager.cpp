
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
}

void Paper_obj_mng::Initialize()
{
	Load();
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

void Paper_obj_mng::Render( const Vector3 &pos, iexShader *shader, char *name )
{
	for (auto it : obj_array)
	{
		Vector3 v( it->Get_pos() - pos );
		if( Vector3Dot( v, Vector3( matView._13, matView._23, matView._33 ) ) < 0 )
			continue;
		if( v.LengthSq() > 1000000 ) // 1000 * 1000
			continue;
		it->Render(shader, name);
	}
}

void Paper_obj_mng::Forward_render()
{
	for( auto it : obj_array )
	{
		it->Forward_render();
	}
}

//**************************************************

void Paper_obj_mng::Rend( int index )
{
	obj_array[index]->Rend();
}
void Paper_obj_mng::Rend_by_skill(int index, BasePlayer *player)
{
	obj_array[index]->Rend_by_skill(player);
}


bool Paper_obj_mng::Can_rend(int index)
{
	return obj_array[index]->Can_rend();
}

int Paper_obj_mng::Get_numof()
{
	return number_of_objects;
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

int Paper_obj_mng::Get_point(int index)
{
	return obj_array[index]->Get_point();
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
	unsigned int num_models = 12;							// モデルの種類
	original_model = new iexMesh[num_models]{};
	original_model[0].LoadIMO( "DATA/paper object/Poster/posuta-.IMO" );
	original_model[1].LoadIMO( "DATA/paper object/calendar/calendar.IMO" );
	original_model[2].LoadIMO( "DATA/paper object/magazin/magazin.IMO" );
	original_model[3].LoadIMO( "DATA/paper object/money/money.IMO" );
	original_model[4].LoadIMO( "DATA/paper object/seisho/seisho.IMO" );
	original_model[5].LoadIMO( "DATA/paper object/sign/sign.IMO" );
	original_model[6].LoadIMO( "DATA/paper object/toilet_paper/toilet_roll.IMO" );
	original_model[7].LoadIMO( "DATA/paper object/zasshi/zasshi.IMO" );
	original_model[8].LoadIMO( "DATA/paper object/shinbun/shinbun.IMO" );
	original_model[9].LoadIMO( "DATA/paper object/shoji/shoji.IMO" );
	original_model[10].LoadIMO( "DATA/paper object/huusen/kamiHuusen.IMO" );
	original_model[11].LoadIMO( "DATA/paper object/kaopanel/kaopanel.IMO" );

	// 位置ロード
	Load_timeposter_tmp<Flyer>( "DATA/MATI/position/flyer pos.txt", original_flyer );
	Load_poster_tmp<Poster>( "DATA/MATI/position/poster_pos.txt", &original_model[0] );
	Load_poster_tmp<Calendar>( "DATA/MATI/position/calendar_pos.txt", &original_model[1] );
	Load_poster_tmp<Magazin>( "DATA/MATI/position/magazin_pos.txt", &original_model[2] );
	Load_poster_tmp<Money>( "DATA/MATI/position/money_pos.txt", &original_model[3] );
	Load_poster_tmp<Seisho>( "DATA/MATI/position/seisho_pos.txt", &original_model[4] );
	Load_poster_tmp<Sign>( "DATA/MATI/position/sign_pos.txt", &original_model[5] );
	Load_poster_tmp<Toilet_paper>( "DATA/MATI/position/toilet_paper_pos.txt", &original_model[6] );
	Load_poster_tmp<Zasshi>( "DATA/MATI/position/zasshi.txt", &original_model[7] );
	Load_poster_tmp<Shinbun>( "DATA/MATI/position/shinbun_pos.txt", &original_model[8] );
	Load_poster_tmp<Shoji>( "DATA/MATI/position/shoji_pos.txt", &original_model[9] );
	Load_timeposter_tmp<Huusenn>( "DATA/MATI/position/huusen_pos.txt", &original_model[10] );
	Load_poster_tmp<Kaopanel>( "DATA/MATI/position/kaopanel_pos.txt", &original_model[11] );
}

template<class POSTERCLASS, class MESHCLASS>void Paper_obj_mng::Load_timeposter_tmp( char *filename, MESHCLASS *model )
{
	std::ifstream infs( filename );
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
			// ポイント
			int point;
			infs >> point;
			// フライヤー作成
			POSTERCLASS *p = new POSTERCLASS;
			p->Initialize( model, point, time );
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
		// 位置
		infs >> angle;
		angle = Degree_to_radian( angle );
		infs >> pos;
		// ポイント
		int point;
		infs >> point;
		// 作成
		p = new POSTERCLASS;
		p->Initialize( model, point );
		p->Set_pose( angle, pos );
		obj_array.push_back( p );
		this->number_of_objects++;
	}
	infs.close();
}

Paper_obj_mng *paper_obj_mng = nullptr;
