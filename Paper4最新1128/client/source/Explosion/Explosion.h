#pragma once

#include "iextreme.h"
#include "../Deferred/Deferred.h"
#include "../Animation/AnimationUV.h"

class AnimationUV;

class Explosion
{
public:
	Explosion();
	~Explosion();

	void Update(Vector3 pos, Vector3 flontVec = Vector3(0.0f, 0.0f, 0.0f), float dist = 25.0f);
	void Render();

	void Action();
private:

	// �n��
	AnimationUV* wave;	
	
	// �Ռ��g
	AnimationUV* impact;
	float impactScale;
	//bool isimpact = false;
	AnimationUV* impactFar;
	
	// ����
	AnimationUV* wind;
	float windScale;

	//AnimationUV* locus;
};
