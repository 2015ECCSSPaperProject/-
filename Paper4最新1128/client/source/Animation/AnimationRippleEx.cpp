
#include "AnimationRippleEx.h"

AnimationRippleEx::AnimationRippleEx(char* name, int EndFlame, int alphaNearFlame, int alphaFarFlame, float StartScale, float ScaleMove, bool IsRoop)
{
	obj = new iex2DObj(name);

	//pos = VECTOR_ZERO;
	isAction = false;
	nowFlame = 0;
	endFlame = EndFlame;

	alphaNear = alphaNearFlame;
	alphaFar = alphaFarFlame;
	alpha = 0.0f;

	// スケール
	scale = 1.0f;
	startScale = StartScale;
	scaleMove = ScaleMove;

	isRoop = IsRoop; //ループするか
}

AnimationRippleEx::~AnimationRippleEx()
{
	delete obj;
}

void AnimationRippleEx::Action()
{
	isAction = true;
	nowFlame = 0;
	scale = startScale;// スケールを初期位置へ
}

void AnimationRippleEx::Stop()
{
	isAction = false;
	nowFlame = 0;
}

void AnimationRippleEx::Update()
{
	if (isAction == false)return;//実行されてないなら出てけ！！


	// フレーム更新
	nowFlame++;
	if (isRoop == true)// ループだったらalphaFarの前のとこでループし続ける処理
	{
		if (nowFlame >= (alphaFar - 1))
		{
			nowFlame = (alphaFar - 1);
			return;
		}
	}
	// エンドフレームまで来たら終わる
	if (nowFlame >= endFlame)
	{
		isAction = false;
	}

	// 拡大率更新
	scale += scaleMove;


	// 透明度更新

	//ラムダ式Min~Maxの範囲に抑える　２～０
	auto Clamp = [](float val, float Min, float Max){
		return min(Max, max(val, Min));
	};
	// α二アーの前か後ろで判定を変える
	if (nowFlame >= alphaNear)
	{
		// 100-100=0  100-50=50   0/50
		float A = ((float)endFlame - nowFlame);
		float B = ((float)endFlame - alphaFar);
		alpha = A / B;
	}
	else
	{
		// 最初の
		alpha = (float)nowFlame / (float)alphaNear;//   0/30=0   60/30=2   1-(0~1)  
	}

	alpha = Clamp(alpha, 0.0f, 1.0f);//指定された値を 0 ～ 1 の範囲にクランプします

	obj->SetScale(scale);
	obj->SetARGB((int)(alpha * 255), 255, 255, 255);

}

void AnimationRippleEx::Render(Vector3 pos, DWORD dwFlag)
{
	if (isAction == false)
	{
		int i = 0;
		return;//実行されてないなら出てけ！！
	}

	obj->Render3D(pos, dwFlag);
}

void AnimationRippleEx::Render(float x, float y, DWORD dwFlag)
{
	if (isAction == false)return;//実行されてないなら出てけ！！

	obj->Render((int)x, (int)y, dwFlag);
}