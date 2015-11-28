
#include "IEX_Math2.h"
#include "iextreme.h"

float Radian_to_degree(float rad)
{
	return rad * 28.6478901f;
}
float Degree_to_radian(float deg)
{
	return deg * 0.0349065848f;
}

void Quaternion_rotate(Quaternion* out, Vector3 axis, float radian)
{
	radian *= 0.5f;
	float s = sinf(radian);
	out->x = axis.x * s;
	out->y = axis.y * s;
	out->z = axis.z * s;
	out->w = cosf(radian);
}
