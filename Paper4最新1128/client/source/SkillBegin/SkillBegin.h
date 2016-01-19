#pragma once

#include "iextreme.h"
#include "../system/System.h"
#include "../Animation/AnimationRippleEx.h"
#include "../Animation/AnimationUV.h"

class AnimationRippleEx;
class AnimationUV;

class SkillBegin
{
public:
	SkillBegin();
	~SkillBegin();

	void Update(Vector3 Pos);
	void Render();

	void Action();

private:
	AnimationRippleEx* sphere;	// Åù
	AnimationRippleEx* shine;	// åı
	AnimationUV* ring;		// ÉäÉìÉO
	AnimationUV* ring2;		// ÉäÉìÉO2

	Vector3 pos;
	Vector3 RandAngle;
	Vector3 RandAngle2;

	bool isAction;
};



