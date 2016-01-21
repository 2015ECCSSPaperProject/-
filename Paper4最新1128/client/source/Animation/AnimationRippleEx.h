#pragma once
#include "iextreme.h"
#include "../system/System.h"

class AnimationRippleEx
{
public:
	AnimationRippleEx(char* name, int endFlame, int alphaNearFlame, int alphaFarFlame, float StartScale, float ScaleMove, bool isRoop = false);
	~AnimationRippleEx();

	void Action();
	void Stop();

	void Update();
	void Render(Vector3 pos, DWORD dwFlag);
	void Render(float x, float y, DWORD dwFlag);

	bool isDoing(){ return isAction; }
	iex2DObj* GetObj(){ return obj; };
private:
	iex2DObj* obj;
	
	//Vector3 pos;

	int			endFlame;		// 描画終りのフレーム
	int			nowFlame;		// 今のフレーム

	int			alphaNear;		// 透明じゃなくなる　始めるフレーム
	int			alphaFar;		// 透明じゃなくなる　終わるフレーム　
	float		alpha;			// 透明度

	float scale;				// スケール
	float startScale;			// 最初のスケール
	float scaleMove;			// スケールをどう動かすか

	bool isAction;
	bool isRoop;				// ループするか
};
