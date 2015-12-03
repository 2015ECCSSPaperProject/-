
// #include "iextreme.h"
// #include "../paper object.h"

#pragma once

// Paper_objのテストで使ったクラス
class Test_box : public Paper_obj
{
public:
	enum class MODE
	{
		WAITE,
		REND,
		MAX
	}mode;
	const static unsigned int DIST = 10;

	void Initialize(int model_type, iex3DObj *model, int point);

	void Update();

	void Render();


	bool Can_do(BasePlayer *player);

	bool Can_rend();

	bool Can_dist(const Vector3 &pos, float dist);

	void Rend();

	int Get_animation_frame();
};
