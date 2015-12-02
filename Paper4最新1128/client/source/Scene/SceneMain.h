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

	// モード関係
	enum MODE{ START, MAIN, END, MAX };

private:
	iexView* view;
	iexMesh* sky;

	PaperClient* net;

	MODE mode;
	void Start();						// ゲーム始まって3秒待機
	void Main();						// 基本的なゲーム処理
	void End();							// ゲーム終了してちょっと待機してからリザルト行く処理
	void(SceneMain::*Mode_funk[MODE::MAX])();	// 関数ポインタ


	Thread* m_pThread;// スレッド
	//---------------------------------------------------------------------
	//   thread
	//---------------------------------------------------------------------
	static void ThreadFunc(void* pData, bool*);

	//---------------------------------------------------------------------
	//   DeferredShader
	//---------------------------------------------------------------------
	//Deferred* deferred;
	bool deferredFlag;
	iex2DObj* DebugTex;
	// Light 
	Vector3 LightVec;
	// 露出レベル
	float exposure;

	void DebugShaderCtrl();
	void RenderShadow();

	void SurfaceRender();
};

#endif // !_SCENEMAIN_H_