#pragma once

#include	"../system/Scene.h"
#include	"../../../share_data/Enum_public.h"

class PaperClient;
class Thread;
class AnimationRipple;
class AnimationRippleEx;
class Mouse;


class SceneSelect : public Scene
{
public:
	//初期化・解放
	bool Initialize();
	~SceneSelect();
	//処理
	void Update();
	//描画
	void Render();

	enum STEP
	{
		START_NO = 0, START_OK = 1, GAME = 2, GAME_FADE = 3
	};
	int step;//ステップ

private:
	iexView* view;

	enum IMAGE{ BACK, P1, P2, P3, P4, P5, P6, WAIT, OK, LIST, ACTION,NOPLAYER,WANTED,TEN,BACK2,INFO,CURSOR,JOIN,NOT_JOIN, MAX };
	iex2DObj* image[IMAGE::MAX];
	//---------------------------------------------------------------------
	//   ImageAnimation
	//---------------------------------------------------------------------
	bool isActivePlayer[PLAYER_MAX];
	int moveX[PLAYER_MAX];
	int alpha[PLAYER_MAX];
	AnimationRipple* IconRip[PLAYER_MAX];
	AnimationRipple* OKRip[PLAYER_MAX];
	bool OKFlag[PLAYER_MAX];

	// 参加したスキル
	AnimationRipple* JoinRip;
	AnimationRipple* InfoOKRip;
	//---------------------------------------------------------------------
	//   InfoAnimation
	//---------------------------------------------------------------------
	AnimationRippleEx* Info;
	AnimationRippleEx* InfoOK;
	AnimationRippleEx* InfoNO;
	bool onePlay;
	bool InfoFlag; //Infoを表示さすフラグ




	//================================
	//		ボタン
	struct Button
	{
		int dstX;
		int dstY;
		int dstW;
		int dstH;
		int srcX;
		int srcY;
		bool in;		// 画像範囲内
		iex2DObj *lpButton;
	};
	Button modoru;
	Button entry;
	Button entry2;
	Button OKInfo_B;
	Button NOInfo_B;

	void Initialize_buttons();
	void Update_buttons(const Vector2 &pos);

	struct
	{
		Vector3 pos;
		float angle;
		iex3DObj* obj;
	}chara;


	PaperClient* net;

	Thread* m_pThread;
	//---------------------------------------------------------------------
	//   thread
	//---------------------------------------------------------------------
	static void ThreadFunc(void* pData, bool*);

	//---------------------------------------------------------------------
	//   Shader&Poster
	//---------------------------------------------------------------------
	iex2DObj* posterScreen[PLAYER_MAX];
	iex2DObj* posterFrame[PLAYER_MAX];
	Surface* backbuffer;// サーフェイス　
	void PosterInit();
	void PosterRender(int i);

	//　スタンプの画像の2次元配列
	iex2DObj* StampPicture[KIND_MAX][NO_MAX];
	Mouse *mouse;

	// ブラ―用
	iex2DObj* Screen;
	iex2DObj* BlurScreenX;
	iex2DObj* BlurScreenY;
	float BlurValue;



};

extern bool SCOREATACK_FLAG;