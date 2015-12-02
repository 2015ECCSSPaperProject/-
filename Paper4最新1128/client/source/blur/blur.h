#pragma once

/*ブラ―フィルター.h*/

class BlurFilter
{
private:
	static iex2DObj* screen;
	static float BluePower;
	static Surface* savebackbuffer;// 保存用サーフェイス
	BlurFilter();
	~BlurFilter();
public:
	static void Init();/*初期化*/
	static void CleanUp();/*解放*/
	
	static void Update();/*更新*/
	static void Set(float power, float x, float y);/*ブラ―効果を発動*/
	
	static void Start_Copy();
	static void End_Copy();

	static void Render();

	// 追加
	static void Set_YBlur(float power);/*ブラ―効果を発動*/
	static void Render_YBlur();
};