#pragma once

#include	"../system/Scene.h"
#include	"../../../share_data/Enum_public.h"

class PaperClient;
class Thread;
class AnimationRippleEx;

class SceneResult : public Scene
{
public:
	//初期化・解放
	bool Initialize();
	~SceneResult();
	//処理
	bool Update();
	//描画
	void Render();

	enum STEP
	{
		START = 0, MAIN = 1, END = 2
	};
	int step;//ステップ
	int timer;

private:
	// 画像
	iexView* view;
	enum IMAGE{ BACK,WIN_BACK,LOSE_BACK,NORMAL_BACK, R1, R2, R3, R4, R5, R6, P1, P2, P3, P4, P5, P6, KEKKA, ACTION, NUMBER,OME, MAX };
	iex2DObj* image[IMAGE::MAX];

	//===============================================
	//	アニメーションUI
	//===============================================
	int MoveX[PLAYER_MAX];	// それぞれのランキング
	int active_num;			// 参加してる人数


	// よろこんだりするキャラ
	struct
	{
		Vector3 pos;
		float angle;
		iex3DObj* obj;
		int motion_no;
	}chara;

	// ランキングの順位
	struct data
	{
		int score;
		int p_num;
	}datas[PLAYER_MAX];
	void Set_ranking();
	int Get_rank(int num)
	{
		for (int i = 0; i < PLAYER_MAX; i++)
		{
			if (datas[i].p_num == num) return i;
		}
		return -1;
	}
	
	PaperClient* net;

	//---------------------------------------------------------------------
	//   Shader&Poster
	//---------------------------------------------------------------------
	iex2DObj* posterScreen[PLAYER_MAX];
	iex2DObj* posterFrame[PLAYER_MAX];

	Surface* backbuffer;// サーフェイス　
	iex2DObj* screen;

	//---------------------------------------------------------------------
	//   Screen
	//---------------------------------------------------------------------
	iex3DObj* breakScreen;
	iex3DObj* normalScreen;



	

};
