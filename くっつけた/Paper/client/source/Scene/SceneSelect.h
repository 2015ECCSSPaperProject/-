#pragma once

#include	"../system/Scene.h"

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
	iexMesh* stage;
	iex2DObj* Select;

	PaperClient* net;

	Thread* m_pThread;
	//---------------------------------------------------------------------
	//   thread
	//---------------------------------------------------------------------
	static void ThreadFunc(void* pData, bool*);
};
