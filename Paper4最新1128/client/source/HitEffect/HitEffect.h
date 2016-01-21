#pragma once

#include "iextreme.h"
#include "../Deferred/Deferred.h"
#include "../Animation/AnimationRippleEx.h"

class AnimationRippleEx;

enum HIT_TYPE
{
	DAMAGE = 0, SLASH = 1, ALL = 2
};

class HitEffect
{
public:
	HitEffect();
	~HitEffect();

	void Update(Vector3 pos);
	void Render();

	void Action(HIT_TYPE type = HIT_TYPE::DAMAGE);
private:

	// HitType
	int hitType;

	// �|�W�V����
	Vector3 pos;

	// �q�b�g�̐�
	AnimationRippleEx* hitDamage;
	// ��
	AnimationRippleEx* sphere;
	// ��
	AnimationRippleEx* hitSlash;

	//AnimationUV* locus;
};
