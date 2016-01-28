#pragma once

#include "iextreme.h"
#include "../Deferred/Deferred.h"
#include "../Animation/AnimationRippleEx.h"

class AnimationRippleEx;



class HitEffect
{
public:
	enum HIT_TYPE
	{
		DAMAGE = 0x1,
		SLASH = 0x2,
		FLASH = 0x4,
		CROSSLIGHT = 0x8,
		ALL = 0x0f
	};

	HitEffect();
	~HitEffect();

	void Update(Vector3 pos);
	void Render();

	void Action(int type = HIT_TYPE::DAMAGE);
private:

	// HitType
	int hitType;

	// �|�W�V����
	Vector3 pos;

	// �q�b�g�̐�
	AnimationRippleEx* hitDamage;
	// ��
	AnimationRippleEx* hitSlash;
	// �t���b�V��
	AnimationRippleEx* hitFlash;
	// ���C�g
	AnimationRippleEx* hitLight;

	// �g��
	AnimationRippleEx* sphere;

	//AnimationUV* locus;
};
