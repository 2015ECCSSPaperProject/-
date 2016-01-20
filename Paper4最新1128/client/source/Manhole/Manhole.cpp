#include "iextreme.h"
#include "Manhole.h"
#include "../fstream/fstream_paper.h"
void ManholeMng::Load(char *filename)
{
	std::ifstream infs(filename);
	int i = 0;
	// ˆÊ’u‚Æ‚©
	while (!infs.eof())
	{
		// ˆÊ’u
		infs >> data[0][i].pos;
		infs >> data[0][i + 1].pos;
		infs >> data[1][i].pos;
		infs >> data[1][i + 1].pos;
		float angle(0);
		infs >> angle;
		infs >> angle;
		i += 2;
	}
	infs.close();
}

ManholeMng::ManholeMng()
{
	//data[0][0].pos = Vector3(380, 0, -393.5f);
	//data[0][1].pos = Vector3(163.3f, 0, -519.2f);
	//data[1][0].pos = Vector3(392, -120, -411.2f);
	//data[1][1].pos = Vector3(174.8f, -120, -538);
	//
	//data[0][2].pos = Vector3(169, 0, 542);
	//data[0][3].pos = Vector3(365.7f, 0, 401);
	//data[1][3].pos = Vector3(390, -120, 412);
	//data[1][2].pos = Vector3(177, -120, 542);
	//
	//data[0][4].pos = Vector3(-532, 0, 120.8f);
	//data[0][5].pos = Vector3(-530, 0, -131);
	//data[1][4].pos = Vector3(-557.6f, -120, 118);
	//data[1][5].pos = Vector3(-552.4f, -120, -132.5f);
	Load("DATA/Manhole/set.txt");
}

int ManholeMng::CheckManhole(LAND_TYPE type, const Vector3 &pos, float dist)
{
	for (int i = 0; i < 6; i++)
	{
		if ((data[(int)type][i].pos - pos).Length() < dist)
		{
			return i;
		}
	}
	return -1;
}