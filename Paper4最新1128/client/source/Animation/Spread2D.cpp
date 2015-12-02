#include	"iextreme.h"
#include	"Spread2D.h"

/*	スケールを利用したアニメーション　*/

Spread2D::Spread2D(char* name, int XY_SIZE, int Maxcount, int speed, float Add_Scale, D3DCOLOR Start_Col)
{
	handle = new iex2DObj(name);

	COUNT_MAX = Maxcount;
	FLAME_MAX = speed;
	AddScale = Add_Scale;
	SIZE = XY_SIZE;
	//初期化
	scale = 1.0f;//初期スケール
	StartCol = Start_Col;
	col = StartCol;//初期の色
	count = 0;//0にもどす
	flame = 0;
	isAction = false;//アニメを実行するスイッチ

}

Spread2D::~Spread2D()
{
	delete handle;
}

void Spread2D::Action()
{
	isAction = true;//起動

	scale = 1.0f;//初期スケール
	col = StartCol;//初期の色
	count = 0;//0にもどす
	flame = 0;
};

void Spread2D::Update()
{
	if (isAction == false)return;//実行されてないなら出てけ！！

	flame++;
	if (flame >= FLAME_MAX)
	{
		flame = 0;
		count++;

		scale += AddScale;//拡大
		col -= 0x11000000;//透明にしていく
	}

	if (count == COUNT_MAX)
	{
		count = 0;//0にもどす
		flame = 0;
		isAction = false;
	}

};

void Spread2D::Render(float X, float Y, u32 dwFlag)
{
	if (isAction == false)return;//実行されてないなら出てけ！！

	handle->Render_scale(scale, (int)X, (int)Y, SIZE, SIZE, 0, 0, SIZE, SIZE, dwFlag, col);

};