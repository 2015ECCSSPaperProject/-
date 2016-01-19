#include "Rush.h"

/*手裏剣クラス*/

Rush::Rush()
{
	rushWave = new AnimationUV("DATA/uvAnim/windCut3.imo", 0.02f, -0.08f, 90, false, 1, 65);
	rushBoost = new AnimationUV("DATA/uvAnim/boost.imo", 0.08f, 0.0f, 90, false, 1, 85);
	//impact = new AnimationUV("DATA/uvAnim/impact.imo", 0.02f, 0.035, 5 * 3, true, 5 * 2.2);
	//impactFar = new AnimationUV("DATA/uvAnim/impact.imo", 0.02f, 0.035, 5 * 3, true, 5 * 2.2);
	//impactScale = 1.0f;

	wind = new AnimationUV("DATA/uvAnim/windFlont.imo", -0.075f, -0.003f, 90, false, 5, 55);
	wind2 = new AnimationUV("DATA/uvAnim/windFlont.imo", -0.06f, -0.003f, 90, false, 5, 55);
	windScale = 0.03f;
	windMove = 1.0f;
}

Rush::~Rush()
{
	SAFE_DELETE(rushWave);
	SAFE_DELETE(rushBoost);
	//SAFE_DELETE(impact);
	//SAFE_DELETE(impactFar);
	SAFE_DELETE(wind);
	SAFE_DELETE(wind2);
}

void Rush::Update(Vector3 pos, Vector3 flontVec, float dist, Vector3 objAngle)
{
	Vector3 vectorZ = Vector3(0.0f, 0.0f, 1.0f);// angle = flontVec;
	float dot = Vector3Dot(vectorZ, flontVec);
	Vector3 jiku;
	Vector3Cross(jiku, vectorZ, flontVec);


	// 拡大速度
	//impactScale += 0.02f;
	windScale += 0.0045f;
	windMove -= (1.0f / 70.0f);
	// エフェクト更新
	rushWave->Update(pos + (flontVec * dist), objAngle, Vector3(0.3f, 0.3f, 0.5f));
	rushBoost->Update(pos + (flontVec * dist*1.5f) + Vector3(0, -15, 0), objAngle, 0.5f);
//	impact->Update(pos + (flontVec * dist), VECTOR_ZERO, Vector3(impactScale, impactScale*2.5f, impactScale));
//	impactFar->Update(pos + (flontVec * dist), VECTOR_ZERO, Vector3(impactScale*2.5f, impactScale*1.0f, impactScale*2.5f));
	
	wind->Update(pos + ((flontVec*dist * 3.1f)*windMove), objAngle, Vector3(windScale, windScale, windScale*2.0f));
	wind2->Update(pos + ((flontVec*dist * 3)*windMove) , objAngle, Vector3(windScale, windScale, windScale*2.0f));
}

void Rush::Render()
{
	// アクション
	rushWave->Render();
	rushBoost->Render();
	//impact->Render();
	//impactFar->Render();
	wind->Render();
	wind2->Render();
}

void Rush::Action()
{
	//impactScale = 0.2f;
	// アクション
	rushWave->Action();
	rushBoost->ActionRoop();
	wind->Action();
	wind2->Action();
	windMove = 1.0f;
	windScale = 0.03f;
}

void Rush::Stop()
{
	// アクション
	rushWave->StopRoop();
	rushBoost->StopRoop();
	wind->Stop();
	wind2->Stop();




}