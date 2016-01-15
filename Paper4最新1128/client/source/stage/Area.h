
#pragma once

struct Vector3;
class Area;
class iexShader;

#include <vector>

class Area_mng
{
public:
	~Area_mng();

	void Push(const char *filename, int time ); // mesh‚Ì

	void Open(int index);

	void Render(iexShader *shader = nullptr, char *name = '\0');

	int RayPick(Vector3* out, Vector3* pos, Vector3* vec, float *Dist);

	int Get_num();

	void Set_Is_work(unsigned int index, bool in);

private:
	std::vector<Area*> area_array;
};
