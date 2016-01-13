
#include	"iextreme.h"

//netは一番初めにインクルードしないとエラーが起こる
#include	"../Net/Socket.h"
//#include	"../Net/net_config_loader.h"

#include	"../system/Framework.h"
#include	"../system/Thread.h"

#include	"SceneSelect.h"
#include	"SceneMain.h"

#include	"../../IEX/OKB.h"
#include	"../Animation/AnimationRipple.h"


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
	if (!SOCKET_MANAGER->Init())
	{
		MessageBox(0, "サーバーとの接続ができません", null, MB_OK);
		exit(0);
		return false;
	}

	//	環境設定
	iexLight::SetAmbient(0x808080);
	iexLight::SetFog(800, 3000, 0);

	// Fade処理
	FadeControl::Setting(FadeControl::FADE_IN, 26);

	view = new iexView();
	view->Set(Vector3(0, 0, -90), Vector3(0, 0, 0));

	// 2D画像初期化
	image[IMAGE::BACK] = new iex2DObj("DATA/Image/lobby/back2.png");
	image[IMAGE::P1] = new iex2DObj("DATA/Image/lobby/red.png");
	image[IMAGE::P2] = new iex2DObj("DATA/Image/lobby/blue.png");
	image[IMAGE::P3] = new iex2DObj("DATA/Image/lobby/yellow.png");
	image[IMAGE::P4] = new iex2DObj("DATA/Image/lobby/green.png");
	image[IMAGE::P5] = new iex2DObj("DATA/Image/lobby/purple.png");
	image[IMAGE::P6] = new iex2DObj("DATA/Image/lobby/pink.png");
	image[IMAGE::WAIT] = new iex2DObj("DATA/Image/lobby/junbi_chu.png");
	image[IMAGE::OK] = new iex2DObj("DATA/Image/lobby/junbi_ok.png");
	image[IMAGE::LIST] = new iex2DObj("DATA/Image/lobby/list.png");
	image[IMAGE::ACTION] = new iex2DObj("DATA/UI/action/1.png");
	// 西田追加
	image[IMAGE::NOPLAYER] = new iex2DObj("DATA/Image/lobby/noPlayer.png");
	image[IMAGE::WANTED] = new iex2DObj("DATA/Image/lobby/Wanted.png");
	image[IMAGE::TEN] = new iex2DObj("DATA/Image/lobby/ten.png");
	image[IMAGE::BACK2] = new iex2DObj("DATA/Image/lobby/背景.png");

	// 文字のアニメーション
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		isActivePlayer[i] = false;
		moveX[i] = +300;
		alpha[i] = 64;
	}
	// Rip用
	IconRip[0] = new AnimationRipple("DATA/Image/lobby/red.png", 15, 0.1f);
	IconRip[1] = new AnimationRipple("DATA/Image/lobby/blue.png", 15, 0.1f);
	IconRip[2] = new AnimationRipple("DATA/Image/lobby/yellow.png", 15, 0.1f);
	IconRip[3] = new AnimationRipple("DATA/Image/lobby/green.png", 15, 0.1f);
	IconRip[4] = new AnimationRipple("DATA/Image/lobby/purple.png", 15, 0.1f);
	IconRip[5] = new AnimationRipple("DATA/Image/lobby/pink.png", 15, 0.1f);


	// キャラクター
	chara.pos = Vector3(20, -19, 0);
	chara.angle = PI;
	Texture2D *texture;
	switch (SOCKET_MANAGER->GetID())
	{
	case 1:
		texture = iexTexture::Load("DATA/CHR/player/player_blue.png");
		break;
	case 2:
		texture = iexTexture::Load("DATA/CHR/player/player_yellow.png");
		break;
	case 3:
		texture = iexTexture::Load("DATA/CHR/player/player_green.png");
		break;
	case 4:
		texture = iexTexture::Load("DATA/CHR/player/player_purple.png");
		break;
	case 5:
		texture = iexTexture::Load("DATA/CHR/player/player_pink.png");
		break;
	default:
		texture = iexTexture::Load("DATA/CHR/player/player_red.png");
		break;
	}

	chara.obj = new iex3DObj("DATA/CHR/player/run.IEM");
	//chara.obj->SetTexture(texture, 0);
	chara.obj->SetAngle(chara.angle);
	chara.obj->SetScale(1.0f);
	chara.obj->SetPos(chara.pos);
	chara.obj->SetMotion(1);

	//　名前送る
	SOCKET_MANAGER->SendName();

	// ポスターに必要な情報を初期化
	PosterInit();

	//　ここのステップが STEP::START_OK が出たら
	step = STEP::START_NO;

	m_pThread = 0;

	//	スレッド開始
	//ThreadEND = false;
	m_pThread = new Thread(ThreadFunc, this);
	m_pThread->Run();

	return true;
}

void SceneSelect::PosterInit()
{

	//　イラスト初期化＆スタンプ手帳初期化
	for (int i = 0; i < KIND_MAX; i++)
	{
		for (int no = 0; no < NO_MAX; no++)
		{
			StampPicture[i][no] = NULL;
		}
	}

	//イラストはここで入れていきます。
	StampPicture[KIND_NAME::OBJECT][0] = new iex2DObj("DATA/makePoster/stamp/yen.png");
	StampPicture[KIND_NAME::OBJECT][1] = new iex2DObj("DATA/makePoster/stamp/coffee.png");
	StampPicture[KIND_NAME::OBJECT][2] = new iex2DObj("DATA/makePoster/stamp/gentei.png");
	StampPicture[KIND_NAME::OBJECT][3] = new iex2DObj("DATA/makePoster/stamp/hamburger.png");
	StampPicture[KIND_NAME::FOOD][0] = new iex2DObj("DATA/makePoster/stamp/aniki.png");
	StampPicture[KIND_NAME::FOOD][1] = new iex2DObj("DATA/makePoster/stamp/歪みねぇな.png");
	StampPicture[KIND_NAME::FOOD][2] = new iex2DObj("DATA/makePoster/stamp/蟹.png");
	StampPicture[KIND_NAME::FOOD][3] = new iex2DObj("DATA/makePoster/stamp/だらしねぇな.png");
	StampPicture[KIND_NAME::BACK][0] = new iex2DObj("DATA/makePoster/stamp/back/not.png");
	StampPicture[KIND_NAME::BACK][1] = new iex2DObj("DATA/makePoster/stamp/back/tehutehu.png");
	StampPicture[KIND_NAME::BACK][2] = new iex2DObj("DATA/makePoster/stamp/back/block.png");
	StampPicture[KIND_NAME::BACK][3] = new iex2DObj("DATA/makePoster/stamp/back/midori.png");
	StampPicture[KIND_NAME::BACK][4] = new iex2DObj("DATA/makePoster/stamp/back/blue.png");
	StampPicture[KIND_NAME::BACK][5] = new iex2DObj("DATA/makePoster/stamp/back/ゲイパレス.png");

	//スクリーン

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		posterScreen[i] = new iex2DObj(512, 512, IEX2D_RENDERTARGET);

		//　色つきフレーム
		char fileName[128];
		sprintf(fileName, "DATA/makePoster/frame/Posterframe%d.png", i);
		posterFrame[i] = new iex2DObj(fileName);
	}

	iexSystem::Device->GetRenderTarget(0, &backbuffer);//★描画先


};

SceneSelect::~SceneSelect()
{
	//ThreadEND = true;

	delete view;
	for (int i = 0; i < IMAGE::MAX; i++)delete image[i];
	delete chara.obj;
	delete m_pThread;

	// スタンプ
	for (int i = 0; i < KIND_MAX; i++)
	{
		for (int no = 0; no < NO_MAX; no++)
		{
			SAFE_DELETE(StampPicture[i][no]);
		}
	}

	// スクリーン
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		SAFE_DELETE(posterScreen[i]);
		SAFE_DELETE(posterFrame[i]);
	}

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		SAFE_DELETE(IconRip[i]);
	}
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
			// レイヤー更新
			SOCKET_MANAGER->UpdateLayer();
			break;
		case SceneSelect::STEP::START_OK:
			//　セレクト画面の更新
			SOCKET_MANAGER->UpdateTeam(true);
			// レイヤー更新
			SOCKET_MANAGER->UpdateLayer();
			break;
		case SceneSelect::STEP::GAME:
			//　セレクト画面の更新
			SOCKET_MANAGER->UpdateTeam(2);
			// レイヤー更新
			SOCKET_MANAGER->UpdateLayer();
			break;
		}
		
	}
}



//******************************************************************
//		処理
//******************************************************************
void SceneSelect::Update()
{
	int AxisX = 0, AxisY = 0;
	if (KEY(KEY_LEFT) == 1) AxisX += -1;
	if (KEY(KEY_RIGHT) == 1) AxisX += 1;
	if (KEY(KEY_UP) == 1) AxisY += 1;
	if (KEY(KEY_DOWN) == 1) AxisY += -1;

	int motion_no = 1;
	if (AxisY > 0) motion_no = 0;
	else if (AxisX < 0) motion_no = 11;
	else if (AxisX > 0) motion_no = 10;
	else if (AxisY < 0) motion_no = 7;
	if (chara.obj->GetMotion() != motion_no)chara.obj->SetMotion(motion_no);
	chara.obj->Animation();


	//=============================================================
	//	マウス更新
	POINT p;
	GetCursorPos(&p);

	//　画面の真ん中へ移動
	RECT rc;
	GetWindowRect(iexSystem::Window, &rc);
	p.x -= rc.left + iexSystem::ScreenWidth / 2;
	p.x = (LONG)((float)p.x * .8f);
	if (GetActiveWindow() == IEX_GetWindow())
	{
		ShowCursor(FALSE);
		SetCursorPos(rc.left + iexSystem::ScreenWidth / 2, rc.top + iexSystem::ScreenHeight / 2);
	}
	float	work;
	work = (float)p.x *0.001f;
	if (work > 0.1f) work = 0.1f;
	chara.obj->SetAngle((chara.angle += work));// Angleに加算
	//==============================================================

	// 波紋　追加
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		IconRip[i]->Update();
	}
	if (KEY(KEY_SPACE) == 3)
	{
		for (int i = 0; i < PLAYER_MAX; i++)
		{
			IconRip[i]->Action();
		}
	}

	//ナンバーエフェクト
	Number_Effect::Update();

	//フェード処理
	FadeControl::Update();

	switch (step)
	{
	case STEP::START_NO:
		//　Aボタン押したら
		if (KEY_Get(KEY_ENTER) == 3)
		{
			step = STEP::START_OK;
		}

		break;
	case STEP::START_OK:
	{
		   //　まだ準備できてないので戻ります
		   if (KEY_Get(KEY_ENTER) == 3)
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

	////　マルチレンダーターゲットでポスターを描画
	//for (int i = 0; i < PLAYER_MAX; i++)
	//{
	//	PosterRender(i);
	//}
	////バックバッファ
	//iexSystem::Device->SetRenderTarget(0, backbuffer);

	view->Clear();

	// 背景
	static float uvMove = 0;
	uvMove += 0.2f;
	image[IMAGE::BACK2]->RenderBack(0, 0, 1280, 720, (int)uvMove, 0, 1280, 720);

	// キャラクター
	chara.obj->Update();
	chara.obj->Render();

	// 参加者リスト
	image[IMAGE::BACK]->Render(0, 0, 1280, 720, 0, 0, 1280, 720, RS_COPY);
	image[IMAGE::LIST]->Render(12, 28, 256, 64, 0, 0, 256, 64);

	// アクションUI
	image[IMAGE::ACTION]->Render(1060, 500, 256, 256, 0, 0, 256, 256);


	// 点のアニメ用変数
	static int tenFlame = 0;
	static int tenAnime = 0;
	tenFlame++;
	if (tenFlame >= 24)
	{
		tenFlame = 0;
		tenAnime++;
		if (tenAnime >= 3){ tenAnime = 0; }
	}



	for (int i = 0; i < PLAYER_MAX; i++)
	{
		// 文字アニメスイッチ
		if (isActivePlayer[i] == false)
		{	// アクティブに初めてなったら
			if (SOCKET_MANAGER->GetUser(i).com == UserData::ACTIVE_USER)
			{
				IconRip[i]->Action();		// 波紋■
				isActivePlayer[i] = true;
			}
		}
		// 文字アニメスイッチ
		if (isActivePlayer[i] == true)
		{	// 出て行ったら・・
			if (SOCKET_MANAGER->GetUser(i).com != UserData::ACTIVE_USER)
			{
				// 初期化
				isActivePlayer[i] = false;
				moveX[i] = 300;
				alpha[i] = 64;
			}
		}


		// 文字アニメ更新
		if (isActivePlayer[i] == true)
		{
			// 動く
			moveX[i]-=24;
			if (moveX[i]<=0)	moveX[i] = 0;
			// 透明
			alpha[i] += 8;
			if (alpha[i] >= 255)alpha[i] = 255;
			

		}
		else
		{
			// 初期値に戻す
			moveX[i] = 300;
			alpha[i] = 32;
			//moveX[i] -= 6;
			//if (moveX[i] <= 0)	moveX[i] = 0;
		}


		// アクティブなプレイヤー以外は描画しない
		if (SOCKET_MANAGER->GetUser(i).com == UserData::ACTIVE_USER)
		{

			// 右のユーザーの■　ユーザーたち
			image[IMAGE::P1 + i]->Render(64, 136 + i * 96, 64, 64, 0, 0, 64, 64);
			// 追加　波紋■
			IconRip[i]->Render(64, 136 + i * 96,RS_COPY);

			// 名前
			DWORD col = ARGB((BYTE)alpha[i], 0, 0, 0);
			Text::Draw(140 + moveX[i], 154 + i * 96, col, "%s", SOCKET_MANAGER->GetUser(i).name);

			// 準備中？準備OK
			image[(!SOCKET_MANAGER->GetUser(i).isReady) ? IMAGE::WAIT : IMAGE::OK]->SetARGB(alpha[i], 255, 255, 255);
			image[(!SOCKET_MANAGER->GetUser(i).isReady) ? IMAGE::WAIT : IMAGE::OK]->Render(396 + moveX[i], 136 + i * 96, 128, 64, 0, 0, 128, 64);

		}
		else // 参加していなかったら
		{
			// 右のユーザーの■
			image[IMAGE::NOPLAYER]->Render(104, 136 + i * 96, 64, 64, 0, 0, 64, 64);
			// 通信待機中
			image[IMAGE::WANTED]->Render(190, 138 + i * 96, 256, 128, 0, 0, 256, 128);
			// 点
			image[IMAGE::TEN]->SetARGB(100, 100, 100, 100);
			image[IMAGE::TEN]->Render(430, 138 + i * 96, 64, 64, tenAnime * 64, 0, 64, 64);
		}

	}

	//for (int i = 0; i < PLAYER_MAX; ++i)
	//{
	//	Text::Draw(800, 40 + (i * 32), 0xffffffff, "Clientaddr[%d]->%ld", i, SOCKET_MANAGER->GetUser(i).com);
	//	Text::Draw(1000, 40 + (i * 32), 0xffffffff, "name[%d]->%s", i, SOCKET_MANAGER->GetUser(i).name);
	//	Text::Draw(900, 470 + (i * 32), 0xff00ffff, "isReady[%d]->%d", i, SOCKET_MANAGER->GetUser(i).isReady);
	//}

	//ステップ
	switch (step)
	{
	case STEP::GAME:

		Text::Draw(10, 620 , 0xff00ffff, "ENTERで進む");

		break;
	}

	// 皆のポスター
	//for (int i = 0; i < PLAYER_MAX; i++)
	//{
	//	posterScreen[i]->Render((i * 160), 580, 512 / 4, 512 / 4, 0, 0, 512, 512);
	//	image[IMAGE::P1 + i]->Render(104, 136 + i * 96, 64, 64, 0, 0, 64, 64);
	//}

	////ナンバーエフェクト
	//Number_Effect::Render();

	//フェード処理
	FadeControl::Render();

}


//　ポスターの描画　引数はプレイヤーNo
void SceneSelect::PosterRender(int i)
{
	posterScreen[i]->RenderTarget();
	view->Clear(0xffffffff);
	//　ここからポスターのテクスチャーに移る絵を描く

	//　レイヤーの画像
	for (int num = (LAYER_MAX - 1); num >= 0; num--)//■描画順を０が手前に来るように
	{
		if (SOCKET_MANAGER->GetLayer(i).layerdata[num].kind != -1)//　レイヤーが空だったら描画しない
		{
			//　絵を表示　※ポスターの表示場所分ずらしている
			StampPicture[SOCKET_MANAGER->GetLayer(i).layerdata[num].kind][SOCKET_MANAGER->GetLayer(i).layerdata[num].num]
				->Render((int)SOCKET_MANAGER->GetLayer(i).layerdata[num].x - 256 - (64), (int)SOCKET_MANAGER->GetLayer(i).layerdata[num].y - 256 - (128), 512, 512, 0, 0, 512, 512);
		}
	}

	// ポスターのフレーム ※クライアントの番号によって色を後で分ける
	posterFrame[i]->Render(0, 0, 512, 512, 0, 0, 512, 512);

	//テクスチャーとしてシェーダーへ
	char MapName[64];
	sprintf(MapName, "PosterMap_%d", i);
	shaderD->SetValue(MapName, posterScreen[i]);

}