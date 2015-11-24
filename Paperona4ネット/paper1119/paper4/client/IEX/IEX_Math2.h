
#ifndef IEX_MATH2_H
#define IEX_MATH2_H

struct Quaternion;
struct Vector3;

float Radian_to_degree(float rad);
float Degree_to_radian(float deg);


void Quaternion_rotate(Quaternion* out, Vector3 axis, // ³‹K‰»
	float radian); // -ƒÎ ~ ƒÎ

#endif // !IEX_MATH2_H
