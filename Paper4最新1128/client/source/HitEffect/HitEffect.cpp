
#include "HitEffect.h"


HitEffect::HitEffect()
{
	hitDamage = new AnimationRippleEx("DATA/effect/DamageEffect.png", 12, 3, 6, 20, -1.0f);
	hitSlash = new AnimationRippleEx("DATA/effect/Slash.png", 12, 3, 6, 60, -1.0f);
	hitFlash = new AnimationRippleEx("DATA/effect/flash.png", 7, 2, 3, 60, 2.0f);
	hitLight= new AnimationRippleEx("DATA/effect/light.png", 6, 2, 3, 40, 2.0f);

	sphere = new AnimationRippleEx("DATA/effect/sphere.png", 12, 3, 6, 15, 2.0f);
	
	pos = Vector3(0.0f, 0.0f, 0.0f);
	hitType = HIT_TYPE::DAMAGE;

}
HitEffect::~HitEffect()
{	
	SAFE_DELETE(hitDamage);
	SAFE_DELETE(hitSlash);
	SAFE_DELETE(hitFlash);
	SAFE_DELETE(hitLight);

	SAFE_DELETE(sphere);

}

void HitEffect::Update(Vector3 pos)
{
	this->pos = pos;
	hitDamage->Update();
	hitSlash->Update();
	hitFlash->Update();
	hitLight->Update();

	sphere->Update();
}

void HitEffect::Render()
{
	// タイプによる分岐
	if (hitType&(BYTE)HIT_TYPE::DAMAGE)
	{
		hitDamage->Render(pos, RS_ADD);
	}
	if (hitType&(BYTE)HIT_TYPE::SLASH)
	{
		hitSlash->Render(pos, RS_ADD);
	}
	if (hitType&(BYTE)HIT_TYPE::FLASH)
	{
		hitFlash->Render(pos, RS_ADD);
	}
	if (hitType&(BYTE)HIT_TYPE::CROSSLIGHT)
	{
		hitLight->Render(pos, RS_ADD);
	}

	//switch (hitType)
	//{
	//case DAMAGE:
	//	hitDamage->Render(pos, RS_ADD);
	//	break;
	//case SLASH:
	//	hitSlash->Render(pos, RS_ADD);
	//	break;
	//case ALL:
	//	hitDamage->Render(pos, RS_ADD);
	//	hitSlash->Render(pos, RS_ADD);
	//	break;
	//default:
	//	break;
	//}


	// 波紋
	sphere->Render(pos, RS_ADD);


}

void HitEffect::Action(int type)
{
	// タイプ設定
	hitType = type;

	// ランダムで2D回転
	float RandAngle = (rand() / 6.28f - 3.14f);

	hitDamage->GetObj()->SetAngle(RandAngle);
	hitSlash->GetObj()->SetAngle(RandAngle);
	hitFlash->GetObj()->SetAngle(RandAngle);

	hitDamage->Action();
	hitSlash->Action();
	hitFlash->Action();
	hitLight->Action();
	sphere->Action();
}