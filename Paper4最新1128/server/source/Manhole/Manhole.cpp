#include "iextreme.h"
#include "Manhole.h"

ManholeMng::ManholeMng()
{
	data[0][0].pos = Vector3(380, 0, -393.5f);
	data[0][1].pos = Vector3(163.3f, 0, -519.2f);
	data[1][0].pos = Vector3(392, -120, -411.2f);
	data[1][1].pos = Vector3(174.8f, -120, -538);
	data[0][0].angle = 4.2f;
	data[0][1].angle = data[0][0].angle - PI;
	data[1][0].angle = data[1][1].angle = 2.7f + PI;

	data[0][2].pos = Vector3(169, 0, 549);
	data[0][3].pos = Vector3(365.7f, 0, 401);
	data[1][3].pos = Vector3(390, -120, 412);
	data[1][2].pos = Vector3(177, -120, 542);
	data[0][2].angle = 2.1f;
	data[0][3].angle = data[0][2].angle + PI;
	data[1][2].angle = data[1][3].angle = 2.6f;

	data[0][4].pos = Vector3(-532, 0, 120.8f);
	data[0][5].pos = Vector3(-530, 0, -131);
	data[1][4].pos = Vector3(-557.6f, -120, 118);
	data[1][5].pos = Vector3(-552.4f, -120, -132.5f);
	data[0][4].angle = PI;
	data[0][5].angle = data[0][4].angle - PI;
	data[1][4].angle = data[1][5].angle = PI*.5f;
}

bool ManholeMng::CheckManhole(LAND_TYPE type, float dist, Vector3 *out_pos, float *out_angle, Vector3 *out_next_pos)
{
	for (int i = 0; i < 6; i++)
	{
		if ((data[(int)type][i].pos - *out_pos).Length() < dist)
		{
			*out_pos = data[(int)type][i].pos;
			*out_angle = data[(int)type][i].angle;
			*out_next_pos = data[(int)type ^ 0x1][i].pos;
			return true;
		}
	}
	return false;
}