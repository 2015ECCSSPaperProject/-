
#include	"AnimationUV.h"

/*
*
*	UVアニメーションクラス
*
*/

AnimationUV::AnimationUV(char* name, float moveTU, float moveTV, int EndFlame, bool AlphaFlag, int AlphaNear, bool IsRoop)
{
	obj = new iexMesh(name);

	tu = 0.0f; tv = 0.0f;
	moveTu = moveTU, moveTv = moveTV;
	endFlame = EndFlame;
	nowFlame = 0;			// 初期フレームを0に
	isAction = false;		// アニメ実行するか

	// α
	alphaFlag = AlphaFlag;
	alphaNear = AlphaNear;
	alpha = 1.0f;

	isRoop = IsRoop;
}

AnimationUV::~AnimationUV()
{
	delete obj;
}


void  AnimationUV::Action()
{
	isAction = true;//起動
	tu = 0.0f; tv = 0.0f;// 初期のuvに戻す
	nowFlame = 0;// 最初のフレームへ
}

void AnimationUV::Stop()
{
	isAction = false;//止める
	nowFlame = 0;
};


void AnimationUV::Update(Vector3 pos, Vector3 angle, float scale)
{
	if (isAction == false)return;//実行されてないなら出てけ！！

	// UV更新
	tu += moveTu;
	tv += moveTv;

	if (isRoop == false)//ループじゃなかったら
	{
		// フレーム更新
		nowFlame++;
		if (nowFlame >= endFlame)
		{
			isAction = false;
		}
	}

	// 透明度更新
	if (alphaFlag == true)
	{
		//ラムダ式Min~Maxの範囲に抑える　２～０
		auto Clamp = [](float val, float Min, float Max){
			return min(Max, max(val, Min));
		};

		// 0が透明　1が不透明にするように設定
		float A = (endFlame - nowFlame);
		float B =(endFlame - alphaNear);
		alpha = A / B;
		alpha = Clamp(alpha, 0.0f, 1.0f);

	}

	// メッシュの更新
	obj->SetScale(scale);
	obj->SetAngle(angle);
	obj->SetPos(pos);
	obj->Update();


}


void AnimationUV::Update(Vector3 pos, Vector3 angle, Vector3 scale)
{
	if (isAction == false)return;//実行されてないなら出てけ！！

	// UV更新
	tu += moveTu;
	tv += moveTv;

	if (isRoop == false)//ループじゃなかったら
	{
		// フレーム更新
		nowFlame++;
		if (nowFlame >= endFlame)
		{
			isAction = false;
		}
	}

	// 透明度更新
	if (alphaFlag == true)
	{
		//ラムダ式Min~Maxの範囲に抑える　２～０
		auto Clamp = [](float val, float Min, float Max){
			return min(Max, max(val, Min));
		};

		// 0が透明　1が不透明にするように設定
		float A = (endFlame - nowFlame);
		float B = (endFlame - alphaNear);
		alpha = A / B;
		alpha = Clamp(alpha, 0.0f, 1.0f);

	}

	// メッシュの更新
	obj->SetScale(scale);
	obj->SetAngle(angle);
	obj->SetPos(pos);
	obj->Update();

}

void AnimationUV::Render()
{
	if (isAction == false)return;//実行されてないなら出てけ！！

	// 描画の前に情報を送る
	//　シェーダーに描画毎UV座標を送る
	shaderD->SetValue("tuAnime", tu);
	shaderD->SetValue("tvAnime", tv);
	shaderD->SetValue("alphaUV", alpha);// 透明度

	// 描画
	obj->Render(shaderD, "uvAnime");

}


