
#pragma once

#include "iextreme.h"
#include <vector>

class iexMesh;
class BasePlayer;
class Area_mng;
class Inhabitants;

class Stage
{
public:
	Stage();
	~Stage();

	void Initialize();
	void Release();
	void Update();
	void Render(iexShader *shader = nullptr, char *name = '\0');
	void RenderForward();


	bool Collision_rand(const Vector3 &pos, Vector3 *move, float up);

	void Collision(const Vector3 &pos, Vector3 *move, float radius, // îºåa
		int recursive_counter); // çƒãAêî

	unsigned int Area_Get_numof();
	void Area_Is_work(unsigned int index, bool in);

private:
	std::vector<iexMesh*>show_model, collision_model;

	float Length( Vector3 &pos, Vector3 &v );

	Area_mng *area;

	void Load_mapdata();

	void Load_area();

	Inhabitants *inhabitants;
};

extern Stage *stage;
