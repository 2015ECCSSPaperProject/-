
#include	"iextreme.h"

//netは一番初めにインクルードしないとエラーが起こる
#include	"../Net/Socket.h"
//#include	"../Net/net_config_loader.h"

#include	"../system/Framework.h"
#include	"../system/Thread.h"

#include	"SceneResult.h"
#include	"SceneSelect.h"

//#include	"../score/Score.h"
//#include	"../data/LimitedData.h"

/**********************/
/*	グローバル変数	　*/
/**********************/

//　スレッドを止める為に仮で作った
//static int ThreadEND = false;


using namespace std;

//******************************************************************
//		初期化・解放
//******************************************************************
bool SceneResult::Initialize()
{
	//	環境設定
	iexLight::SetAmbient(0x808080);
	iexLight::SetFog(800, 3000, 0);

	// Fade処理
	FadeControl::Setting(FadeControl::FADE_IN, 26);

	view = new iexView();
	view->Set(Vector3(0, 30, -60), Vector3(0, 0, 0));

	Select = new iex2DObj("DATA/Image/lobby/select.png");

	//if (!SOCKET_MANAGER->Init())
	//{
	//	MessageBox(0, "サーバーとの接続ができません", null, MB_OK);
	//	exit(0);
	//	return false;
	//}

	//　名前送る
	//SOCKET_MANAGER->SendName();

	// ポスターに必要な情報を初期化
	//PosterInit();

	//　ここのステップが STEP::START_OK が出たら
	step = STEP::START_NO;

	return true;
}

SceneResult::~SceneResult()
{
	//ThreadEND = true;

	delete view;
	delete Select;
}

//******************************************************************
//		処理
//******************************************************************
void SceneResult::Update()
{
	//ナンバーエフェクト
	Number_Effect::Update();

	//フェード処理
	FadeControl::Update();

	//switch (step)
	//{
	//case STEP::START_NO:
	//	//　Aボタン押したら
	//	if (KEY_Get(KEY_ENTER) == 3)
	//	{
	//		step = STEP::START_OK;
	//	}

	//	break;
	//case STEP::START_OK:
	//{
	//					   //　まだ準備できてないので戻ります
	//					   if (KEY_Get(KEY_ENTER) == 3)
	//					   {
	//						   step = STEP::START_NO;
	//					   }

	//					   //　追記：同期して
	//					   // 皆isResdy==2だったらゲーム画面へ！！
	//					   //  enum { READY = 2 };
	//					   //int count(0);
	//					   //int active(0);
	//					   //for (int i = 0; i < PLAYER_MAX; ++i)
	//					   //{
	//					   // if (SOCKET_MANAGER->GetUser(i).com == UserData::ACTIVE_USER)
	//					   //  ++active;

	//					   // // isReadyが全員OKになってたら　カウントするよ
	//					   // if (SOCKET_MANAGER->GetUser(i).isReady == UserData::READY_MUTCH_ALL)
	//					   //  ++count;
	//					   //}
	//					   ////　全員OKだったら飛ぶ
	//					   //if (active == count)
	//					   //{
	//					   // MainFrame->ChangeScene(new SceneMain());
	//					   //}

	//					   enum { READY = 2 };
	//					   if (SOCKET_MANAGER->GetUser(SOCKET_MANAGER->GetID()).isReady == READY)
	//					   {
	//						   //	初期シーン登録
	//						   //ThreadEND = true;
	//						   //MainFrame->ChangeScene(new SceneMain());	

	//						   step = STEP::GAME;

	//					   }

	//}
	//	break;
	//case STEP::GAME:

	//	//if (KEY_Get(KEY_ENTER, 0) == 3)
	//{
	//				   MainFrame->ChangeScene(new SceneSelect());
	//}

	//	break;

	//default:


	//	break;
	//}

	if (KEY_Get(KEY_ENTER) == 3)
	{
		   MainFrame->ChangeScene(new SceneSelect());
	}

}


//******************************************************************
//		描画
//******************************************************************

void SceneResult::Render()
{
	view->Activate();

	view->Clear();

	Select->Render(0, 0, 1280, 720, 0, 0, 1280, 720);

	for (int i = 0; i < PLAYER_MAX; ++i)
	{
		Text::Draw(200, 470 + (i * 32), 0xffff0000, "isReady[%d]->%d", i, SOCKET_MANAGER->GetUser(i).isReady);
	}

	// スコア描画
	//Text::Draw(320, 320, 0xffffffff, "スコア : %d",limited_data->Get_score(SOCKET_MANAGER->GetID()));


	//ステップ
	switch (step)
	{
	case STEP::GAME:

		Text::Draw(10, 620, 0xff00ffff, "ENTERで進む");

		break;
	}

	//ナンバーエフェクト
	Number_Effect::Render();

	//フェード処理
	FadeControl::Render();

}
