#pragma once

#include "iextreme.h"
#include "../Deferred/Deferred.h"
#include "../Animation/AnimationRippleEx.h"

class AnimationRippleEx;

class HitEffect
{
public:
	HitEffect();
	~HitEffect();

	void Update(Vector3 pos);
	void Render();

	void Action();
private:

	// ポジション
	Vector3 pos;

	// ヒットの赤
	AnimationRippleEx* hitefffect;
	// ◎
	AnimationRippleEx* sphere;


	//AnimationUV* locus;
};
