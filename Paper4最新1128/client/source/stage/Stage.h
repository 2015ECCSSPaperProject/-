
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
	void Render_collision_model( iexShader *shader, char *name ); // 影とか
	void RenderForward();
	void Render_inhabitants( iexShader *shader, char *name );


	bool Collision_rand(const Vector3 &pos, Vector3 *move, float up);

	void Collision(const Vector3 &pos, Vector3 *move, float radius, // 半径
		int recursive_counter); // 再帰数

	unsigned int Area_Get_numof();
	void Area_Is_work(unsigned int index, bool in);
	
	// ウォールとの最短距離を返す関数(追加しました)
	void Area_Get_nearest_point(unsigned int index, Vector3 *out, const Vector3 &pos);

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
