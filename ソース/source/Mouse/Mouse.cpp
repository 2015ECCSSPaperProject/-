#include	"iextreme.h"
//#include<windows.h>
//#include<windef.h>
#include	"Mouse.h"

//=============================================================================================
//		staticメンバ変数
POINT Mouse::p;
RECT Mouse::rc;
float Mouse::axis_x;
float Mouse::axis_y;


//=============================================================================================
//		更		新
void Mouse::Update()
{
	static const float Max = 0.5f;
	static const float Min = 0.05f;

	//取得
	GetCursorPos(&p);
	GetWindowRect(iexSystem::Window, &rc);
	//窓位置と縁による調整
	int ScreenXpos = p.x - rc.left - 8;
	int ScreenYpos = p.y - rc.top - 29;
	////中央オフセット＆正規化
	axis_x = ((float)ScreenXpos - 640.0f) / 640.0f;
	axis_y = -(((float)ScreenYpos - 384.0f) / 384.0f);

	// 最大値制御
	if (axis_x > Max)axis_x = Max;
	else if (axis_x < -Max)axis_x = -Max;
	if (axis_y > Max)axis_y = Max;
	else if (axis_y < -Max)axis_y = -Max;

	// 最小値なら0
	float val_x = sqrtf(axis_x * axis_x), val_y = sqrtf(axis_y * axis_y);
	if (val_x < Min * 5) axis_x = 0;
	if (val_y < Min) axis_y = 0;
}
