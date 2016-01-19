
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

	bool Collision(const Vector3 &pos, Vector3 *move, float radius, // ���a
		int recursive_counter, // �ċA��
		Vector3 *n	// ���������ǂ̖@��		�ǉ����܂���
		);

	unsigned int Area_Get_numof();
	bool Area_Is_work(unsigned int index);

private:
	iexMesh *collision_model;

	float Length( Vector3 &pos, Vector3 &v );

	Area_mng* area;

	enum : int
	{
		COLLISION_UP_SHIFT = 4 // �����蔻��Ō��_����ɂ��炷����
	};

	void Load_area();
};

extern Stage *stage;
