
#include "Area.h"
#include "../../IEX/iextreme.h"

class Area
{
public:
	Area();
	Area(char *filename);
	~Area();

	void Set_mesh(char *filename);

	int RayPick(Vector3* out, Vector3* pos, Vector3* vec, float *Dist);

	void Close(); // •ÇÝ’u
	void Open(); // •Ç‰ð•ú

	void Render();

	bool Is_work();

protected:
	bool is_work; // ”»’è‚ð‚Æ‚é‚©
	iexMesh *wall;
};

Area::Area() : wall(nullptr), is_work(true)
{}

Area::Area(char *filename) : wall(nullptr), is_work(true)
{
	Set_mesh(filename);
}

Area::~Area()
{
	delete wall;
}

void Area::Set_mesh(char *filename)
{
	delete wall;
	wall = new iexMesh(filename);
}

int Area::RayPick(Vector3* out, Vector3* pos, Vector3* vec, float *Dist)
{
	if (!is_work || !wall)
		return -1;
	return wall->RayPick(out, pos, vec, Dist);
}

void Area::Close()
{
	is_work = true;
}
void Area::Open()
{
	is_work = false;
}

void Area::Render()
{
	if (is_work && wall)
		wall->Render();
}

bool Area::Is_work()
{
	return is_work;
}



Area_mng::~Area_mng()
{
	for (int i = 0; i < area_array.size(); i++)
		delete area_array[i];
}

void Area_mng::Push(char *filename)
{
	area_array.push_back(new Area(filename));
}

void Area_mng::Open(int index)
{
	area_array[index]->Open();
}

void Area_mng::Render()
{
	for (int i = 0; i < area_array.size(); i++)
	{
		area_array[i]->Render();
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

	for (int i = 0; i < area_array.size(); i++)
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

int Area_mng::Get_numof()
{
	return area_array.size();
}

bool Area_mng::Is_work(unsigned int index)
{
	return area_array[index]->Is_work();
}
