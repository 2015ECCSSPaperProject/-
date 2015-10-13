#include	"iextreme.h"

//netは一番初めにインクルードしないとエラーが起こる
#include	"../Net/Socket.h"
//#include	"../Net/net_config_loader.h"

#include	"../system/Framework.h"
#include	"../system/Thread.h"

#include	"SceneMain.h"
#include	"../Player/BasePlayer.h"
#include	"../Player/MyPlayer.h"
#include	"../Player/NetPlayer.h"
//net
//#include "Network/TCPServerSocket.h"
//#include "Network/net_config_loader.h"


//#include <process.h>
//#include <thread>
//#include <vector>
//#include <queue>
//#include <mutex>
//#include <condition_variable>
//#include <atomic>
//#include <WS2tcpip.h>

#include	"../bench/Bench_mark.h"
extern Bench_mark bench;

// アラミタマ
iexMesh *set;

#include "../camera/Camera.h"
#include "../stage/Stage.h"


using namespace std;

//******************************************************************
//		初期化・解放
//******************************************************************
bool SceneMain::Initialize()
{

	//	環境設定
	iexLight::SetAmbient(0x808080);
	iexLight::SetFog(800, 3000, 0);

	// Fade処理
	FadeControl::Setting(FadeControl::FADE_IN, 26);

	//view = new iexView();
	//view->Set(Vector3(0, 10, -60), Vector3(0, 10, 0));
	camera = new Camera;


	//stage = new iexMesh2("DATA/BG/stage_puroto.imo");
	stage = new Stage;
	stage->Initialize();

	sky = new iexMesh("DATA/Skydome/Skydome.IMO");
	sky->SetScale(2.0f);
	sky->SetPos(0, -100, 0);
	sky->Update();

	set = new iexMesh("DATA/a.IMO");

	//■■■　相手と自分で分ける
	for (int i = 0; i <PLAYER_MAX; ++i)
	{
		if (i == SOCKET_MANAGER->GetID()){
			player[i] = new MyPlayer();
			camera->Initialize(player[i]);	// カメラ設定
		}
		else
			player[i] = new NetPlayer();

		player[i]->Initialize(set);
	}

	// 開始フラグを送る
	m_pThread = new Thread(ThreadFunc, this);

	//	同期をとる
	SOCKET_MANAGER->InitGame();//ここで

	//	ネットワークを開始する
	m_pThread->Run();


	return true;


}

SceneMain::~SceneMain()
{
	delete camera;
	//delete view;
	delete stage;
	delete sky;
	delete m_pThread;//　なんかスレッド消さないとエラー起こる

	delete set;

	for (int i = 0; i <PLAYER_MAX; ++i)
	{
		SAFE_DELETE(player[i]);
	}
}

//===================================================================================
//   スレッド
//===================================================================================
void SceneMain::ThreadFunc(void* pData, bool*isEnd)
{
	SceneMain* pMulti((SceneMain*)pData);

	/*フレームワーク*/
	DWORD dwFrameTime(clock());
	while (true)
	{
		/*スレッド終了*/
		if (*isEnd)
			return;

		/*フレーム制限*/
		DWORD CurrentTime = clock() * 10;
		if (CurrentTime < dwFrameTime + 167)
			continue;

		/*経過時間*/
		DWORD	dTime = CurrentTime - dwFrameTime;
		if (dTime > 2000) dwFrameTime = CurrentTime;

		dwFrameTime += 167;

		/*ネットワーク更新*/
		static int FLAME = 0;
		FLAME++;
		//if (FLAME > 6)
		{
			FLAME = 0;
			SOCKET_MANAGER->UpdateUser();
		}

	}
}

//******************************************************************
//		処理
//******************************************************************
void SceneMain::Update()
{
	//　プレイヤー
	for (int i = 0; i < PLAYER_MAX; ++i)
	{
		//　ここのながれ一番重要
		//　送られてきた情報を各プレイヤーにセット！！
		PlayerData sendPlayer = SOCKET_MANAGER->GetPlayer(i);
		player[i]->Set_pos(sendPlayer.pos);
		player[i]->Set_angleY(sendPlayer.angleY);

		player[i]->Update();
	}

	// カメラ
	camera->Update();

	//ナンバーエフェクト
	Number_Effect::Update();

	//フェード処理
	FadeControl::Update();

	//　仮の処理
	static float angle = 0;
	angle += 0.0007f;
	sky->SetAngle(angle);
	sky->Update();

}



//******************************************************************
//		描画
//******************************************************************

void SceneMain::Render()
{
	//view->Activate();
	//view->Clear();
	camera->Activate();
	camera->Clear();

	stage->Render();
	sky->Render();

	//　プレイヤー
	for (int i = 0; i < PLAYER_MAX; ++i)
	{
		player[i]->Render();
		Text::Draw(1100, 20 + (i * 32), 0xff00ffff, "pos.x->%.2f", player[i]->Get_pos().x);

		Text::Draw(950, 20 + (i * 32), 0xff00ffff, "名前：%s", SOCKET_MANAGER->GetUser(i).name);
	}


	Text::Draw(100, 20, 0xff00ffff, "受信時間%.2f", bench.Get_time());



	//ナンバーエフェクト
	Number_Effect::Render();

	//フェード処理
	FadeControl::Render();

}