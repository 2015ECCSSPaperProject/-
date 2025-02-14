
#pragma once

class iexMesh;
class BasePlayer;
class Area_mng;

class Stage
{
public:
	Stage();
	~Stage();

	void Initialize();
	void Release();
	void Update();
	void Render();

	bool Collision_rand(const Vector3 &pos, Vector3 *move);

	bool Collision_up( const Vector3 &pos, Vector3 *move, float high);

	bool Collision(const Vector3 &pos, Vector3 *move, float radius, // 半径
		int recursive_counter, // 再帰数
		Vector3 *n	// 当たった壁の法線		追加しました
		);

	unsigned int Area_Get_numof();
	bool Area_Is_work(unsigned int index);

private:
	iexMesh *collision_model;

	float Length( Vector3 &pos, Vector3 &v );

	Area_mng* area;

	enum : int
	{
		COLLISION_UP_SHIFT = 8 // 当たり判定で原点を上にずらす距離
	};

	void Load_area();
};

extern Stage *stage;
