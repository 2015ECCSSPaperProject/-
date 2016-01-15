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

	// éÜîjÇË
	AnimationUV* scatter;

	// ínñ 
	//AnimationUV* wave;

	// è’åÇîg
	//AnimationUV* impact;
	//float impactScale;
	////bool isimpact = false;
	//AnimationUV* impactFar;

	//// ó≥ä™
	//AnimationUV* wind;
	//float windScale;

	//AnimationUV* locus;
};
