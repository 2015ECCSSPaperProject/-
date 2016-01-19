#include	"iextreme.h"

#include	"../system/Framework.h"
#include	"SceneSelect.h"
#include	"../Mouse/Mouse.h"
#include	"SceneTitle.h"
#include	"SceneMakePoster.h"

#include "../stage/Stage.h"

#include	"../sound/SoundManager.h"
#include	"../../IEX/OKB.h"

#include "../Animation/AnimationRippleEx.h"


using namespace std;

static const Vector2 MOUSE_POS(960, 454);

//******************************************************************
//		初期化・解放
//******************************************************************
bool SceneTitle::Initialize()
{
	// ディファード
	LightVec = Vector3(1, 1, 1);
	FarShadowTimer = 0;
	DeferredManager;
	DeferredManager.CreateShadowMap(2048);
	DeferredManager.CreateShadowMapL(1024);
	DeferredManager.SetEmissiveRate(1000);// エミッシブレート
	DeferredManager.SetExposure(-10);
	// 目のシュミレート結果を送る
	shaderD->SetValue("exposure", DeferredManager.GetExposure());

	// 影
	// 追加
	float ShadowFar = 200.0f;	// 近距離の影を滑らかに入れ替える為
	float ShadowNear = 150.0f;	// 近距離の影を滑らかに入れ替える為
	shaderD->SetValue("ShadowFar", ShadowFar);
	shaderD->SetValue("ShadowNear", ShadowNear);
	float AdjustValue = -0.001f;
	shaderD->SetValue("AdjustValue", AdjustValue);

	stage = new Stage;
	stage->Initialize();

	sky = new iexMesh("DATA/Skydome/Skydome.IMO");
	sky->SetScale(5.0f);
	sky->SetPos(0, -100, 0);
	sky->Update();

	//	環境設定
	iexLight::SetAmbient(0x808080);
	iexLight::SetFog(800, 3000, 0);

	// Fade処理
	FadeControl::Setting(FadeControl::FADE_IN_W, 22);

	view = new iexView();
	viewPos = Vector3(0, 10, -90);
	viewTarget = Vector3(0, 20, 0);
	view->Set(viewPos, viewTarget);

	images[IMAGE::BACK] = new iex2DObj("DATA/Image/title/title_back.png");
	images[IMAGE::CLICK1] = new iex2DObj("DATA/Image/title/click.png");
	images[IMAGE::CLICK2] = new iex2DObj("DATA/Image/title/click2.png");
	images[IMAGE::CLICK3] = new iex2DObj("DATA/Image/title/click3.png");
	images[IMAGE::GAME_START] = new iex2DObj("DATA/Image/title/gamestart.png");
	images[IMAGE::MOUSE] = new iex2DObj("DATA/Image/title/mouse.png");
	images[IMAGE::TITLE] = new iex2DObj("DATA/Image/title/title.png");
	images[IMAGE::CURSOR] = new iex2DObj("DATA/makePoster/cursor4.png");
	images[IMAGE::ARROW] = new iex2DObj("DATA/Image/title/Arrow.png");
	images[IMAGE::ICON] = new iex2DObj("DATA/Image/title/cursor_mini.png");
	//---------------------------------------------------------------------
	// ImageAnimation
	//---------------------------------------------------------------------
	arrowPosY = 0;
	arrowMoveY = 0;

	// タイトルの絵
	titleEx = new AnimationRippleEx("DATA/Image/title/title.png",
		12, 10, 12, 3.0f, -(2.5f / 12.0f), true);
	titleEx->Action();

	start_button.pos = Vector3(42.7f, 13.4f, 0);
	Texture2D *texture = iexTexture::Load("DATA/Image/title/gamestart.png");
	start_button.obj = new iex3DObj("DATA/paper object/Poster/posuta-.IEM");
	start_button.obj->SetTexture(texture, 0);
	start_button.obj->SetAngle(0,PI,PI*-.5f);
	start_button.obj->SetScale(1.0f);
	start_button.obj->SetPos(start_button.pos);
	start_button.obj->SetMotion(0);

	start_button.rend = false;
	start_button.pointing = false;

	step = STEP::WAIT;

	move_mouse = MOUSE_POS;
	mouse = new Mouse;
	mouse->Initialize(FALSE);

	return true;
}

SceneTitle::~SceneTitle()
{
	// deferredの解除
	DeferredManager.Release();

	delete stage;
	delete sky;
	delete titleEx;

	delete view;
	for (int i = 0; i < IMAGE::MAX; i++)
	{
		delete images[i];
	}
	delete start_button.obj;
	delete mouse;
}

static float angle2 = .49f;
//******************************************************************
//		処理
//******************************************************************

void SceneTitle::Update()
{
	static float ANGLE = 2.5f;
	if ((GetKeyState('U') & 0x80))ANGLE += 0.05f;
	if ((GetKeyState('I') & 0x80))ANGLE -= 0.05f;
	LightVec = Vector3(sinf(ANGLE), -0.98f, cosf(ANGLE));
	LightVec.Normalize();

	//　空の処理
	static float skyAngle = 0;
	skyAngle += 0.0007f;
	sky->SetAngle(skyAngle);
	sky->Update();

	// カメラ
	static float angle = 0.0f;
	angle += 0.001f;
	Vector3 vecAngle;
	vecAngle.x = sinf(angle);
	vecAngle.z = cosf(angle);
	vecAngle.y = .0;

	static Vector3 cameraPos = VECTOR_ZERO;

	if (KEY_Get(KEY_UP)){
		cameraPos.x += sinf(angle) * 2; cameraPos.z += cosf(angle) * 2;
	}
	if (KEY_Get(KEY_RIGHT))angle2 += 0.05f;
	if (KEY_Get(KEY_LEFT))angle2 -= 0.05f;
	if (KEY_Get(KEY_DOWN)){
		cameraPos.x -= sinf(angle); cameraPos.z -= cosf(angle);
	}
	viewPos = cameraPos+ Vector3(0, 10, 0) -vecAngle * 70;
	viewTarget = cameraPos + Vector3(0, 20, 0);

	view->Set(viewPos, viewTarget);

	vecAngle.x = sinf(angle+angle2);
	vecAngle.z = cosf(angle+angle2);
	start_button.pos = (viewPos + vecAngle * 70);
	start_button.pos.y -= .5f;

	mouse->Update();

	// イメージのアニメーション
	if (arrowPosY >= 0)
	{
		arrowMoveY = -1;
	}
	else if (arrowPosY <= -20)
	{
		arrowMoveY = 1;
	}
	// やじるし更新
	arrowPosY += arrowMoveY;


	//ナンバーエフェクト
	Number_Effect::Update();

	//フェード処理
	FadeControl::Update();


	// タイトルEX
	if (KEY_Get(KEY_SPACE) == 3){
		titleEx->Action();
	}
	titleEx->Update();

	//if (KeyBoard(KB_A)) start_button.pos.x -= .1f;
	//if (KeyBoard(KB_W)) start_button.pos.y += .1f;
	//if (KeyBoard(KB_S)) start_button.pos.y -= .1f;
	//if (KeyBoard(KB_D)) start_button.pos.x += .1f;

	start_button.obj->SetAngle(0, angle, PI*.5f);
	start_button.obj->SetPos(start_button.pos);
	start_button.obj->Update();

	if (mouse->pos.x >= min_v.x && mouse->pos.x <= max_v.x&&
		mouse->pos.y >= min_v.y && mouse->pos.y <= max_v.y)
	{
		if (!start_button.pointing)se->Play("カーソル");
		start_button.pointing = true;
	}
	else start_button.pointing = false;

	switch (step)
	{
	case STEP::WAIT:
		if ((KeyBoard(MOUSE_LEFT) && start_button.pointing))
		{
			step = STEP::CLICK;
			start_button.obj->SetMotion(1);
			start_button.rend = true;
			se->Play("成功");
		}
		break;

	case STEP::CLICK:
		if (!KeyBoard(MOUSE_LEFT))	// マウス離す
		{
			move_mouse = MOUSE_POS;
			step = STEP::WAIT;
		}
		else if (mouse->Get_move_dist() > 20)
		{
			move_mouse = MOUSE_POS;
			step = STEP::DRAG;
			se->Play("成功");
			se->Play("破る");
		}

		{
			float move_x, move_y;
			Vector2 next_vec(1180 - move_mouse.x, 600 - move_mouse.y);
			if (next_vec.Length() < 4)
			{
				move_mouse = MOUSE_POS;
			}
			else
			{
				next_vec.Normalize(move_x, move_y);
				move_mouse.x += (int)(move_x * 8);
				move_mouse.y += (int)(move_y * 8);
			}
		}
		break;
		
	case STEP::DRAG:
		start_button.obj->SetMotion(1);
		start_button.rend = true;
		step = STEP::REND_PAPER;
		break;

	case STEP::REND_PAPER:
		start_button.obj->Animation();

		if (start_button.obj->GetFrame() >= 47) MainFrame->ChangeScene(new SceneSelect());
		return;
		break;
	}


	//　debug
	if (KEY(KEY_ENTER) == 3)
	{
		MainFrame->ChangeScene(new SceneSelect());
		return;
	}
}



//******************************************************************
//		描画
//******************************************************************

void SceneTitle::Render()
{
	view->Activate();
	view->Clear(0xff666666);
	
	// ディファードセットアップ
	DeferredManager.Update(viewPos);

	// 影描画
	RenderShadow();

	//　描画クリア
	DeferredManager.ClearBloom();
	DeferredManager.ClearForward();
	DeferredManager.ClearAllRender();

	DeferredManager.G_Bigin();
	// ステージ配置
	stage->Render(shaderD, "G_Buffer");
	sky->Render(shaderD, "G_Buffer");
	start_button.obj->Render(shaderD, "G_Buffer");
	DeferredManager.G_End();// ここまで

	/*	G_bufferを利用した描画	*/
	DeferredManager.DirLight(LightVec, Vector3(642.5f, 640.0f, 640.0f));
	DeferredManager.HemiLight(Vector3(310.5f, 300.0f, 300.0f), Vector3(320.5f, 300.0f, 300.0f));
	DeferredManager.Fog(800, 2000, Vector3(200, 170, 200));
	DeferredManager.FinalResultDeferred(); // 最後にまとめる

	/*ファイナルリザルト*/
	DeferredManager.RenderDeferred();

	// フォアード
	DeferredManager.ForwardBigin();
	DeferredManager.ForwardEnd();
	DeferredManager.ForwardRender();
	
	// ブルーム
	DeferredManager.BeginDrawBloom();
	DeferredManager.GetTex(SURFACE_NAME::SCREEN)->Render(0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight,
		0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight);
	DeferredManager.EndDrawBloom();
	DeferredManager.BloomRender();



	// 西田書き換え
	//images[IMAGE::BACK]->Render(0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight, 0, 0, 1280, 720, RS_COPY, 0xffffffff, 1.0f);
	//images[IMAGE::BACK]->RenderBack(0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight, 0, 0, 1280, 720, RS_COPY);

	
	images[IMAGE::ARROW]->Render(1025, 364 + arrowPosY);				//	やじるしぴょん
	images[IMAGE::CLICK1]->Render(962, 520, 256, 64, 0, 0, 256, 64);

	if ((int)step >= (int)STEP::CLICK)
	{
		images[IMAGE::CLICK2]->Render(962, 520, 256, 64, 0, 0, 256, 64);
	}
	if ((int)step >= (int)STEP::DRAG)
	{
		images[IMAGE::CLICK3]->Render(962, 520, 256, 64, 0, 0, 256, 64);
	}

	// タイトル絵
	//images[IMAGE::TITLE]->Render(128, 8, 1024, 512, 0, 0, 1024, 512);
	// アニメーションタイトルEx
	titleEx->Render(128, 8, RS_COPY);

	// 手のアイコン
	if (start_button.pointing)
	{
		if ((int)step == (int)STEP::WAIT)
		{
			images[IMAGE::MOUSE]->Render(MOUSE_POS.x, MOUSE_POS.y + (int)(arrowPosY*.5f), 64, 64, 0, 0, 64, 64);
		}
		else if ((int)step < (int)STEP::DRAG)
		{
			images[IMAGE::MOUSE]->Render(move_mouse.x, move_mouse.y, 64, 64, 0, 0, 64, 64);
		}


		bool iconFlag = false;
		if (KeyBoard(MOUSE_LEFT))iconFlag = true;	// マウス離す
		images[IMAGE::ICON]->Render(mouse->pos.x - 32, mouse->pos.y - 32, 64, 64, iconFlag * 64, 0, 64, 64);
	}
	else
	{
		//	わっしょい
		images[IMAGE::ICON]->Render(mouse->pos.x - 32, mouse->pos.y - 32, 64, 64, 0, 64, 64, 64);

	}

	//ナンバーエフェクト
	Number_Effect::Render();

	//フェード処理
	//FadeControl::Render();

	//if(start_button.pointing)
		//iexPolygon::Rect(min_v.x, min_v.y, max_v.x-min_v.x, max_v.y-min_v.y, RS_COPY, 0x99000000);

#ifdef _DEBUG

	if ((GetKeyState('R') & 0x80))
		DeferredManager.GetTex(SURFACE_NAME::SHADOWMAP)->Render(0, 0, 1024/2, 1024/2, 0, 0, 1024, 1024);
	if ((GetKeyState('T') & 0x80))
		DeferredManager.GetTex(SURFACE_NAME::SHADOWMAPL)->Render(0, 0, 2048/2, 2048/2, 0, 0, 2048, 2048);
	//if ((GetKeyState('Y') & 0x80))DeferredManager.AddExposure(1.0f);
	//if ((GetKeyState('U') & 0x80))DeferredManager.AddExposure(-1.0f);

#endif

	//Text::Draw(32, 32, 0xff000000, "%d", mouse.pos_x);
	//Text::Draw(32, 64, 0xff000000, "%d", mouse.pos_y);
	//Text::Draw(32, 96, 0xff000000, "%.1f", angle2);
}

void SceneTitle::RenderShadow()
{
	// 近距離
	// 影用プロジェクションの更新
	Vector3 flontVec;
	flontVec = viewTarget - viewPos;
	flontVec.Normalize();

	DeferredManager.CreateShadowMatrix(LightVec, viewPos, flontVec * 30 , 450);
	// near
	DeferredManager.ShadowBegin();

	stage->Render(shaderD, "ShadowBuf");

	DeferredManager.ShadowEnd();// end

	// 遠距離
	if (DeferredManager.GetCascadeFlag())
	{
		if (FarShadowTimer <= 600)
		{
			// 遠距離プロジェクションの更新
			DeferredManager.CreateShadowMatrixL(LightVec, viewPos, flontVec * 10, 650);
			// far
			DeferredManager.ShadowBeginL();

			stage->Render(shaderD, "ShadowBufL");

			DeferredManager.ShadowEndL();// end

			FarShadowTimer++;
		}
	}

}