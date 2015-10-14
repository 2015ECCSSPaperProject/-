#include	"iextreme.h"

float Number_Effect::x[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
float Number_Effect::y[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int Number_Effect::Num[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
bool Number_Effect::isAlive[] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };
D3DCOLOR Number_Effect::Color[] = { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff };
int Number_Effect::Alpha[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int Number_Effect::kind[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


/*	数値表示エフェクト	*/

void Number_Effect::Update()
{
	for (int i = 0; i < NUM_MAX; i++)
	{
		if (isAlive[i] == true)
		{

			Alpha[i] += 10;
			if (Alpha[i] > 255){ Alpha[i] = 255;	isAlive[i] = false; }
			else
			{
				y[i] -= 2.0f;
			}
		}
		if (isAlive[i] == false)
		{
			Alpha[i] -= 5;
			if (Alpha[i] < 0){ Alpha[i] = 0; }
		}

		if (kind[i] == 0)
		{
			//色
			Color[i] = D3DCOLOR_ARGB(Alpha[i], 255, 55, 95);
		}
		else if (kind[i] == 1)
		{
			//色
			Color[i] = D3DCOLOR_ARGB(Alpha[i], 155, 155, 255);
		}
		else if (kind[i] == 3)
		{
			//色
			Color[i] = D3DCOLOR_ARGB(Alpha[i], 155, 255, 155);
		}
		else if (kind[i] == 2)
		{
			//色
			Color[i] = D3DCOLOR_ARGB(Alpha[i], 255, 255, 155);
		}
		else
		//色
		Color[i] = D3DCOLOR_ARGB(Alpha[i], 0, 200, 255);//水色　魂
		

	}
}

void Number_Effect::SetNum(float X, float Y, int NUM, int KIND)
{
	for (int i = 0; i < NUM_MAX; i++)
	{
		if (isAlive[i] == true)continue;

		x[i] = X; y[i] = Y; Num[i] = NUM;
		kind[i] = KIND;

		Alpha[i] = 0;
		isAlive[i] = true;

		break;
	}
}

void Number_Effect::Render()
{
	//if (isAlive == false)return;
	for (int i = 0; i < NUM_MAX; i++)
	{
		if (kind[i] == NOM_COLOR::SPIRIT)//　魂マークありかなしか
			Number::Render(x[i], y[i], Num[i], Color[i], Number::SPIRIT);
		else
			Number::Render(x[i], y[i], Num[i], Color[i], Number::PLUS);
		
	}

}



/*ナンバークラス*/
/*数字を描画する*/

iex2DObj* Number::handle=NULL;
 int Number::digit[] = {0};

 void Number::Init()
 {
	 handle = new iex2DObj("DATA/Number/Number.png");

 };

 void Number::CleanUp()
 {
	 delete handle;
 };

 void Number::Render(float x, float y, int num, D3DCOLOR col, KIND kind)
 {
	 int nnn = num;
	 int number = 0;
	 int COUNT = 0;

	 for (int i = 0; i < DIGIT_MAX; i++)
	 {
		 number = nnn % 10;//一番小さい桁を入手
		 nnn = (int) nnn / 10;//数値の一番小さい桁を消す
		 
		 digit[i] = number;//桁順に数値を入れる

		 COUNT++;//レンダーで回すカウント
		 if (nnn <= 0)break;
	 }

	 /*その数値の種類により描画の仕方を変える*/
	 switch (kind)
	 {
	 case Number::NORMAL:
		 for (int i = 0; i < COUNT; i++)
		 {
			 handle->Render((int)x - (i * 32), (int)y, 64, 64, digit[i] * 64, 0, 64, 64, NULL, col);//
		 }
		 break;
	 case Number::PLUS:
		 for (int i = 0; i < COUNT; i++)
		 {
			 handle->Render((int)x - (i * 26), (int)y, 64, 64, digit[i] * 64, 0, 64, 64, NULL, col);//
		 }
		 handle->Render((int)x - (COUNT * 26), (int)y, 64, 64, 10 * 64, 0, 64, 64, NULL, col);//+
		 break;
	 case Number::POINT:

		 handle->Render((int)x + 26, (int)y, 64, 64, 11 * 64, 0, 64, 64, NULL, col);//P
		 for (int i = 0; i < COUNT; i++)
		 {
			 handle->Render((int)x - (i * 26), (int)y, 64, 64, digit[i] * 64, 0, 64, 64, NULL, col);//
		 }
		 break;
	 case Number::MINUS:
		 for (int i = 0; i < COUNT; i++)
		 {
			 handle->Render((int)x - (i * 32), (int)y, 64, 64, digit[i] * 64, 0, 64, 64, NULL, col);//
		 }
		 handle->Render((int)x - (COUNT * 32), (int)y, 64, 64, 12 * 64, 0, 64, 64, NULL, col);//-
		 break;
	 case Number::TIME:

		 for (int i = 0; i < COUNT; i++)
		 {
			 handle->Render((int)x - (i * 30), (int)y, 64, 64, digit[i] * 64, 0, 64, 64, NULL, col);//
		 }
		 handle->Render((int)x - (COUNT * 32), (int)y, 64, 64, 13 * 64, 0, 64, 64, NULL, col);//:

		 //for (int i = 0; i < COUNT; i++)
		 //{
			// handle->Render((int)x - (i * 32), (int)y, 64, 64, digit[i] * 64, 0, 64, 64, NULL, col);//
		 //}
		 //handle->Render((int)x - (COUNT * 32), (int)y, 64, 64, 13 * 64, 0, 64, 64, NULL, col);//:
		
		 //for (int i = 0; i < COUNT; i++)
		 //{
			// handle->Render((int)x - (i * 32), (int)y, 64, 64, digit[i] * 64, 0, 64, 64, NULL, col);//
		 //}

		 break;
	 case Number::SPIRIT:
		 handle->Render((int)x + 32, (int)y+8, 64, 64, 14 * 64, 0, 64, 64, NULL, col);//魂
		 for (int i = 0; i < COUNT; i++)
		 {
			 handle->Render((int)x - (i * 26), (int)y, 64, 64, digit[i] * 64, 0, 64, 64, NULL, col);//
		 }
		 handle->Render((int)x - (COUNT * 26), (int)y, 64, 64, 10 * 64, 0, 64, 64, NULL, col);//+
		 break;

	 default:
		 break;
	 }


 };
