#ifndef _SCENEMAIN_H_
#define _SCENEMAIN_H_

#include	"../../../share_data/Enum_public.h"
#include	"../system/Scene.h"

class ShadowMap;
class Stage;
class Thread;

class SceneMainServer : public Scene
{
public:
	//初期化・解放
	SceneMainServer();
	bool Initialize();

	~SceneMainServer();
	//処理
	void Update();
	//描画
	void Render();
	
private:
	iexView* view;


	//	通信用
	bool isRun;
	Thread* m_pThread;
	//---------------------------------------------------------------------
	//   thread
	//---------------------------------------------------------------------
	static void ThreadFunc(void* pData, bool*);

	//---------------------------------------------------------------------
	//   ポスター
	//---------------------------------------------------------------------
	iex2DObj* posterScreen[PLAYER_MAX];
	iex2DObj* posterFrame[PLAYER_MAX];

	Surface* backbuffer;// サーフェイス　
	void PosterInit();
	void PosterRender(int i);

	//　スタンプの画像の2次元配列
	iex2DObj* StampPicture[KIND_MAX][NO_MAX];
};

#endif // !_SCENEMAIN_H_