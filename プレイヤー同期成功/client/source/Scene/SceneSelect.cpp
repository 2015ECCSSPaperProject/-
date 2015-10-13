
#include	"iextreme.h"

//netは一番初めにインクルードしないとエラーが起こる
#include	"../Net/Socket.h"
//#include	"../Net/net_config_loader.h"

#include	"../system/Framework.h"
#include	"../system/Thread.h"

#include	"SceneSelect.h"
#include	"SceneMain.h"

/**********************/
/*	グローバル変数	　*/
/**********************/

//　スレッドを止める為に仮で作った
//static int ThreadEND = false;


using namespace std;

//******************************************************************
//		初期化・解放
//******************************************************************
bool SceneSelect::Initialize()
{

	//	環境設定
	iexLight::SetAmbient(0x808080);
	iexLight::SetFog(800, 3000, 0);

	// Fade処理
	FadeControl::Setting(FadeControl::FADE_IN, 26);

	view = new iexView();
	view->Set(Vector3(0, 30, -60), Vector3(0, 0, 0));

	stage = new iexMesh("DATA/BG/stage_puroto.imo");

	Select = new iex2DObj("DATA/select.png");

	if (!SOCKET_MANAGER->Init())
	{
		MessageBox(0, "サーバーとの接続ができません", null, MB_OK);
		exit(0);
		return false;
	}

	//　名前送る
	SOCKET_MANAGER->SendName();

	//　ここのステップが STEP::START_OK が出たら
	step = STEP::START_NO;

	m_pThread = 0;

	//	スレッド開始
	//ThreadEND = false;
	m_pThread = new Thread(ThreadFunc, this);
	m_pThread->Run();

	return true;
}

SceneSelect::~SceneSelect()
{
	//ThreadEND = true;

	delete view;
	delete stage;
	delete Select;

	delete m_pThread;
}

//===================================================================================
//   スレッド
//===================================================================================
void SceneSelect::ThreadFunc(void* pData, bool*isEnd)
{
	SceneSelect* pMulti((SceneSelect*)pData);

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
		switch (pMulti->step)
		{
		case SceneSelect::STEP::START_NO:
			//　セレクト画面の更新
			SOCKET_MANAGER->UpdateTeam(false);

			break;
		case SceneSelect::STEP::START_OK:
			//　セレクト画面の更新
			SOCKET_MANAGER->UpdateTeam(true);

			break;
		case SceneSelect::STEP::GAME:
			//　セレクト画面の更新
			SOCKET_MANAGER->UpdateTeam(2);

			break;
		}
		
	}
}



//******************************************************************
//		処理
//******************************************************************
void SceneSelect::Update()
{


	//ナンバーエフェクト
	Number_Effect::Update();

	//フェード処理
	FadeControl::Update();

	switch (step)
	{
	case STEP::START_NO:
		//　Aボタン押したら
		if (KEY_Get(KEY_ENTER, 0) == 3)
		{
			step = STEP::START_OK;
		}

		break;
	case STEP::START_OK:
	{
		   //　まだ準備できてないので戻ります
		   if (KEY_Get(KEY_ENTER, 0) == 3)
		   {
			   step = STEP::START_NO;
		   }

		   //　追記：同期して
		   // 皆isResdy==2だったらゲーム画面へ！！
		 //  enum { READY = 2 };
		   //int count(0);
		   //int active(0);
		   //for (int i = 0; i < PLAYER_MAX; ++i)
		   //{
			  // if (SOCKET_MANAGER->GetUser(i).com == UserData::ACTIVE_USER)
				 //  ++active;

			  // // isReadyが全員OKになってたら　カウントするよ
			  // if (SOCKET_MANAGER->GetUser(i).isReady == UserData::READY_MUTCH_ALL)
				 //  ++count;
		   //}
		   ////　全員OKだったら飛ぶ
		   //if (active == count)
		   //{
			  // MainFrame->ChangeScene(new SceneMain());
		   //}

		   enum { READY = 2 };
		   if (SOCKET_MANAGER->GetUser(SOCKET_MANAGER->GetID()).isReady == READY)
		   {
		   		//	初期シーン登録
		   		//ThreadEND = true;
				//MainFrame->ChangeScene(new SceneMain());	

			   step = STEP::GAME;

		   }

	}
		break;
	case STEP::GAME:

		//if (KEY_Get(KEY_ENTER, 0) == 3)
		{
			MainFrame->ChangeScene(new SceneMain());
		}

		break;

	default:


		break;
	}

}


//******************************************************************
//		描画
//******************************************************************

void SceneSelect::Render()
{
	view->Activate();
	view->Clear();

	stage->Render();

	Select->Render(0, 0, 1280, 720, 0, 0, 1280, 720);

	for (int i = 0; i < PLAYER_MAX; ++i)
	{
		Text::Draw(800, 40 + (i * 32), 0xffffffff, "Clientaddr[%d]->%ld", i, SOCKET_MANAGER->GetUser(i).com);
		Text::Draw(1000, 40 + (i * 32), 0xffffffff, "name[%d]->%s", i, SOCKET_MANAGER->GetUser(i).name);
		Text::Draw(900, 470 + (i * 32), 0xff00ffff, "isReady[%d]->%d", i, SOCKET_MANAGER->GetUser(i).isReady);
	}

	//ステップ
	switch (step)
	{
	case STEP::GAME:

		Text::Draw(10, 620 , 0xff00ffff, "ENTERで進む");

		break;
	}


	//ナンバーエフェクト
	Number_Effect::Render();

	//フェード処理
	FadeControl::Render();

}