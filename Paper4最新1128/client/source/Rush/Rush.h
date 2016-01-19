#pragma once

#include "iextreme.h"
#include "../Deferred/Deferred.h"
#include "../Animation/AnimationUV.h"

class AnimationUV;

class Rush
{
public:
	Rush();
	~Rush();

	void Update(Vector3 pos, Vector3 flontVec = Vector3(0.0f, 0.0f, 0.0f), float dist = 25.0f, Vector3 objAngle = Vector3(0.0f, 0.0f, 0.0f));
	void Render();

	void Action();
	void Stop();

private:

	// rushWave
	AnimationUV* rushWave;

	// boost
	AnimationUV* rushBoost;

	// è’åÇîg
	//AnimationUV* impact;
	//float impactScale;
	////bool isimpact = false;
	//AnimationUV* impactFar;

	// ó≥ä™
	AnimationUV* wind;
	AnimationUV* wind2;
	float windMove;
	float windScale;

	//AnimationUV* locus;
};
