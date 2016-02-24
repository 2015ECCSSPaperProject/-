#pragma once

/*	スケールを利用したアニメーション　*/

class ScaleMove2D
{
private:
	iex2DObj* handle;
	float scale;				// 実際に画像で出すスケール
	float default_scale;		// 基本スケール(変動する中心)
	float fluctuation_range;	// 変動幅
	float add_scale;			// 増減幅
	enum ADD_FLAG
	{
		NONE,		// 何もしない
		INCREASE,	// 増やす
		DECREASE	// 減らす
	}add_flag;
public:
	//画像の名前　 基本画像サイズ　変動幅　一回に増減するスケール
	ScaleMove2D(char* name, float DefaultScale, float FluctuationRange, float AddScale);
	~ScaleMove2D();
	void Action();
	void Stop();
	void Update();
	void Render(int dstX, int dstY, u32 dwFlag = RS_COPY);

	void SetDefaultScale(float s){ default_scale = s; }
	void SetFluctuationRange(float f){ fluctuation_range = f; }
	void SetAddScale(float s){ add_scale = s; }
};
