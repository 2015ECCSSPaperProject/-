
#include "HitEffect.h"


HitEffect::HitEffect()
{
	hitDamage = new AnimationRippleEx("DATA/effect/DamageEffect.png", 12, 3, 6, 20, -1.0f);
	hitSlash = new AnimationRippleEx("DATA/effect/Slash.png", 12, 3, 6, 30, -1.0f);
	sphere = new AnimationRippleEx("DATA/effect/sphere.png", 12, 3, 6, 15, 2.0f);
	
	pos = Vector3(0.0f, 0.0f, 0.0f);
	hitType = HIT_TYPE::DAMAGE;

}
HitEffect::~HitEffect()
{	
	SAFE_DELETE(hitDamage);
	SAFE_DELETE(hitSlash);
	SAFE_DELETE(sphere);

}

void HitEffect::Update(Vector3 pos)
{
	this->pos = pos;
	hitDamage->Update();
	hitSlash->Update();
	sphere->Update();
}

void HitEffect::Render()
{
	// �^�C�v�ɂ�镪��
	switch (hitType)
	{
	case DAMAGE:
		hitDamage->Render(pos, RS_ADD);
		break;
	case SLASH:
		hitSlash->Render(pos, RS_ADD);
		break;
	case ALL:
		hitDamage->Render(pos, RS_ADD);
		hitSlash->Render(pos, RS_ADD);
		break;
	default:
		break;
	}


	// �g��
	sphere->Render(pos, RS_ADD);


}

void HitEffect::Action(HIT_TYPE type)
{
	// �^�C�v�ݒ�
	hitType = type;

	// �����_����2D��]
	float RandAngle = (rand() / 6.28f - 3.14f);

	hitDamage->GetObj()->SetAngle(RandAngle);
	hitSlash->GetObj()->SetAngle(RandAngle);

	hitDamage->Action();
	hitSlash->Action();
	sphere->Action();
}