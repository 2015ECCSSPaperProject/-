
#include "HitEffect.h"


HitEffect::HitEffect()
{
	hitefffect = new AnimationRippleEx("DATA/effect/DamageEffect.png", 12, 3, 6, 30, -1.0f);
	sphere = new AnimationRippleEx("DATA/effect/sphereBlack.png", 12, 3, 6, 10, 2.0f);
	pos = Vector3(0.0f, 0.0f, 0.0f);
}
HitEffect::~HitEffect()
{
	SAFE_DELETE(hitefffect);
	SAFE_DELETE(sphere);

}

void HitEffect::Update(Vector3 pos)
{
	this->pos = pos;

	hitefffect->Update();
	sphere->Update();
}

void HitEffect::Render()
{
	hitefffect->Render(pos, RS_ADD);
	sphere->Render(pos, RS_ADD);

}

void HitEffect::Action()
{
	hitefffect->Action();
	sphere->Action();
}