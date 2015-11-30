
#pragma once

struct Vector3;

class BasePlayer;

class Paper_obj
{
public:
	Paper_obj();
	virtual ~Paper_obj();

	virtual void Initialize();

	virtual void Update();

	virtual void Render();

	virtual int Can_do(BasePlayer *player);

	virtual bool Can_rend(int index);

	virtual const Vector3& Get_pos(int index);

	virtual float Get_angle(int index);

	virtual void Rend_poster(int number, int index);

	virtual void Can_dist(const Vector3 &pos, float dist, int number, int out[]);

	virtual void Load_pos(char *filename){};
};
