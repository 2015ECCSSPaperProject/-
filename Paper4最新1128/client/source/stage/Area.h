
#pragma once

struct Vector3;
class Area;

#include <vector>

class Area_mng
{
public:
	~Area_mng();

	void Push(char *filename); // mesh‚Ì

	void Open(int index);

	void Render();

	int RayPick(Vector3* out, Vector3* pos, Vector3* vec, float *Dist);

	int Get_num();

protected:
	std::vector<Area*> area_array;
};
