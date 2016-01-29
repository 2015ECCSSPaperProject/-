#include "FanEffect.h"

/*�藠���N���X*/

FanEffect::FanEffect()
{
	uvFan = new AnimationUV("DATA/uvAnim/Fan.imo", -0.016f, 0.028f, 114, true);
	uvFan->Action();
	uvFan2 = new AnimationUV("DATA/uvAnim/Fan.imo", 0.021f, 0.016f, 114, true);
	uvFan2->Action();
}

FanEffect::~FanEffect()
{
	SAFE_DELETE(uvFan);
	SAFE_DELETE(uvFan2);
}

void FanEffect::Update(Vector3 pos, float scale)
{
	// �G�t�F�N�g�X�V
	uvFan->Update(pos, VECTOR_ZERO, scale);
	uvFan2->Update(pos, VECTOR_ZERO, scale);
}

void FanEffect::Render()
{
	uvFan->Render();
	uvFan2->Render();
}

void FanEffect::Action()
{
	// �A�N�V����
	uvFan->Action();
	uvFan2->Action();
}

void FanEffect::Stop()
{
	uvFan->Stop();
	uvFan2->Stop();
}