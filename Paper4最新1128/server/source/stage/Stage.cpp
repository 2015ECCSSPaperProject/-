
#include "iextreme.h"

#include "Stage.h"

#include <vector>
#include "area.h"

#include "../event/Event list.h"
#include "../event/Event.h"
#include "../event/Event open area.h"
//#include "../player/Player.h"
#include <string>

Stage::Stage() : collision_model(nullptr), area(nullptr)
{}

Stage::~Stage()
{
	Release();
}

void Stage::Initialize()
{
	collision_model = new iexMesh("DATA/MATI/stage_atari.IMO");

	Load_area();
}

void Stage::Release()
{
	delete collision_model; collision_model = nullptr;
	delete area; area = nullptr;
}

void Stage::Update()
{

}

void Stage::Render()
{
	if( collision_model )
		collision_model->Render();
	if( area )
		area->Render();
}

// 当たり判定

bool Stage::Collision_rand(const Vector3 &pos, Vector3 *move)
{
	if (move->y >= 0)
		return false;

	Vector3 hit_pos, ray_pos(pos), vec(0, -1, 0);
	float dist(FLT_MAX);
	// xzだけ移動
	ray_pos += *move;
	ray_pos.y = pos.y + COLLISION_UP_SHIFT;

	if (collision_model->RayPickUD(&hit_pos, &ray_pos, &vec, &dist) == -1)
		return false;

	if (pos.y + move->y >= hit_pos.y)
		return false;

	move->y = hit_pos.y - pos.y;
	return true;
}

bool Stage::Collision_up( const Vector3 &pos, Vector3 *move, float high)
{
	if( move->y <= 0 )
		return false;

	Vector3 hit_pos, ray_pos( pos ), vec( 0, 1, 0 );
	float dist( FLT_MAX );
	// xzだけ移動
	ray_pos += *move;
	ray_pos.y = pos.y + COLLISION_UP_SHIFT;

	if( collision_model->RayPick( &hit_pos, &ray_pos, &vec, &dist ) == -1 )
		return false;

	if( pos.y + high + move->y <= hit_pos.y )
		return false;

	move->y = hit_pos.y - ( pos.y + high );
	return true;
}

// 横
bool Stage::Collision(const Vector3 &pos, Vector3 *move, float radius, int recursive_counter)
{
	if (recursive_counter <= 0)
		return false;

	Vector3 move_xz(move->x, 0, move->z); // moveのxとzだけ

	Vector3 out, ray_pos, vec;
	float dist(FLT_MAX);
	Vector3 hit_pos, normal;
	float length(FLT_MAX);
	// レイの始点を上げる
	ray_pos = pos;
	ray_pos.y += COLLISION_UP_SHIFT;
	// レイの方向を決める
	vec = move_xz;
	vec.y = 0;
	vec.Normalize();

	// true  : 当たってない
	// false : 当たってる
	bool ret = true;
	if (collision_model->RayPick(&out, &ray_pos, &vec, &dist) != -1)
	{
		ret = false;
		hit_pos = out;
		normal = vec;
		length = dist;
	}

	// レイの方向を決める
	vec = move_xz;
	vec.y = 0;
	vec.Normalize();

	if (area->RayPick(&out, &ray_pos, &vec, &dist) != -1 && length > dist)
	{
		ret = false;
		hit_pos = out;
		normal = vec;
		length = dist;
	}
	// 当たってない
	if (ret == true)
		return false;

	/***********
	  位置補正
	***********/
	Vector3 next_pos(pos + move_xz);
	// 壁にめり込んだ量
	Vector3 sink(next_pos - hit_pos);
	// sinkの法線方向の大きさ
	float sink_nl;
	normal.y = 0;
	normal.Normalize();
	sink_nl = -Vector3Dot(sink, normal);

	if (sink_nl + radius <= 0)
		return false;

	sink_nl += radius;
	*move += normal * sink_nl;

	return true;

	return Collision(pos, move, radius, recursive_counter - 1);
}



unsigned int Stage::Area_Get_numof()
{
	return area->Get_numof();
}

bool Stage::Area_Is_work(unsigned int index)
{
	return area->Is_work(index);
}

void Stage::Load_area()
{
	if( area )delete area;
	area = new Area_mng;

	std::ifstream ifs( "DATA/MATI/area/data.txt" );

	while( !ifs.eof() )
	{
		std::string str;
		ifs >> str;
		if( str.find( "IMO" ) != std::string::npos )
		{
			area->Push( str.c_str() );
			int time;
			ifs >> time;
			event_list->push( new Event_open_area( time * 1000, area, 0 ) );
		}
	}

}

Stage *stage;