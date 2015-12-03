#include "Explosion.h"

/*爆発クラス*/

Explosion::Explosion()
{
	wave = new AnimationUV("DATA/uvAnim/wave.imo", 0.00f, 0.018f, 20 * 3, true, 20 * 0.5f);

	impact = new AnimationUV("DATA/uvAnim/impact.imo", 0.02f, 0.035f, 5 * 3, true, 5 * 2.2f);
	impactFar = new AnimationUV("DATA/uvAnim/impact.imo", 0.02f, 0.035f, 5 * 3, true, 5 * 2.2f);
	impactScale = 1.0f;

	wind = new AnimationUV("DATA/uvAnim/wind2.imo", -0.07f, 0.009f, 15 * 3, true, 15 * 2.2f);
	windScale = 1.0f;
}

Explosion::~Explosion()
{
	SAFE_DELETE(wave);
	SAFE_DELETE(impact);
	SAFE_DELETE(impactFar);
	SAFE_DELETE(wind);
}

void Explosion::Update(Vector3 pos, Vector3 flontVec, float dist)
{
	// 拡大速度
	impactScale += 0.02f;
	windScale += 0.02f;

	// エフェクト更新
	wave->Update(pos + (flontVec * dist), VECTOR_ZERO, 0.5f);
	impact->Update(pos + (flontVec * dist), VECTOR_ZERO, Vector3(impactScale, impactScale*2.5f, impactScale));
	impactFar->Update(pos + (flontVec * dist), VECTOR_ZERO, Vector3(impactScale*2.5f, impactScale*1.0f, impactScale*2.5f));
	wind->Update(pos + (flontVec * dist), VECTOR_ZERO, Vector3(windScale, 0.6f, windScale));
}

void Explosion::Render()
{
	// アクション
	wave->Render();
	impact->Render();
	impactFar->Render();
	wind->Render();
}

void Explosion::Action()
{
	impactScale = 0.2f;
	windScale = 0.6f;

	// アクション
	wave->Action();
	impact->Action();
	impactFar->Action();
	wind->Action();

}