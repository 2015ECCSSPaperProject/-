
#include <vector>

#include "paper object manager.h"

#include "iextreme.h"
#include "paper object.h"

#include "../event/Event list.h"
#include "../event/Event.h"
#include "../event/Event advent paper obj.h"

#include "../../../share_data/Enum_public.h"
#include "../Player/BasePlayer.h"
#include "../score/Score.h"

Paper_obj_mng::Paper_obj_mng() : original_model(nullptr), number_of_objects(0)
{
	obj_array.clear();
}

Paper_obj_mng::~Paper_obj_mng()
{
	delete[] original_model;
	for (unsigned i = 0; i < obj_array.size(); i++) delete obj_array[i];
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
}

void Paper_obj_mng::Update()
{
	for (auto it = obj_array.begin(); it != obj_array.end(); it++)
	{
		(*it)->Update();
	}
}

void Paper_obj_mng::Render()
{
	for (unsigned i = 0; i < obj_array.size(); i++)
	{
		obj_array[i]->Render();
	}
}

//**************************************************

int Paper_obj_mng::Can_do(BasePlayer *player)
{
	int index = -1;

	for (unsigned int i = 0; i < obj_array.size(); i++)
	{
		if (obj_array[i]->Can_do(player))
		{
			index = i;
		}
	}

	return index;
}

void Paper_obj_mng::Can_dist(const Vector3 &pos, float dist, int out[])
{
	int count = 0;
	for (unsigned int i = 0; i < obj_array.size(); i++)
	{
		if (obj_array[i]->Can_dist(pos, dist))
		{
			out[count++] = i;
		}
	}
	out[count] = -1;	// 終端は-1
}

bool Paper_obj_mng::Can_rend(int index)
{
	return obj_array[index]->Can_rend();
}

void Paper_obj_mng::Rend(int index)
{
	obj_array[index]->Rend();
}

int Paper_obj_mng::Can_targeting(BasePlayer *player, float range_dist, int range_degree)
{
	float min_dist = range_dist;
	int ret_num = -1;

	for (unsigned int i = 0; i < obj_array.size(); i++)
	{
		if (obj_array[i]->Get_number() == PLAYER_MAX) continue;

		//④v1とv2のなす角（度）を計算し、変数angleに代入する
		Vector3 player_front(sinf(player->Get_angleY()), 0, cosf(player->Get_angleY()));
		Vector3 to_target_vec(obj_array[i]->Get_pos() - player->Get_pos());

		const float to_target_len = to_target_vec.Length();
		const int theta = (int)(acosf(Vector3Dot(player_front, to_target_vec) / (player_front.Length() * to_target_len)) / 0.01745f);

		if (theta < range_degree && to_target_len < range_dist)	// 距離と角度
		{
			if (to_target_len < min_dist)
			{
				ret_num = i;
				min_dist = to_target_len;
			}
		}
	}
	return ret_num;
}



int Paper_obj_mng::Get_numof()
{
	return number_of_objects;
}

int Paper_obj_mng::Get_number(int index)
{
	return obj_array[index]->Get_number();
}

int Paper_obj_mng::Get_point(int index)
{
	return obj_array[index]->Get_point();
}

int Paper_obj_mng::Get_type(int index)
{
	return obj_array[index]->Get_type();
}

int Paper_obj_mng::Get_animation_frame(int index)
{
	return obj_array[index]->Get_animation_frame();
}

const Vector3& Paper_obj_mng::Get_pos(int index)
{
	return obj_array[index]->Get_pos();
}

float Paper_obj_mng::Get_angle(int index)
{
	return obj_array[index]->Get_angle();
}

//void Paper_obj_mng::Rend_poster(int number, int index)
//{
//	poster->Rend_poster(number, index);
//}

//void Paper_obj_mng::Can_dist(const Vector3 &pos, float dist, int number, int out[])
//{
//	poster->Can_dist(pos, dist, number, out);
//}

//**************************************************

unsigned int Paper_obj_mng::Get_send_data_size()
{
	unsigned int out( 0U );

	for( auto it : obj_array )
	{
		out += it->Get_send_data_size();
	}

	return out;
}

void Paper_obj_mng::Get_send_data( char *out )
{
	for( auto it : obj_array )
	{
		it->Get_send_data( out );
		out += it->Get_send_data_size();
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
	unsigned int num_models = 2; // モデルの種類
	original_model = new iex3DObj[num_models];
	original_model[0].LoadObject( "DATA//paper object/Poster/posuta-.IEM" );
	original_model[1].LoadObject( "DATA/paper object/flyer/flyer.IEM" );

	number_of_objects = 0;

	Load_flyer();
	Load_poster_tmp<Poster>( "DATA/MATI/poster_pos.txt" );
	Load_poster_tmp<Calendar>( "DATA/MATI/calendar_pos.txt" );
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
			p->Initialize( 1, &original_model[1], 5 );
			p->Set_pose( angle, pos );
			obj_array.push_back( p );
			this->number_of_objects++;
			// イベント設定
			Event_advent_paper_obj *ev;
			ev = new Event_advent_paper_obj( p );
			ev->Set_time( ( unsigned int ) ( time * 1000 ) );
			ev->Set_telop_id( 1 );
			event_list->push( ev );
		}
	}
}

template<class POSTERCLASS>void Paper_obj_mng::Load_poster_tmp( char *filename )
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
		p->Initialize( 0, &original_model[0], point );
		p->Set_pose( angle, pos );
		obj_array.push_back( p );
		this->number_of_objects++;
	}
	infs.close();
}

Paper_obj_mng *paper_obj_mng = nullptr;
