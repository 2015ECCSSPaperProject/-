#pragma once

/*	スケールを利用したアニメーション　*/

class Spread2D
{
private:
	iex2DObj* handle;
	int SIZE;//画像の縦横
	float scale;
	float AddScale;
	D3DCOLOR col;
	D3DCOLOR StartCol;
	//アニメに必要な変数
	int COUNT_MAX;
	int FLAME_MAX;
	int count;
	int flame;

	bool isAction;
public:
	//画像の名前　 画像サイズ※正方形限定　最大カウント数　フレームの速度　一回に増えるスケール 最初の色
	Spread2D(char* name, int XY_SIZE, int Maxcount, int Flame, float Add_Scale, D3DCOLOR Start_Col=0xffffffff);
	~Spread2D();

	void Action();

	void Update();
	void Render(float X, float Y, u32 dwFlag=RS_COPY);

};
