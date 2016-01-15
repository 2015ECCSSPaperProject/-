#include	"iextreme.h"

#include	"../system/Framework.h"
#include	"SceneTitle.h"
#include	"SceneSelect.h"
#include	"SceneMakePoster.h"

#include	"../../IEX/OKB.h"

using namespace std;

static const Vector2 MOUSE_POS(960, 600);

//******************************************************************
//		初期化・解放
//******************************************************************
bool SceneTitle::Initialize()
{
	//	環境設定
	iexLight::SetAmbient(0x808080);
	iexLight::SetFog(800, 3000, 0);

	// Fade処理
	FadeControl::Setting(FadeControl::FADE_IN, 26);

	view = new iexView();
	view->Set(Vector3(0, 0, -90), Vector3(0, 0, 0));

	images[IMAGE::BACK] = new iex2DObj("DATA/Image/title/title_back.png");
	images[IMAGE::CLICK1] = new iex2DObj("DATA/Image/title/click.png");
	images[IMAGE::CLICK2] = new iex2DObj("DATA/Image/title/click2.png");
	images[IMAGE::CLICK3] = new iex2DObj("DATA/Image/title/click3.png");
	images[IMAGE::GAME_START] = new iex2DObj("DATA/Image/title/gamestart.png");
	images[IMAGE::MOUSE] = new iex2DObj("DATA/Image/title/mouse.png");
	images[IMAGE::TITLE] = new iex2DObj("DATA/Image/title/title.png");
	images[IMAGE::CURSOR] = new iex2DObj("DATA/makePoster/cursor4.png");
	images[IMAGE::ARROW] = new iex2DObj("DATA/Image/title/Arrow.png");

	//---------------------------------------------------------------------
	// ImageAnimation
	//---------------------------------------------------------------------
	arrowPosY = 0;
	arrowMoveY = 0;


	start_button.pos = Vector3(42.7f, -13.4f, 0);
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

	return true;
}

SceneTitle::~SceneTitle()
{
	delete view;
	for (int i = 0; i < IMAGE::MAX; i++)
	{
		delete images[i];
	}
	delete start_button.obj;
}


//******************************************************************
//		処理
//******************************************************************
void SceneTitle::Update()
{
	mouse.Update();

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

	//if (KeyBoard(KB_A)) start_button.pos.x -= .1f;
	//if (KeyBoard(KB_W)) start_button.pos.y += .1f;
	//if (KeyBoard(KB_S)) start_button.pos.y -= .1f;
	//if (KeyBoard(KB_D)) start_button.pos.x += .1f;

	start_button.obj->SetPos(start_button.pos);

	start_button.pointing = (
		mouse.pos_x >= min_v.x && mouse.pos_x <= max_v.x&&
		mouse.pos_y >= min_v.y && mouse.pos_y <= max_v.y);
	switch (step)
	{
	case STEP::WAIT:
		if ((KeyBoard(MOUSE_LEFT) && start_button.pointing))
		{
			step = STEP::CLICK;
			start_button.obj->SetMotion(1);
			start_button.rend = true;
		}
		break;

	case STEP::CLICK:
		if (!KeyBoard(MOUSE_LEFT))	// マウス離す
		{
			move_mouse = MOUSE_POS;
			step = STEP::WAIT;
		}
		else if (mouse.Get_move_dist() > 20)
		{
			move_mouse = MOUSE_POS;
			step = STEP::DRAG;
		}

		{
			float move_x, move_y;
			Vector2 next_vec(1180 - move_mouse.x, 454 - move_mouse.y);
			if (next_vec.Length() < 4)
			{
				move_mouse = MOUSE_POS;
			}
			else
			{
				next_vec.Normalize(move_x, move_y);
				move_mouse.x += (int)(move_x * 4);
				move_mouse.y += (int)(move_y * 4);
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
		break;
	}

	//　debug
	if (KEY(KEY_ENTER) == 3)
	{
		MainFrame->ChangeScene(new SceneSelect());
	}



}



//******************************************************************
//		描画
//******************************************************************

void SceneTitle::Render()
{
	view->Activate();
	view->Clear(0xff666666);
	
	// 西田書き換え
	//images[IMAGE::BACK]->Render(0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight, 0, 0, 1280, 720, RS_COPY, 0xffffffff, 1.0f);
	//images[IMAGE::BACK]->RenderBack(0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight, 0, 0, 1280, 720, RS_COPY);

	start_button.obj->Update();
	start_button.obj->Render();
	
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
	if ((int)step == (int)STEP::WAIT)
	{
		images[IMAGE::MOUSE]->Render(MOUSE_POS.x, MOUSE_POS.y + (int)(arrowPosY*.5f), 64, 64, 0, 0, 64, 64);
	}
	else if ((int)step < (int)STEP::DRAG)
	{
		images[IMAGE::MOUSE]->Render(move_mouse.x, move_mouse.y, 64, 64, 0, 0, 64, 64);
	}

	images[IMAGE::TITLE]->Render(128, 8, 1024, 512, 0, 0, 1024, 512);

	//ナンバーエフェクト
	Number_Effect::Render();

	//フェード処理
	FadeControl::Render();

	//if(start_button.pointing)iexPolygon::Rect(min_v.x, min_v.y, max_v.x-min_v.x, max_v.y-min_v.y, RS_COPY, 0x99fffff);

	//Text::Draw(32, 32, 0xff000000, "%d", mouse.pos_x);
	//Text::Draw(32, 64, 0xff000000, "%d", mouse.pos_y);
	//Text::Draw(32, 96, 0xff000000, "%.1f", start_button.pos.z);
}





//=============================================================================================
//		マ	ウ	ス	更		新
void Mouse::Update()
{
	static const float Max = 0.6f;
	static const float Min = 0.25f;

	// 前回座標保存
	prev_point = current_point;

	// 取得
	GetCursorPos(&current_point);
	GetWindowRect(iexSystem::Window, &rc);
	// 窓位置と縁による調整
	pos_x = current_point.x - rc.left - 8;
	pos_y = current_point.y - rc.top - 29;
	////中央オフセット＆正規化
	axis_x = ((float)pos_x - (iexSystem::ScreenWidth / 2)) / (iexSystem::ScreenWidth / 2);
	axis_y = -((float)pos_y - (iexSystem::ScreenHeight / 2)) / (iexSystem::ScreenHeight / 2);

	// 最大値制御
	if (axis_x > Max)axis_x = Max;
	else if (axis_x < -Max)axis_x = -Max;
	if (axis_y > Max)axis_y = Max;
	else if (axis_y < -Max)axis_y = -Max;

	// 最小値なら0
	float val_x = sqrtf(axis_x * axis_x), val_y = sqrtf(axis_y * axis_y);
	if (val_x < Min) axis_x = 0;
	if (val_y < Min * 1.5f) axis_y = 0;
}
