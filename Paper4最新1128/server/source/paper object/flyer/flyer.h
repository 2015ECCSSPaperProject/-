
//#include "../paper object.h"

#pragma once

class Flyer : public Paper_obj_3DObj
{
private:
	static const float DIST;

public:
	Flyer();
	~Flyer();

	void Update();

	void Render();

	//**************************************************

	bool Can_do(BasePlayer *player);

	bool Can_rend();

	bool Can_dist(const Vector3 &pos, float dist);

	void Rend();

	//**************************************************

	int Get_animation_frame();

	//**************************************************

	unsigned int Get_send_data_size ();

	void Get_send_data ( char *out );
};
