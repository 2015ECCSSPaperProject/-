#ifndef _SCENEMAIN_H_
#define _SCENEMAIN_H_

#include	"../system/Scene.h"

class PaperClient;
class Thread;

class Stage;

class SceneMain : public Scene
{
public:
	//初期化・解放
	bool Initialize();
	~SceneMain();
	//処理
	void Update();
	//描画
	void Render();
	
private:
	iexView* view;
	Stage* stage;
	iexMesh* sky;

	PaperClient* net;

	Thread* m_pThread;// スレッド
	//---------------------------------------------------------------------
	//   thread
	//---------------------------------------------------------------------
	static void ThreadFunc(void* pData, bool*);

};

#endif // !_SCENEMAIN_H_