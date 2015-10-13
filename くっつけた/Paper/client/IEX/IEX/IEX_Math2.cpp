
#include "IEX_Math2.h"
#include "iextreme.h"

void Quaternion_rotate(Quaternion* out, Vector3 axis, float radian)
{
	radian *= 0.5f;
	float s = sinf(radian);
	out->x = axis.x * s;
	out->y = axis.y * s;
	out->z = axis.z * s;
	out->w = cosf(radian);
}
