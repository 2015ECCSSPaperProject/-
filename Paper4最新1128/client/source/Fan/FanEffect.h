#pragma once

#include "iextreme.h"
#include "../Deferred/Deferred.h"
#include "../Animation/AnimationUV.h"

class AnimationUV;

class FanEffect
{
public:
	FanEffect();
	~FanEffect();

	void Update(Vector3 pos,float scale);
	void Render();

	void Action();
	void Stop();

private:

	AnimationUV* uvFan;
	AnimationUV* uvFan2;

};
