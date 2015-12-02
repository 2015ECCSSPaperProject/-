
#pragma once

#include "iextreme.h"

class iexMesh;
class BasePlayer;
class Area_mng;

class Stage
{
private:
	iexMesh *show_model, *collision_model;

	float Length(Vector3 &pos, Vector3 &v);

	Area_mng *area;

public:
	Stage();
	~Stage();

	void Initialize();
	void Release();
	void Update();
	void Render(iexShader *shader = nullptr, char *name = '\0');

	bool Collision_rand(const Vector3 &pos, Vector3 *move, float up);

	void Collision(const Vector3 &pos, Vector3 *move, float radius, // îºåa
		int recursive_counter); // çƒãAêî

	unsigned int Area_Get_numof();
	void Area_Is_work(unsigned int index, bool in);
};

extern Stage *stage;
