#include	"../system/Framework.h"
//net
#include	"../Net/PaperServer.h"
#include	"SceneMainServer.h"

#include	"../system/Thread.h"
#include	"../Player/BasePlayer.h"
#include	"../Player/NetPlayer.h"

#include	"../stage/Stage.h"

using namespace std;

// アラミタマ
iex3DObj *set;

//******************************************************************
//		初期化・解放
//******************************************************************
SceneMainServer::SceneMainServer()
{
	//	環境設定
	iexLight::SetAmbient(0x808080);
	iexLight::SetFog( 800, 3000, 0 );

	// Fade処理
	FadeControl::Setting(FadeControl::FADE_IN, 26);

	view = new iexView();
	view->Set(Vector3(0, 600, -60), Vector3(0, 0, 0));
	view->Activate();

	//stage = new iexMesh2("DATA/BG/stage_puroto.imo");
	stage = new Stage;
	stage->Initialize();

	set = new iex3DObj("DATA/CHR/player/run.IEM");

	//　プレイヤー初期化
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		player[i] = new NetPlayer(i);
		player[i]->Initialize(set);
	}
	

	//	スレッド開始
	m_pThread = 0;
	isRun = true;

	m_pThread = new Thread(ThreadFunc, this);
	m_pThread->Run();

}

bool SceneMainServer::Initialize()
{

	////	環境設定
	//iexLight::SetAmbient(0x808080);
	//iexLight::SetFog( 800, 3000, 0 );

	//// Fade処理
	//FadeControl::Setting(FadeControl::FADE_IN, 26);

	//view = new iexView();
	//view->Set(Vector3(0, 100, -60), Vector3(0, 0, 0));
	//view->Activate();

	//stage = new iexMesh2("DATA/BG/stage_puroto.imo");

	////	スレッド開始
	//m_pThread = 0;
	//isRun = true;

	//m_pThread = new Thread(ThreadFunc, this);
	//m_pThread->Run();


	return true;

}


//===================================================================================
//    スレッド
//===================================================================================
void SceneMainServer::ThreadFunc(void* pData, bool*isEnd)
{
	SceneMainServer* pMulti((SceneMainServer*)pData);

	//　サーバー初期化
	ServerManager::Init();


	/*フレームワーク*/
	DWORD dwFrameTime(clock());
	while (pMulti->isRun)
	{
		/*スレッド終了*/
		if (*isEnd)
			return;

		///*フレーム制限*/
		//DWORD CurrentTime = clock() * 10;
		//if (CurrentTime < dwFrameTime + 167) // 0.0167f (1フレーム)
		//	continue;

		///*経過時間*/
		//DWORD	dTime = CurrentTime - dwFrameTime;
		//if (dTime > 2000) dwFrameTime = CurrentTime;

		//dwFrameTime += 167;

		/*ネットワーク更新*/
		ServerManager::Update();
	}

	//ServerManager::Release();
}

SceneMainServer::~SceneMainServer()
{
	
	delete view;
	delete stage;
	
	//isRun = false;
	//　今ここで消す
	ServerManager::Release();
	delete m_pThread;

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		SAFE_DELETE(player[i]);
	}

	delete set;
}


//******************************************************************
//		処理
//******************************************************************
void SceneMainServer::Update()
{

	//ナンバーエフェクト
	Number_Effect::Update();

	//フェード処理
	FadeControl::Update();

	stage->Render();

	// サーバー
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		player[i]->Update();
	}
	
}



//******************************************************************
//		描画
//******************************************************************

void SceneMainServer::Render()
{

	view->Clear();

	stage->Render();

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		player[i]->Render();
		Text::Draw(1000, 50 + (i * 40), 0xffff00ff, "pos.x->%.2f", player[i]->Get_pos().x);

	}

	// サーバー描画
	ServerManager::Render();

	//ナンバーエフェクト
	Number_Effect::Render();

	//フェード処理
	FadeControl::Render();

}