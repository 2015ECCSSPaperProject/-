#pragma once

#include	"../system/Scene.h"
#include	"../../../share_data/Enum_public.h"

class PaperClient;
class Thread;

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
		START_NO = 0, START_OK = 1, GAME = 2
	};
	int step;//ステップ

private:
	iexView* view;
	iex2DObj* Select;

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

};
