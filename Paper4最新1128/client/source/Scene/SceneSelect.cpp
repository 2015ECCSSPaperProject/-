
#include	"iextreme.h"

//netは一番初めにインクルードしないとエラーが起こる
#include	"../Net/Socket.h"
//#include	"../Net/net_config_loader.h"

#include	"../system/Framework.h"
#include	"../system/Thread.h"
#include	"../Mouse/Mouse.h"
#include	"SceneSelect.h"
#include	"SceneMain.h"
#include	"../sound/SoundManager.h"
#include	"../../IEX/OKB.h"
#include	"../Animation/AnimationRipple.h"
#include	"../Animation/AnimationRippleEx.h"

#include	"SceneTitle.h"

/**********************/
/*	グローバル変数	　*/
/**********************/

// EXTERNで使って
// スコアアタックフラグ
bool SCOREATACK_FLAG;

//　スレッドを止める為に仮で作った
//static int ThreadEND = false;


using namespace std;
float uvMove = 0;

//******************************************************************
//		初期化・解放
//******************************************************************
bool SceneSelect::Initialize()
{
	SCOREATACK_FLAG = false;

	uvMove = 0;

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
	image[IMAGE::INFO] = new iex2DObj("DATA/Image/lobby/info.png");
	image[IMAGE::CURSOR] = new iex2DObj("DATA/Image/lobby/cursor4.png");
	image[IMAGE::JOIN] = new iex2DObj("DATA/Image/lobby/不参加.png");
	image[IMAGE::NOT_JOIN] = new iex2DObj("DATA/Image/lobby/参加.png");

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
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		OKRip[i] = new AnimationRipple("DATA/Image/lobby/junbi_ok.png", 25, 0.05f);
		OKFlag[i] = false;
	}
	
	// 酸化破門
	JoinRip = new AnimationRipple("DATA/Image/lobby/参加Rip.png", 30, 0.1f);
	InfoOKRip= new AnimationRipple("DATA/Image/lobby/OK.png", 30, 0.1f);
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
	chara.obj->SetTexture(texture, 0);
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

	mouse = new Mouse;
	mouse->Initialize(FALSE);

	Initialize_buttons();


	// INFO_ANIMATION!
	Info = new AnimationRippleEx("DATA/Image/lobby/InfoText.png",
		12, 10, 12, 0.5f, +(0.20f / 12.0f), true);
	onePlay = false;
	InfoFlag = false;

	// Shader
	Screen = new iex2DObj(iexSystem::ScreenWidth, iexSystem::ScreenHeight, IEX2D_RENDERTARGET);
	BlurScreenX = new iex2DObj(iexSystem::ScreenWidth, iexSystem::ScreenHeight, IEX2D_RENDERTARGET);
	BlurScreenY= new iex2DObj(iexSystem::ScreenWidth, iexSystem::ScreenHeight, IEX2D_RENDERTARGET);
	BlurValue = 0.0f;

	bgm->Fade_in("ドッグラン", .1f);

	return true;
}


//*************************************************************************************************
// マウスでクリックするアイコン
void SceneSelect::Initialize_buttons()
{
	modoru.lpButton = new iex2DObj("DATA/Image/lobby/もどる.png");
	modoru.dstX = 1120;
	modoru.dstY = 560;
	modoru.dstW = 128;
	modoru.dstH = 128;
	modoru.srcX = 256;
	modoru.srcY = 256;
	modoru.in = false;

	entry.lpButton = image[IMAGE::WAIT];
	entry.dstX = 396;
	entry.dstY = 136 + SOCKET_MANAGER->GetID() * 96;
	entry.dstW = 128;
	entry.dstH = 64;
	entry.srcX = 128;
	entry.srcY = 64;
	entry.in = false;

	entry2.lpButton = image[IMAGE::JOIN];
	entry2.dstX = 920;
	entry2.dstY = 560;
	entry2.dstW = 128;
	entry2.dstH = 128;
	entry2.srcX = 256;
	entry2.srcY = 256;
	entry2.in = false;


	OKInfo_B.lpButton = new iex2DObj("DATA/Image/lobby/OK.png");
	OKInfo_B.dstX = 680;
	OKInfo_B.dstY = 340;
	OKInfo_B.dstW = 128;
	OKInfo_B.dstH = 128;
	OKInfo_B.srcX = 128;
	OKInfo_B.srcY = 128;
	OKInfo_B.in = false;

	NOInfo_B.lpButton = new iex2DObj("DATA/Image/lobby/NO.png");
	NOInfo_B.dstX = 840;
	NOInfo_B.dstY = 340;
	NOInfo_B.dstW = 128;
	NOInfo_B.dstH = 128;
	NOInfo_B.srcX = 128;
	NOInfo_B.srcY = 128;
	NOInfo_B.in = false;

}

void SceneSelect::Update_buttons(const Vector2 &pos)
{
	//INFOじゃないとき
	if (!InfoFlag)
	{

		if (pos.x > modoru.dstX && pos.x  < modoru.dstX + modoru.dstW &&
			pos.y > modoru.dstY && pos.y < modoru.dstY + modoru.dstH)
		{
			if (!modoru.in){
				se->Play("カーソル");
				modoru.in = true;
			}
		}
		else modoru.in = false;

		if (pos.x > entry.dstX && pos.x  < entry.dstX + entry.dstW &&
			pos.y > entry.dstY && pos.y < entry.dstY + entry.dstH)
		{
			if (!entry.in){
				se->Play("カーソル");
				entry.in = true;
			}
		}
		else entry.in = false;

		if (pos.x > entry2.dstX && pos.x  < entry2.dstX + entry2.dstW &&
			pos.y > entry2.dstY && pos.y < entry2.dstY + entry2.dstH)
		{
			if (!entry2.in){
				se->Play("カーソル");
				entry2.in = true;
			}
		}
		else entry2.in = false;

	}


	//INFO
	if (InfoFlag)
	{

		if (pos.x > OKInfo_B.dstX && pos.x  < OKInfo_B.dstX + OKInfo_B.dstW &&
			pos.y > OKInfo_B.dstY && pos.y < OKInfo_B.dstY + OKInfo_B.dstH)
		{
			if (!OKInfo_B.in){
				se->Play("カーソル");
				OKInfo_B.in = true;
			}
		}
		else OKInfo_B.in = false;

		if (pos.x > NOInfo_B.dstX && pos.x  < NOInfo_B.dstX + NOInfo_B.dstW &&
			pos.y > NOInfo_B.dstY && pos.y < NOInfo_B.dstY + NOInfo_B.dstH)
		{
			if (!NOInfo_B.in){
				se->Play("カーソル");
				NOInfo_B.in = true;
			}
		}
		else NOInfo_B.in = false;

	}
}
//*************************************************************************************************

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
	delete modoru.lpButton;

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
	SAFE_DELETE(JoinRip);
	SAFE_DELETE(InfoOKRip);

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		SAFE_DELETE(OKRip[i]);
	}
	delete mouse;

	// AnimationRippleEX
	SAFE_DELETE(Info);
	SAFE_DELETE(OKInfo_B.lpButton);
	SAFE_DELETE(NOInfo_B.lpButton);

	SAFE_DELETE(Screen);
	SAFE_DELETE(BlurScreenX);
	SAFE_DELETE(BlurScreenY);
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
	mouse->Update();
	//==============================================================

	// 波紋　追加
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		IconRip[i]->Update();
	}

	// 参加
	JoinRip->Update();
	InfoOKRip->Update();



	if (KEY(KEY_SPACE) == 3)
	{
		

		Info->Action();
		for (int i = 0; i < PLAYER_MAX; i++)
		{
			IconRip[i]->Action();
		}
	}

	// OK波紋　追加
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		OKRip[i]->Update();

		// アクション!!
		if (OKFlag[i] == false)
		{
			if (SOCKET_MANAGER->GetUser(i).isReady)
			{
				OKFlag[i] = true;
				OKRip[i]->Action();
			}
		}

		// フラグ戻します
		if (OKFlag[i] == true)
		{
			if (!SOCKET_MANAGER->GetUser(i).isReady)
			{
				OKFlag[i] = false;
			}
		}
	}

	//ナンバーエフェクト
	Number_Effect::Update();

	//フェード処理
	FadeControl::Update();

	// ボタン更新
	Update_buttons(mouse->pos);

	// 戻るボタン範囲内
	if (modoru.in)
	{
		if (KeyBoardTRG(MOUSE_LEFT))
		{
			// シーンもドル
			MainFrame->ChangeScene(new SceneTitle());
			return;
		}
	}

	switch (step)
	{
	case STEP::START_NO:


		//　Aボタン押したら
		if (KeyBoardTRG(MOUSE_LEFT)&&entry.in)
		{
			//　自分の準備OKを光らす
			//OKRip[SOCKET_MANAGER->GetID()]->Action();
			step = STEP::START_OK;
			se->Play("決定");
			JoinRip->Action();
		}

		//　Aボタン押したら(Botton)
		if (KeyBoardTRG(MOUSE_LEFT) && entry2.in)
		{
			//　自分の準備OKを光らす
			//OKRip[SOCKET_MANAGER->GetID()]->Action();
			step = STEP::START_OK;
			se->Play("決定");
			JoinRip->Action();
		}

		break;
	case STEP::START_OK:
	{
						   // インフォが出てたら無効！！
						   if (InfoFlag == false)
						   {

							   //　まだ準備できてないので戻ります
							   if (KeyBoardTRG(MOUSE_LEFT) && entry.in)
							   {
								   step = STEP::START_NO;
								   se->Play("キャンセル");
							   }

							   //　まだ準備できてないので戻ります(bottonver)
							   if (KeyBoardTRG(MOUSE_LEFT) && entry2.in)
							   {
								   step = STEP::START_NO;
								   se->Play("キャンセル");
							   }

						   }
						   else
						   {
							   //　OK!スコアアタックを始める
							   if (KeyBoardTRG(MOUSE_LEFT) && OKInfo_B.in)
							   {
								   // いくぜえええええ
								   InfoOKRip->Action();
								   step = STEP::GAME;
								   se->Play("決定");
								   SCOREATACK_FLAG = true;	//スコアアタックフラグをON！
							   }

							   //　NO!もどる
							   if (KeyBoardTRG(MOUSE_LEFT) && NOInfo_B.in)
							   {
								   step = STEP::START_NO;
								   InfoFlag = false;			// INFOフラグを戻して
								   BlurValue = 0.0f;			// ぼかしも戻すぴょん
								   se->Play("キャンセル");
								   
								   return;			//出ていけ！！！
							   }


						   }

		//　追記：同期して
		   // 皆isResdy==2だったらゲーム画面へ！！
		   enum { READY = 2 };

		   int count(0);
		   int active(0);
		   for (int i = 0; i < PLAYER_MAX; ++i)
		   {
			   if (SOCKET_MANAGER->GetUser(i).com == UserData::ACTIVE_USER)
				   ++active;
		   }

		   // ぼっちなら選択画面へ
		   if (active == 1)
		   {
			   // Info
			   if (InfoFlag == false)
			   {
				   InfoFlag = true;
				   Info->Action();
			   }

			   // ぼかす
			   if (BlurValue <= 2.0f)
			   {
				   BlurValue += 0.1f;
			   }
			  

		   }
		   else
		   {
			   // 誰か入ってきたら戻る？？？
			   InfoFlag = false;			// INFOフラグを戻して
			   BlurValue = 0.0f;			// ぼかしも戻すぴょん


			   if (SOCKET_MANAGER->GetUser(SOCKET_MANAGER->GetID()).isReady == READY)
			   {
				   //	初期シーン登録
				   //ThreadEND = true;
				   //MainFrame->ChangeScene(new SceneMain());	

				   step = STEP::GAME;

			   }

		   }



	}
		break;
	case STEP::GAME:

		FadeControl::Setting(FadeControl::FADE_OUT, 26);
		bgm->Fade_out("ドッグラン", .05f);
		step = STEP::GAME_FADE;
		//if (KEY_Get(KEY_ENTER, 0) == 3)
		//{
		//	MainFrame->ChangeScene(new SceneMain());
		//}

		break;
	case STEP::GAME_FADE: //　フェード作ります

		if (FadeControl::isFadeOut == true)
		{
			bgm->Stop("ドッグラン");
			MainFrame->ChangeScene(new SceneMain());
			return;
		}

		break;
	default:


		break;
	}

	// INFO_ANIMATION!Update
	Info->Update();
	shader2D->SetValue("BlurValue", BlurValue);

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


	view->Clear();
	
	//ブラ―用に
	Screen->RenderTarget();

	view->Clear();

	// 背景
	if (uvMove <= 770.0f)	uvMove += 0.2f;

	image[IMAGE::BACK2]->RenderBack(0, 0, 1280, 720, (int)uvMove, 0, 1280, 720);

	// キャラクター
	chara.obj->Update();
	chara.obj->Render();

	// 参加者リスト
	image[IMAGE::BACK]->Render(0, 0, 1280, 720, 0, 0, 1280, 720, RS_COPY);
	image[IMAGE::LIST]->Render(12, 28, 256, 64, 0, 0, 256, 64);

	// アクションUI
	//image[IMAGE::ACTION]->Render(1060, 500, 256, 256, 0, 0, 256, 256);
	image[IMAGE::INFO]->Render(600, 0);

	// 戻るボタン
	modoru.lpButton->Render(modoru.dstX, modoru.dstY, modoru.dstW, modoru.dstH, 0, 0, modoru.srcX, modoru.srcY);
	if (modoru.in){//　戻るを押したら
	//	modoru.lpButton->Render(modoru.dstX, modoru.dstY, modoru.dstW, modoru.dstH, 0, 0, modoru.srcX, modoru.srcY, RS_ADD);
		modoru.lpButton->SetARGB(127, 127, 127, 127);
		modoru.lpButton->Render(modoru.dstX, modoru.dstY, modoru.dstW, modoru.dstH, 0, 0, modoru.srcX, modoru.srcY, RS_ADD);
		modoru.lpButton->SetARGB(255, 255, 255, 255);
		modoru.lpButton->SetScale(1.2f);// 大きく
	}
	else
	{
		modoru.lpButton->SetScale(1.0f);// もどす
	}


	// 準備中？準備OK
	if (SOCKET_MANAGER->GetUser(SOCKET_MANAGER->GetID()).isReady)
	{
		//ボタンバージョン
		image[IMAGE::JOIN]->Render(entry2.dstX, entry2.dstY, entry2.dstW, entry2.dstH, 0, 0,
			entry2.srcX, entry2.srcY, RS_COPY);
		// 振れていたら
		if (entry2.in){
			//image[IMAGE::OK]->Render(396 + moveX[i], 136 + i * 96, 128, 64, 0, 0, 128, 64, RS_ADD);
			image[IMAGE::JOIN]->SetARGB(127, 127, 127, 127);
			image[IMAGE::JOIN]->Render(entry2.dstX, entry2.dstY, entry2.dstW, entry2.dstH, 0, 0,
				entry2.srcX, entry2.srcY, RS_ADD);
			image[IMAGE::JOIN]->SetARGB(255, 255, 255, 255);
			image[IMAGE::JOIN]->SetScale(1.2f);	// 大きく
		}
		else
		{
			image[IMAGE::JOIN]->SetScale(1.0f);
		}
	}
	else//準備中
	{


		//ボタンバージョン
		image[IMAGE::NOT_JOIN]->Render(entry2.dstX, entry2.dstY, entry2.dstW, entry2.dstH, 0, 0,
			entry2.srcX, entry2.srcY, RS_COPY);
		// 振れていたら
		if (entry2.in){
			//image[IMAGE::OK]->Render(396 + moveX[i], 136 + i * 96, 128, 64, 0, 0, 128, 64, RS_ADD);
			image[IMAGE::NOT_JOIN]->SetARGB(127, 127, 127, 127);
			image[IMAGE::NOT_JOIN]->Render(entry2.dstX, entry2.dstY, entry2.dstW, entry2.dstH, 0, 0,
				entry2.srcX, entry2.srcY, RS_ADD);
			image[IMAGE::NOT_JOIN]->SetARGB(255, 255, 255, 255);
			image[IMAGE::NOT_JOIN]->SetScale(1.2f);	// 大きく
		}
		else
		{
			image[IMAGE::NOT_JOIN]->SetScale(1.0f);
		}

	}


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
				if (SOCKET_MANAGER->GetID() != i) se->Play("エントリー");
				IconRip[i]->Action();		// 波紋■
				isActivePlayer[i] = true;
				if (SOCKET_MANAGER->GetID() != i) se->Play("エントリー");
			}
		}
		// 文字アニメスイッチ
		if (isActivePlayer[i] == true)
		{
			// 出て行ったら・・
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
			moveX[i] -= 24;
			if (moveX[i] <= 0)	moveX[i] = 0;
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
			IconRip[i]->Render(64, 136 + i * 96, RS_COPY);

			// 自分の名前or相手	
			if (SOCKET_MANAGER->GetID() == i)
			{
				DWORD col = ARGB((BYTE)alpha[i], 0, 0, 0);
				Text::Draw(130 + moveX[i], 154 + i * 96, col, "%s", SOCKET_MANAGER->GetUser(i).name);
			}
			else
			{
				DWORD col = ARGB((BYTE)alpha[i], 127, 127, 127);
				Text::Draw(130 + moveX[i], 154 + i * 96, col, "%s", SOCKET_MANAGER->GetUser(i).name);
			}

			// 準備中？準備OK
			if (SOCKET_MANAGER->GetUser(i).isReady)
			{
				image[IMAGE::OK]->SetARGB(alpha[i], 255, 255, 255);

				// 振れていたら
				if (entry.in&&SOCKET_MANAGER->GetID() == i){
					
					// デフォルト
					image[IMAGE::OK]->SetScale(1.1f);	// 大きく
					image[IMAGE::OK]->Render(396 + moveX[i], 136 + i * 96, 128, 64, 0, 0, 128, 64);

					//image[IMAGE::OK]->Render(396 + moveX[i], 136 + i * 96, 128, 64, 0, 0, 128, 64, RS_ADD);
					image[IMAGE::OK]->SetARGB(127, 127, 127, 127);
					image[IMAGE::OK]->Render(396 + moveX[i], 136 + i * 96, 128, 64, 0, 0, 128, 64, RS_ADD);
					image[IMAGE::OK]->SetARGB(255, 255, 255, 255);

				}
				else
				{
					// デフォルト
					image[IMAGE::OK]->SetScale(1.0f);
					image[IMAGE::OK]->Render(396 + moveX[i], 136 + i * 96, 128, 64, 0, 0, 128, 64);
					
				}

		
			}
			else
			{
				image[IMAGE::WAIT]->SetARGB(alpha[i], 255, 255, 255);
				// 振れていたら
				if (entry.in&&SOCKET_MANAGER->GetID() == i){

					// Defo
					image[IMAGE::WAIT]->SetScale(1.1f);// 大きく
					image[IMAGE::WAIT]->Render(396 + moveX[i], 136 + i * 96, 128, 64, 0, 0, 128, 64);

					image[IMAGE::WAIT]->SetARGB(127, 127, 127, 127);
					image[IMAGE::WAIT]->Render(396 + moveX[i], 136 + i * 96, 128, 64, 0, 0, 128, 64, RS_ADD);
					image[IMAGE::WAIT]->SetARGB(255, 255, 255, 255);
	
				}
				else
				{
					image[IMAGE::WAIT]->SetScale(1.0f);
					image[IMAGE::WAIT]->Render(396 + moveX[i], 136 + i * 96, 128, 64, 0, 0, 128, 64);

				}



			}

			// 準備OK波紋　追加
			OKRip[i]->Render(396 + moveX[i], 136 + i * 96);

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
	
	//参加波紋
	JoinRip->Render(entry2.dstX, entry2.dstY, RS_ADD);





	//for (int i = 0; i < PLAYER_MAX; ++i)
	//{
	//	Text::Draw(800, 40 + (i * 32), 0xffffffff, "Clientaddr[%d]->%ld", i, SOCKET_MANAGER->GetUser(i).com);
	//	Text::Draw(1000, 40 + (i * 32), 0xffffffff, "name[%d]->%s", i, SOCKET_MANAGER->GetUser(i).name);
	//	Text::Draw(900, 470 + (i * 32), 0xff00ffff, "isReady[%d]->%d", i, SOCKET_MANAGER->GetUser(i).isReady);
	//}

	// ぼかすゲーム画面X
	BlurScreenX->RenderTarget();
	Screen->Render(0, 0, shader2D, "XBlur");
	shader2D->SetValue("TextureBX", BlurScreenX);//Xブラ―の情報を送

	//バックバッファにチェンジ
	iexSystem::Device->SetRenderTarget(0, backbuffer);

	// ぼかすゲーム画面
	//Screen->Render(0, 0, shader2D, "XBlur");
	//shader2D->SetValue("TextureBX", BlurScreen);//Xブラ―の情報を送
	BlurScreenY->Render(0, 0, shader2D, "YBlur_M");



	//INFO
	if (InfoFlag)
	{
		// INFO_ANIMATIO_RENDER
		Info->Render(-380, -170, RS_COPY);



		//ボタンバージョン
		OKInfo_B.lpButton->Render(OKInfo_B.dstX, OKInfo_B.dstY, OKInfo_B.dstW, OKInfo_B.dstH, 0, 0,
			OKInfo_B.srcX, OKInfo_B.srcY, RS_COPY);
		// 振れていたら
		if (OKInfo_B.in){
			//image[IMAGE::OK]->Render(396 + moveX[i], 136 + i * 96, 128, 64, 0, 0, 128, 64, RS_ADD);
			OKInfo_B.lpButton->SetARGB(127, 127, 127, 127);
			OKInfo_B.lpButton->Render(OKInfo_B.dstX, OKInfo_B.dstY, OKInfo_B.dstW, OKInfo_B.dstH, 0, 0,
				OKInfo_B.srcX, OKInfo_B.srcY, RS_ADD);
			OKInfo_B.lpButton->SetARGB(255, 255, 255, 255);
			OKInfo_B.lpButton->SetScale(1.2f);	// 大きく
		}
		else
		{
			OKInfo_B.lpButton->SetScale(1.0f);
		}


		//ボタンバージョン
		NOInfo_B.lpButton->Render(NOInfo_B.dstX, NOInfo_B.dstY, NOInfo_B.dstW, NOInfo_B.dstH, 0, 0,
			NOInfo_B.srcX, NOInfo_B.srcY, RS_COPY);
		// 振れていたら
		if (NOInfo_B.in){
			//image[IMAGE::NO]->Render(396 + moveX[i], 136 + i * 96, 128, 64, 0, 0, 128, 64, RS_ADD);
			NOInfo_B.lpButton->SetARGB(127, 127, 127, 127);
			NOInfo_B.lpButton->Render(NOInfo_B.dstX, NOInfo_B.dstY, NOInfo_B.dstW, NOInfo_B.dstH, 0, 0,
				NOInfo_B.srcX, NOInfo_B.srcY, RS_ADD);
			NOInfo_B.lpButton->SetARGB(255, 255, 255, 255);
			NOInfo_B.lpButton->SetScale(1.2f);	// 大きく
		}
		else
		{
			NOInfo_B.lpButton->SetScale(1.0f);
		}

	}
	// OKRIP
	InfoOKRip->Render(OKInfo_B.dstX, OKInfo_B.dstY, RS_ADD);

	// カーソル
	bool iconFlag = false;
	if (KeyBoard(MOUSE_LEFT))iconFlag = true;	// マウス離す
	image[IMAGE::CURSOR]->Render(mouse->pos.x - 32, mouse->pos.y - 24, 64, 64, iconFlag * 64, 64, 64, 64);


	//ステップ
	switch (step)
	{
	case STEP::GAME:

		Text::Draw(10, 620, 0xff00ffff, "ENTERで進む");

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
