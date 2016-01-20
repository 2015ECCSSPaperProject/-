#include "SkillBegin.h"


/**************************/
//	スキル仕様エフェクト
/**************************/

SkillBegin::SkillBegin()
{
	sphere = new AnimationRippleEx("DATA/effect/sphere.png", 18, 3, 6, 10, 4.0f);;
	shine = new AnimationRippleEx("DATA/effect/flash.png", 16, 2, 6, 30, 2.0f);;
	ring = new AnimationUV("DATA/uvAnim/wave2.imo", 0.00f, 0.008f, 18, false, 2, 10);
	ring2 = new AnimationUV("DATA/uvAnim/wave2.imo", 0.00f, 0.008f, 18, false, 2, 10);
	pos = VECTOR_ZERO;
	RandAngle = VECTOR_ZERO;
	RandAngle2 = VECTOR_ZERO;

	// 色
	//sphere->GetObj()->SetARGB(27, 255, 55, 255);
	//shine->GetObj()->SetARGB(27, 255, 55, 255);

}

SkillBegin::~SkillBegin()
{
	SAFE_DELETE(sphere);
	SAFE_DELETE(shine);
	SAFE_DELETE(ring);
	SAFE_DELETE(ring2);
}

void SkillBegin::Update(Vector3 Pos)
{
	pos = Pos; //場所更新

	sphere->Update();
	shine->Update();
	// エフェクト更新
	ring->Update(pos, RandAngle, 0.3f);
	ring2->Update(pos, RandAngle2, 0.3f);

}

void SkillBegin::Render()
{
	sphere->Render(pos, RS_ADD);
	shine->Render(pos, RS_ADD);
	//ring->Render();
	//ring2->Render();

}


void SkillBegin::Action()
{
	// Action
	sphere->Action();
	shine->Action();
	ring->Action();
	ring2->Action();

	// ランダム
	RandAngle = Vector3((rand() / 6.24f - 3.14f), (rand() / 6.24f - 3.14f), (rand() / 6.24f - 3.14f));
	RandAngle2 = Vector3((rand() / 6.24f - 3.14f), (rand() / 6.24f - 3.14f), (rand() / 6.24f - 3.14f));
}
