
#pragma once

struct Vector3;

class Poster_mng;
class BasePlayer;

class Paper_obj_mng
{
public:
	Paper_obj_mng();
	~Paper_obj_mng();

	void Initialize();

	void Update();

	void Render();

	int Can_do(BasePlayer *player);

	bool Can_rend(int index);

	const Vector3& Get_pos(int index);

	float Get_angle(int index);

	void Rend_poster(int number, int index);

	void Paper_obj_mng::Can_dist(const Vector3 &pos, float dist, int number, int out[]);

	Poster_mng *poster;
};

extern Paper_obj_mng *paper_obj_mng;
