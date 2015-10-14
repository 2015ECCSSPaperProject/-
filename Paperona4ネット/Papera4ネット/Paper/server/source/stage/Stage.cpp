
#include "Stage.h"
//#include "../player/Player.h"

Stage::Stage() : show_model(nullptr), collision_model(nullptr)
{}

Stage::~Stage()
{
	Release();
}

void Stage::Initialize()
{
	// 仮

	show_model = new iexMesh("DATA/MATI/stage_machi.IMO");
	collision_model = new iexMesh("DATA/MATI/stage_atari.IMO");

	//show_model = new iexMesh("./DATA/Stage/MATI/stage_machi.IMO");
	//collision_model = new iexMesh("./DATA/Stage/MATI/stage_atari.IMO");
}

void Stage::Release()
{
	delete show_model; show_model = nullptr;
	delete collision_model; collision_model = nullptr;
}

void Stage::Update()
{

}

void Stage::Render()
{
	if (show_model)
		show_model->Render();
}

// 当たり判定

bool Stage::Collision_rand(const Vector3 &pos, Vector3 *move, float up)
{
	if (move->y >= 0)
		return false;

	Vector3 hit_pos, ray_pos(pos), vec(0, -1, 0);
	float dist(FLT_MAX);
	// xzだけ移動
	ray_pos += *move;
	ray_pos.y = pos.y + 3.0f;

	if (collision_model->RayPickUD(&hit_pos, &ray_pos, &vec, &dist) == -1)
		return false;

	if (pos.y + move->y >= hit_pos.y)
		return false;

	move->y = hit_pos.y - pos.y;
	return true;
}

void Stage::Collision(const Vector3 &pos, Vector3 *move, float radius, int recursive_counter)
{
	if (recursive_counter <= 0)
		return;

	Vector3 move_xz(move->x, 0, move->z); // moveのxとzだけ

	Vector3 hit_pos, ray_pos, vec;
	float dist(FLT_MAX);
	// レイの始点を上げる
	ray_pos = pos;
	ray_pos.y += 1.0f;
	// レイの方向を決める
	vec = move_xz;
	vec.y = 0;
	vec.Normalize();

	if (collision_model->RayPick(&hit_pos, &ray_pos, &vec, &dist) == -1)
		return;

	Vector3 next_pos(pos + move_xz);
	// 壁にめり込んだ量
	Vector3 sink(next_pos - hit_pos);
	// sinkの法線方向の大きさ
	float sink_nl;
	vec.y = 0;
	vec.Normalize();
	sink_nl = -Vector3Dot(sink, vec);

	if (sink_nl + radius <= 0)
		return;

	sink_nl += radius;
	*move += vec * sink_nl;

	Collision(pos, move, radius, recursive_counter - 1);
}


Stage *stage;