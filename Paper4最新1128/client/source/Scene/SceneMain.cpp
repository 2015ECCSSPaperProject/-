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
#include	"../sound/SoundManager.h"

#include	"../Scene/SceneResult.h"
//#include	"../data/LimitedData.h"
#include	"../ui/UI.h"
#include	"../blur/blur.h"

#include	"../Effect/Effect.h"


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
//iex3DObj *set;
//iex3DObj *set_die;
#include	"../Player/PlayerManager.h"


#include "../camera/Camera.h"
#include "../stage/Stage.h"

#include <vector>
#include "../../../share_data/Enum_public.h"
#include "../paper object/paper object manager.h"

#include "../score/Score.h"
#include "../timer/Timer.h"

using namespace std;

static int FLAME = 0;


//******************************************************************
//		初期化・解放
//******************************************************************
bool SceneMain::Initialize()
{
	BlurFilter::Init();

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
	sky->SetScale(4.0f);
	sky->SetPos(0, -100, 0);
	sky->Update();

	//set = new iex3DObj("DATA/CHR/player/run.IEM");
	//set_die = new iex3DObj("DATA/CHR/player/die.IEM");

	//■■■　相手と自分で分ける
	//for (int i = 0; i <PLAYER_MAX; ++i)
	//{
	//	if (i == SOCKET_MANAGER->GetID()){
	//		player[i] = new MyPlayer();
	//		camera->Initialize(player[i]);	// カメラ設定
	//	}
	//	else
	//		player[i] = new NetPlayer();

	//	player[i]->Initialize(set, set_die);
	//}

	player_mng = new PlayerManager;
	player_mng->Initialize();

	extern int result_my_number;
	result_my_number = SOCKET_MANAGER->GetID();

	paper_obj_mng = new Paper_obj_mng;
	paper_obj_mng->Initialize();

	// タイマー
	timer = new Timer;

	// スコア
	score_mng->Initialize();

	// UI
	ui = new UI;
	ui->Initialize(player_mng->Get_player(SOCKET_MANAGER->GetID()));

	event_bgm->Initialize("フライハイ");
	event_bgm->Set_mode(EventBGM::MODE::START);

	// 開始フラグを送る
	m_pThread = new Thread(ThreadFunc, this);

	//	同期をとる
	SOCKET_MANAGER->InitGame();//ここで

	//	ネットワークを開始する
	m_pThread->Run();

	// モード関数初期化
	mode = MODE::START;
	Mode_funk[0] = &SceneMain::Start;
	Mode_funk[1] = &SceneMain::Main;
	Mode_funk[2] = &SceneMain::End;

	FLAME = 0;

	// deferredの初期設定　初期化　
	DeferredManager;
	DeferredManager.CreateShadowMap(1024);
	DeferredManager.CreateShadowMapL(512);
	DeferredManager.SetEmissiveRate(1000);// エミッシブレート
	DeferredManager.SetExposure(-10);
	
	deferredFlag = true;// flag
	DebugTex = nullptr;
	LightVec = Vector3(1.0f, -1.0f, 1.0f);
	exposure = -4.6f;
	FarShadowFlag = 0;

	//particle = new iexParticlePlus();
	particle->Initialize("DATA/effect/particle.png",1024);

	return true;
}

SceneMain::~SceneMain()
{
	delete camera;
	//delete view;
	delete stage;
	delete sky;
	delete m_pThread;//　なんかスレッド消さないとエラー起こる

	SAFE_DELETE(player_mng);

	// 今だけコメントアウト！あとで直す
	SAFE_DELETE(paper_obj_mng);
	SAFE_DELETE(ui);
	SAFE_DELETE(timer);

	// deferredの解除
	DeferredManager.Release();

	BlurFilter::CleanUp();

	particle->Release();
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
		FLAME++;
		//if (FLAME < 60*5)
		{
			SOCKET_MANAGER->UpdateUser();
			SOCKET_MANAGER->UpdateStage();
			SOCKET_MANAGER->UpdateScore();
		}

	}
}

//******************************************************************
//		処理
//******************************************************************
void SceneMain::Update()
{
	// ブラ―
	BlurFilter::Update();

	//フェード処理
	FadeControl::Update();
	event_bgm->Update();
	ui->Update();
	// シェーダーのデバッグ
	DebugShaderCtrl();
	particle->Update();

	(this->*Mode_funk[mode])();
}

void SceneMain::Start()
{
	//　プレイヤー
	player_mng->Update();
	// カメラ
	camera->Update();

	if (ui->isStart())mode = MODE::MAIN;
}
void SceneMain::Main()
{
	//　プレイヤー
	player_mng->Update();

	paper_obj_mng->Update();

	// カメラ
	camera->Update();

	//ナンバーエフェクト
	Number_Effect::Update();

	//　仮の処理
	static float angle = 0;
	angle += 0.0007f;
	sky->SetAngle(angle);
	sky->Update();

	if (timer->Get_limit_time() == 0)
	{
		event_bgm->Set_mode(EventBGM::MODE::END);
		ui->Change_mode((int)SceneMain::MODE::END);
		se->Stop_all();
		mode = MODE::END;
		m_pThread->End();	// ここでEndすることによってフェードの間にスレッド終了の余裕を持たせる
		
		SOCKET_MANAGER->EndGame();// 終わったよと報告
	}
}
void SceneMain::End()
{
	if (event_bgm->Get_mode() == EventBGM::MODE::NONE && FadeControl::isFadeOut_W)
	{
		se->Stop_all();
		// シーン登録
		MainFrame->ChangeScene(new SceneResult());
	}
}

static float ANGLE = 2.5f;
// シェーダーのデバッグに色々
void SceneMain::DebugShaderCtrl()
{

	
	if ((GetKeyState('U') & 0x80))ANGLE += 0.05f;
	if ((GetKeyState('I') & 0x80))ANGLE -= 0.05f;
	LightVec = Vector3(sinf(ANGLE), -0.98f, cosf(ANGLE));
	LightVec.Normalize();


	//if ((GetKeyState('O') & 0x80))exposure += 0.01f;
	//if ((GetKeyState('P') & 0x80))exposure -= 0.01f;
	//if ((GetKeyState('K') & 0x80))exposure += 1.0f;
	//if ((GetKeyState('L') & 0x80))exposure -= 1.0f;
	//shaderD->SetValue("exposure", exposure);

}

//******************************************************************
//		描画
//******************************************************************

void SceneMain::Render()
{
	camera->Render();
	if (deferredFlag)
	{
		DeferredManager.Update(camera->Get_pos());

		// 影描画
		RenderShadow();

		//　描画クリア
		DeferredManager.ClearBloom();
		DeferredManager.ClearForward();
		DeferredManager.ClearAllRender();

		/*■■■■■■■G_Buffer開始■■■■■■■*/
		DeferredManager.G_Bigin();

		stage->Render(shaderD, "G_Buffer");
		sky->Render(shaderD, "G_Buffer");
		player_mng->Render(shaderD, "G_Buffer");
		paper_obj_mng->Render(shaderD, "G_Buffer");

		DeferredManager.G_End();// ここまで
		/*■■■■■■■■G_Buffer終了■■■■■■*/

		/*	G_bufferを利用した描画	*/
		DeferredManager.DirLight(LightVec, Vector3(642.5f, 640.0f, 640.0f));
		DeferredManager.HemiLight(Vector3(310.5f, 300.0f, 300.0f), Vector3(320.5f, 300.0f, 300.0f));
		DeferredManager.FinalResultDeferred(); // 最後にまとめる
		DeferredManager.Fog(800, 2000, Vector3(200, 170, 200));



		/*■■■■■■通常描画(フォアード)開始■■*/
		DeferredManager.ForwardBigin();
		player_mng->EffectRender();
		particle->Render();
	
		// αテスト 
		iexSystem::Device->
			SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		iexSystem::Device->
			SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
		iexSystem::Device->
			SetRenderState(D3DRS_ALPHAREF, 230);

		/*★この中にやじるし入れて*/
	
		camera->Render_mark();


		iexSystem::Device->
			SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		iexSystem::Device->
			SetRenderState(D3DRS_ALPHAREF, 0);


		DeferredManager.ForwardEnd();
		/*■■■■■■■通常描画終り■■■■■■■*/


		// 全部描画したスクリーンを作る
		DeferredManager.AllBegin();
		DeferredManager.GetTex(SURFACE_NAME::SCREEN)->Render(0, 0, 1280, 720, 0, 0, 1280, 720, RS_COPY_NOZ);
		DeferredManager.GetTex(SURFACE_NAME::FORWARD)->Render(0, 0, 1280, 720, 0, 0, 1280, 720, RS_COPY_NOZ);
		DeferredManager.AllEnd();
		// バリアー用環境マップ
		shaderD->SetValue("EnvFullBuf", DeferredManager.GetTex(SURFACE_NAME::ALLSCREEN));

		/*■■■■■■バリアーや水用　通常描画(フォアード)開始■■*/
		DeferredManager.ForwardBigin();
		// 先頭へ
		player_mng->Render_forword();	// バリアー
		
		stage->RenderForward();			// エリア
		DeferredManager.ForwardEnd();
		/*■■■■■■■通常描画終り■■■■■■■*/

		// グロウ
		//DeferredManager.BeginDrawGlow();
		//player_mng->EffectRender();
		//DeferredManager.EndDrawGlow();

	

		// ブラ―
		//BlurFilter::Render();		


		//BlurFilter::Start_Copy();

	


		/*描画*/
		DeferredManager.RenderDeferred();
		DeferredManager.ForwardRender();

		// ブルーム
		DeferredManager.BeginDrawBloom();
		DeferredManager.GetTex(SURFACE_NAME::SCREEN)->Render(0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight,
			0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight);
		DeferredManager.EndDrawBloom();
		DeferredManager.BloomRender();
		//BlurFilter::End_Copy();

		// DownSample
		DeferredManager.UpdateDownSample(0.9f, 0.75f);
		// サーフェイス描画
		//SurfaceRender();


	}
	else
	{
		stage->Render();
		sky->Render();

		//　プレイヤー
		for (int i = 0; i < PLAYER_MAX; ++i)
		{
			//player[i]->Render();
			//Text::Draw(1100, 20 + (i * 32), 0xff00ffff, "pos.x->%.2f", player[i]->Get_pos().x);

			Text::Draw(950, 20 + (i * 32), 0xff00ffff, "名前：%s", SOCKET_MANAGER->GetUser(i).name);
		}
		player_mng->Render();
		paper_obj_mng->Render(shaderD, "copy");



	}

		//Text::Draw(32, 60, 0xff00ffff, "受信時間%.2f", bench.Get_time());

		//Text::Draw(32, 360, 0xff00ffff, "x%.2f", ANGLE);
		//Text::Draw(32, 390, 0xff00ffff, "x%.2f", bench.Get_time());
		//マウスの場所
		//Text::Draw(10, 60, 0xff000000, "マウスのXの動き%.2f", player_mng->Get_player(SOCKET_MANAGER->GetID())->m_controlDesc.mouseX);
		//Text::Draw(10, 80, 0xff000000, "マウスのYの動き%.2f", player_mng->Get_player(SOCKET_MANAGER->GetID())->m_controlDesc.mouseY);

		//ナンバーエフェクト
		Number_Effect::Render();

		// UI
		ui->Render();

		// 退魔ー(UIで描画)
		//timer->Render();

		//　プレイヤーの名前
		//for (int i = 0; i < PLAYER_MAX; ++i)
		//{
		//	Text::Draw(950, 20 + (i * 32), 0xff00ffff, "名前：%s", SOCKET_MANAGER->GetUser(i).name);
		//}


		//SurfaceRender();

		//フェード処理
		FadeControl::Render();



}

// シャドウ描画
void SceneMain::RenderShadow()
{
	// 近距離
	// 影用プロジェクションの更新
	DeferredManager.CreateShadowMatrix(LightVec, player_mng->Get_player(SOCKET_MANAGER->GetID())->Get_pos(), player_mng->Get_player(SOCKET_MANAGER->GetID())->Get_Flont() * 80, 350);
	// near
	DeferredManager.ShadowBegin();
	
		stage->Render(shaderD, "ShadowBuf");

		Vector3 flont;
		flont.x = matView._13;
		flont.y = matView._23;
		flont.z = matView._33;
		Vector3 savePos = player_mng->Get_player(0)->Get_pos();
		player_mng->Get_player(0)->Set_pos(player_mng->Get_player(0)->Get_pos() + (flont * 35));
		player_mng->Get_player(0)->Get_Model()->Update();
		player_mng->Render(shaderD, "ShadowBuf");
		player_mng->Get_player(0)->Set_pos(savePos);
		player_mng->Get_player(0)->Get_Model()->Update();

	DeferredManager.ShadowEnd();// end
	
	// 遠距離
	if (DeferredManager.GetCascadeFlag())
	{
		//if (FarShadowFlag <= 600)
		{
			// 遠距離プロジェクションの更新
			DeferredManager.CreateShadowMatrixL(LightVec, player_mng->Get_player(SOCKET_MANAGER->GetID())->Get_pos(), player_mng->Get_player(SOCKET_MANAGER->GetID())->Get_Flont() * 160, 500);

			// far
			DeferredManager.ShadowBeginL();

			stage->Render(shaderD, "ShadowBufL");
			//player_mng->Render(shaderD, "G_Buffer");

			DeferredManager.ShadowEndL();// end

			FarShadowFlag++;
		}
	}
	
}

void SceneMain::SurfaceRender()
{

	enum {
		X = 320 / 2, Y = 180 / 2
	};

	int texX = 0;
	int texY = 0;

	DeferredManager.GetTex(SURFACE_NAME::DIFFUSE)->Render(X*texX, Y*texY, X, Y, 0, 0, 1280, 720);

	texX++;
	DeferredManager.GetTex(SURFACE_NAME::NORMAL)->Render(X*texX, Y*texY, X, Y, 0, 0, 1280, 720);

	texX++;
	DeferredManager.GetTex(SURFACE_NAME::SPECULAR)->Render(X*texX, Y*texY, X, Y, 0, 0, 1280, 720);

	texX++;
	DeferredManager.GetTex(SURFACE_NAME::DEPTH)->Render(X*texX, Y*texY, X, Y, 0, 0, 1280, 720);

	texX++;
	DeferredManager.GetTex(SURFACE_NAME::LIGHT)->Render(X*texX, Y*texY, X, Y, 0, 0, 1280, 720);

	texX++; texY;
	DeferredManager.GetTex(SURFACE_NAME::SPEC)->Render(X*texX, Y*texY, X, Y, 0, 0, 1280, 720);

	texX++; texY;
	DeferredManager.GetTex(SURFACE_NAME::DOF)->Render(X*texX, Y*texY, X, Y, 0, 0, 1280, 720);

	texX++; texY;
	DeferredManager.GetTex(SURFACE_NAME::SHADOW)->Render(X*texX, Y*texY, X, Y, 0, 0, 1280, 720);

	texX = 0; texY++;
	if (DeferredManager.GetShadowFlag())
	{
		DeferredManager.GetTex(SURFACE_NAME::SHADOWMAP)->Render(X*texX, Y*texY, X, Y, 0, 0, 2048, 2048);
	}
	texX++; texY;
	if (DeferredManager.GetCascadeFlag())
	{
		DeferredManager.GetTex(SURFACE_NAME::SHADOWMAPL)->Render(X*texX, Y*texY, X, Y, 0, 0, 2048, 2048);
	}

	texX++; texY;
	DeferredManager.GetTex(SURFACE_NAME::BLOOMSCREEN)->Render(X*texX, Y*texY, X, Y, 0, 0, 1280, 720);

	texX++; texY;
	DeferredManager.GetTex(SURFACE_NAME::BLOOM)->Render(X*texX, Y*texY, X, Y, 0, 0, 1280 / 8, 720 / 8);

	texX++; texY;
	DeferredManager.GetTex(SURFACE_NAME::GLOWSCREEN)->Render(X*texX, Y*texY, X, Y, 0, 0, 1280, 720);

	texX++; texY;
	DeferredManager.GetTex(SURFACE_NAME::GLOW)->Render(X*texX, Y*texY, X, Y, 0, 0, 1280 / 8, 720 / 8);

	texX++; texY;
	DeferredManager.GetTex(SURFACE_NAME::FORWARD)->Render(X*texX, Y*texY, X, Y, 0, 0, 1280, 720);

	texX=0; texY++;
	DeferredManager.GetTex(SURFACE_NAME::DOWNSAMPLE)->Render(X*texX, Y*texY, X, Y, 0, 0, 1280, 720);

	texX=0; texY++;
	DeferredManager.GetTex(SURFACE_NAME::ALLSCREEN)->Render(X*texX, Y*texY, X, Y, 0, 0, 1280, 720);



}