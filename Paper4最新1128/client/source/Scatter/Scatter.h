#pragma once

#include "iextreme.h"
#include "../Deferred/Deferred.h"
#include "../Animation/AnimationUV.h"

class AnimationUV;

class Scatter
{
public:
	Scatter();
	~Scatter();

	void Update(Vector3 pos, Vector3 PlayerVec, float scale = 1.0f);
	void Render();

	void Action();
private:

	// ���j��
	AnimationUV* scatter;

	// �n��
	//AnimationUV* wave;

	// �Ռ��g
	//AnimationUV* impact;
	//float impactScale;
	////bool isimpact = false;
	//AnimationUV* impactFar;

	//// ����
	//AnimationUV* wind;
	//float windScale;

	//AnimationUV* locus;
};
