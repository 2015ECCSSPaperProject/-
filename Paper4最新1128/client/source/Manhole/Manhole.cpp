#include "iextreme.h"
#include "Manhole.h"

ManholeMng::ManholeMng()
{
	data[0][0].pos = Vector3(380, 0, -393.5f);
	data[0][1].pos = Vector3(163.3f, 0, -519.2f);
	data[1][0].pos = Vector3(392, -120, -411.2f);
	data[1][1].pos = Vector3(174.8f, -120, -538);

	data[0][2].pos = Vector3(-204, 0, 401);
	data[0][3].pos = Vector3(-422, 0, 524.7f);
	data[1][2].pos = Vector3(-188.2f, -120, 428.4f);
	data[1][3].pos = Vector3(-407.5f, -120, 549.3f);

	data[0][4].pos = Vector3(-532, 0, 120.8f);
	data[0][5].pos = Vector3(-530, 0, -131);
	data[1][4].pos = Vector3(-557.6f, -120, 118);
	data[1][5].pos = Vector3(-552.4f, -120, -132.5f);
}

bool ManholeMng::CheckManhole(LAND_TYPE type, const Vector3 &pos, float dist, Vector3 *out_pos)
{
	for (int i = 0; i < 6; i++)
	{
		if ((data[(int)type][i].pos - pos).Length() < dist)
		{
			// 地下なら地上の、地上なら地下の座標を返す
			*out_pos = data[(int)type ^ 0x1][i].pos;
			return true;
		}
	}
	return false;
}