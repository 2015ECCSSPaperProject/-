#include "iextreme.h"
#include "Mouse.h"

//*******************************************************************************************************************
//
//		※このクラスはタイトルとかセレクト等で使います。プレイヤーの移動には使ってません
//
//*******************************************************************************************************************

//=============================================================================================
//		マ	ウ	ス	更		新
void Mouse::Update()
{
	static const float Max = 0.6f;
	static const float Min = 0.25f;

	// 前回座標保存
	prev_point = current_point;

	// 取得
	GetCursorPos(&current_point);
	GetWindowRect(iexSystem::Window, &rc);
	// 窓位置と縁による調整
	pos.x = current_point.x - rc.left - 8;
	pos.y = current_point.y - rc.top - 29;
	////中央オフセット＆正規化
	axis_x = ((float)pos.x - (iexSystem::ScreenWidth / 2)) / (iexSystem::ScreenWidth / 2);
	axis_y = -((float)pos.y - (iexSystem::ScreenHeight / 2)) / (iexSystem::ScreenHeight / 2);

	// 最大値制御
	if (axis_x > Max)axis_x = Max;
	else if (axis_x < -Max)axis_x = -Max;
	if (axis_y > Max)axis_y = Max;
	else if (axis_y < -Max)axis_y = -Max;

	// 最小値なら0
	float val_x = sqrtf(axis_x * axis_x), val_y = sqrtf(axis_y * axis_y);
	if (val_x < Min) axis_x = 0;
	if (val_y < Min * 1.5f) axis_y = 0;
}
