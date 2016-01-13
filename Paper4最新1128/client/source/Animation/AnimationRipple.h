#pragma once
#include "iextreme.h"
#include "AnimationRipple.h"


class AnimationRipple
{
public:
	AnimationRipple(char* fileName, int MoveAlpha, float MoveScale);
	~AnimationRipple();


	void Update();
	void Render(int x,int y,DWORD dwFlag=RS_COPY);
	void Render(int x, int y, int w, int h, int tx, int ty, int tw, int th, DWORD dwFlag = RS_COPY);

	void Action();

private:
	iex2DObj* image;

	int alpha;
	float scale;
	int moveAlpha;
	float moveScale;
	bool isAction;

};
