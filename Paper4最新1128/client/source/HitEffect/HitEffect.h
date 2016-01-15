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

	// �|�W�V����
	Vector3 pos;

	// �q�b�g�̐�
	AnimationRippleEx* hitefffect;
	// ��
	AnimationRippleEx* sphere;


	//AnimationUV* locus;
};
