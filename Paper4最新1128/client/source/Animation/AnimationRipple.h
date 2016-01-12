#pragma once
#include "iextreme.h"
#include "AnimationRipple.h"


class AnimationRipple
{
public:
	AnimationRipple(char* fileName, int MoveAlpha, float MoveScale);
	~AnimationRipple();


	void Update();
	void Render(int x,int y);

	void Action();

private:
	iex2DObj* image;

	int alpha;
	float scale;
	int moveAlpha;
	float moveScale;
	bool isAction;

};
