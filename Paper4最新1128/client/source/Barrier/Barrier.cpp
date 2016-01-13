#include "Barrier.h"

/************************************/
//
//		バリアエフェクト
//
/************************************/

Barrier::Barrier()
{
	// バリアー
	barrier = new AnimationUV("DATA/uvAnim/barrier.IMO", 0.0025f, -0.0075, 60*20, false, 10, 200);

}

Barrier::~Barrier()
{
	delete barrier;
}



void Barrier::Update(Vector3 Pos, Vector3 ViewVec, float Scale)
{
	// ゲーム画面の→　と　ゲーム画面の↑　を指定
	//Vector3 right(matView._11, matView._21, matView._31);// +で→　-で←
	//Vector3 up(matView._12, matView._22, matView._32);// +で↑　-で↓
	Vector3 flont(matView._13, matView._23, matView._33);// +で↑　-で↓
	//right.Normalize();
	//up.Normalize();
	flont.Normalize();

	// オーラの位置←ビュー座標
	//Vector3 to_view(-matView._31, 0, -matView._33);
	float bill = atan2f(-flont.x, -flont.z);

	//bill = Vector3Dot(to_view, Vector3(0, 0, 1));

	barrier->Update(Pos, Vector3(0,bill,0), Scale);
}

void Barrier::Render()
{
	// バリアーバージョンのUV描画
	barrier->Render_Barrier();
	//barrier->Render();
}

void Barrier::Action()
{
	barrier->Action();
}

void Barrier::Stop()
{
	barrier->Stop();
}