
#include	"iextreme.h"

//netは一番初めにインクルードしないとエラーが起こる
#include	"../Net/Socket.h"
//#include	"../Net/net_config_loader.h"

#include	"../system/Framework.h"
#include	"../system/Thread.h"

#include	"SceneResult.h"
#include	"SceneSelect.h"
#include	"../score/Score.h"

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
void SceneResult::Set_ranking()
{
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		datas[i].p_num = i;
		datas[i].score = score_mng->Get(i);
	}
	for (int i = 0; i < PLAYER_MAX - 1; i++) for (int j = i + 1; j < PLAYER_MAX; j++)
	{
		if (datas[i].score > datas[j].score)
		{
			int temp = datas[i].score;
			datas[i].score = datas[j].score;
			datas[j].score = temp;
			temp = datas[i].p_num;
			datas[i].p_num = datas[j].p_num;
			datas[j].p_num = temp;
		}
	}
}

bool SceneResult::Initialize()
{
	//	環境設定
	iexLight::SetAmbient(0x808080);
	iexLight::SetFog(800, 3000, 0);

	// Fade処理
	FadeControl::Setting(FadeControl::FADE_IN, 26);

	view = new iexView();
	view->Set(Vector3(0, 30, -60), Vector3(0, 0, 0));

	image[IMAGE::R1] = new iex2DObj("DATA/Image/result/rank1");
	image[IMAGE::R2] = new iex2DObj("DATA/Image/result/rank2");
	image[IMAGE::R3] = new iex2DObj("DATA/Image/result/rank3");
	image[IMAGE::R4] = new iex2DObj("DATA/Image/result/rank4");
	image[IMAGE::R5] = new iex2DObj("DATA/Image/result/rank5");
	image[IMAGE::R6] = new iex2DObj("DATA/Image/result/rank6");
	image[IMAGE::P1] = new iex2DObj("DATA/Image/lobby/red.png");
	image[IMAGE::P2] = new iex2DObj("DATA/Image/lobby/blue.png");
	image[IMAGE::P3] = new iex2DObj("DATA/Image/lobby/yellow.png");
	image[IMAGE::P4] = new iex2DObj("DATA/Image/lobby/green.png");
	image[IMAGE::P5] = new iex2DObj("DATA/Image/lobby/purple.png");
	image[IMAGE::P6] = new iex2DObj("DATA/Image/lobby/pink.png");
	image[IMAGE::BACK] = new iex2DObj("DATA/Image/lobby/select.png");
	image[IMAGE::ACTION] = new iex2DObj("DATA/UI/action/1.png");
	image[IMAGE::KEKKA] = new iex2DObj("DATA/Image/result/result.png");
	image[IMAGE::NUMBER] = new iex2DObj("DATA/UI/Num.png");

	Set_ranking();

	return true;
}

SceneResult::~SceneResult()
{
	//ThreadEND = true;

	delete view;
	for (int i = 0; i < IMAGE::MAX; i++)delete image[i];
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

	//image[IMAGE::BACK]->Render(0, 0, 1280, 720, 0, 0, 1280, 720);
	iexPolygon::Rect(0, 0, 1280, 720, RS_COPY, 0xffffffff);

	// 結果発表
	image[IMAGE::KEKKA]->Render(12, 28, 256, 64, 0, 0, 256, 64);

	// アクションUI
	image[IMAGE::ACTION]->Render(1060, 500, 256, 256, 0, 0, 256, 256);

	for (int i = 0; i < PLAYER_MAX; ++i)
	{
		image[IMAGE::P1 + datas[i].p_num]->Render(104, 136 + i * 96, 64, 64, 0, 0, 64, 64);
		image[IMAGE::NUMBER]->Render(568, 136 + i * 96, 64, 64, 64*11, 0, 64, 64);
	}

	// ランク表示
	//image[IMAGE::P1 + datas[SOCKET_MANAGER->GetID()].p_num]->Render(104, 136 + i * 96, 64, 64, 0, 0, 64, 64);

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
