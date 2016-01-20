
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
	void Render( iexShader *shader, char *name );
	void Render_culling( const Vector3 &pos, const Vector3 &front, iexShader *shader, char *name );
	void Render_collision_model( iexShader *shader, char *name ); // âeÇ∆Ç©
	void RenderForward();
	void Render_inhabitants( iexShader *shader, char *name );


	bool Collision_rand(const Vector3 &pos, Vector3 *move, float up);

	void Collision(const Vector3 &pos, Vector3 *move, float radius, // îºåa
		int recursive_counter); // çƒãAêî

	unsigned int Area_Get_numof();
	void Area_Is_work(unsigned int index, bool in);

private:
	struct Show_model_part
	{
		float size;
		iexMesh *model;

		Show_model_part( iexMesh *model );
		~Show_model_part();
	};
	std::vector<Show_model_part*>show_model;
	std::vector<iexMesh*>collision_model;

	float Length( Vector3 &pos, Vector3 &v );

	Area_mng *area;

	void Load_mapdata();

	void Load_area();

	Inhabitants *inhabitants;
};

extern Stage *stage;
