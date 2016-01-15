
#include "Area.h"
#include "../../IEX/iextreme.h"
#include "../timer/Timer.h"
#include "../Animation/AnimationUV.h"

class Area
{
public:
	Area();
	Area(const char *filename, int time);
	~Area();

	void Set_mesh(const char *filename);

	int RayPick(Vector3* out, Vector3* pos, Vector3* vec, float *Dist);

	void Close(); // •ÇÝ’u
	void Open(); // •Ç‰ð•ú

	void Render(iexShader *shader = nullptr, char *name = '\0');

	bool Is_work();

	int time_to_break; // Á‚¦‚éŽžŠÔ

protected:
	bool is_work; // ”»’è‚ð‚Æ‚é‚©
	AnimationUV *wall;
};

Area::Area() : wall(nullptr), is_work(true)
{}

Area::Area(const char *filename, int time) : wall(nullptr), is_work(true)
{
	Set_mesh(filename);
	time_to_break = time;
}

Area::~Area()
{
	delete wall;
}

void Area::Set_mesh(const char *filename)
{
	delete wall;
	char fn[128];
	strcpy_s( fn, filename );
	wall = new AnimationUV(fn, 0, 0.005f, 100, true);
	wall->Action();
}

int Area::RayPick(Vector3* out, Vector3* pos, Vector3* vec, float *Dist)
{
	if (!is_work || !wall)
		return -1;
	return wall->GetObj()->RayPick(out, pos, vec, Dist);
}

void Area::Close()
{
	is_work = true;
}
void Area::Open()
{
	is_work = false;
}

void Area::Render(iexShader *shader, char *name)
{
	if (is_work && wall){
		wall->Update();
		wall->Render_AT();
	}
}

bool Area::Is_work()
{
	return is_work;
}



Area_mng::~Area_mng()
{
	for (unsigned int i = 0; i < area_array.size(); i++)
		delete area_array[i];
}

void Area_mng::Push(const char *filename, int time)
{
	area_array.push_back(new Area(filename, time));
}

void Area_mng::Open(int index)
{
	area_array[index]->Open();
}

void Area_mng::Render(iexShader *shader, char *name)
{
	for( unsigned int i = 0; i < area_array.size(); i++ )
	{
		if( area_array[i]->time_to_break > timer->Get_limit_time() )
		{
			area_array[i]->Open();
		}
	}

	if (shader)
	{
		for (unsigned int i = 0; i < area_array.size(); i++)
		{
			area_array[i]->Render(shader, name);
		}
	}
	else
	{
		for (unsigned int i = 0; i < area_array.size(); i++)
		{
			area_array[i]->Render();
		}
	}
}

int Area_mng::RayPick(Vector3* out, Vector3* pos, Vector3* vec, float *Dist)
{
	int ret(-1);
	Vector3 hit_pos;
	Vector3 v;
	Vector3 normal;
	float length(FLT_MAX);
	int work_ret;

	for (unsigned int i = 0; i < area_array.size(); i++)
	{
		v = *vec;
		length = *Dist;

		work_ret = area_array[i]->RayPick(&hit_pos, pos, &v, &length);
		if (work_ret != -1 && length < *Dist)
		{
			ret = work_ret;
			*out = hit_pos;
			normal = v;
			*Dist = length;
		}
	}

	*vec = normal;
	return ret;
}

int Area_mng::Get_num()
{
	return area_array.size();
}

void Area_mng::Set_Is_work(unsigned int index, bool in)
{
	if (area_array[index]->Is_work() && !in)
	{
		area_array[index]->Open();
	}
}
