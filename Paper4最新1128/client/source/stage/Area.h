
#pragma once

struct Vector3;
class Area;
class iexShader;

#include <vector>

class Area_mng
{
public:
	~Area_mng();

	void Push(const char *filename, int time ); // mesh��

	void Open(int index);

	void Update();

	void Render(iexShader *shader = nullptr, char *name = '\0');

	int RayPick(Vector3* out, Vector3* pos, Vector3* vec, float *Dist);

	int Get_num();

	void Set_Is_work(unsigned int index, bool in);

	// �E�H�[���Ƃ̍ŒZ������Ԃ��֐�(�ǉ����܂���)
	void Get_nearest_point(unsigned int index, Vector3 *out, const Vector3 &pos);

private:
	std::vector<Area*> area_array;
};
