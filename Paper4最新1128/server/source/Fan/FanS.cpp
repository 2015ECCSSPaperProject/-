#include "iextreme.h"
#include "../fstream/fstream_paper.h"
#include "FanS.h"

void FanMng::Load(char *filename)
{
	std::ifstream infs(filename);

	Vector3 pos_array[32];
	float dist_array[32];
	num_fan = 0;

	while (!infs.eof())
	{
		infs >> pos_array[num_fan];
		infs >> dist_array[num_fan];
		num_fan++;
	}
	infs.close();

	fan_pos = new Vector3[num_fan];
	up_dist = new float[num_fan];

	// ˆÊ’u‚Æ‚©
	for (int i = 0; i < num_fan; i++)
	{
		fan_pos[i] = pos_array[i];
		up_dist[i] = dist_array[i];
	}

}

FanMng::FanMng()
{
	Load("DATA/Fan/set.txt");
}
FanMng::~FanMng()
{
	delete fan_pos;
	delete up_dist;
}

int FanMng::CheckFan(float dist, const Vector3 &pos)
{
	for (int i = 0; i < num_fan; i++)
	{
		// x‚Æz‚Å‚Æ‚é(‰~’Œ)
		if ((pos.x - fan_pos[i].x)*(pos.x - fan_pos[i].x) + (pos.z - fan_pos[i].z)*(pos.z - fan_pos[i].z) < dist*dist)
		{
			return i;
		}
	}
	return -1;
}

bool FanMng::CheckHeight(int no, float pos_y)
{
	assert(no != -1);
	// x‚Æz‚Å‚Æ‚é(‰~’Œ)
	if (pos_y - fan_pos[no].y > up_dist[no])
	{
		return false;
	}
	return true;
}