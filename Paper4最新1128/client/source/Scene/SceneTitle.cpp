#include	"iextreme.h"

#include	"../system/Framework.h"
#include	"SceneTitle.h"
#include	"SceneSelect.h"
#include	"SceneMakePoster.h"

#include	"../../IEX/OKB.h"

using namespace std;

iex2DObj* aaa;
Vector3 aaa_pos;
Vector3 aaa_vec;

iex2DObj* bbb;
Vector3 bbb_pos;

//******************************************************************
//		初期化・解放
//******************************************************************
bool SceneTitle::Initialize()
{
	aaa = new iex2DObj("DATA/USB.png");
	aaa_pos = Vector3(600,600,0);
	aaa_vec = Vector3(0, 1, 0);

	bbb = new iex2DObj("DATA/もなど.png");
	bbb_pos = Vector3(200,200,0);


	//	環境設定
	iexLight::SetAmbient(0x808080);
	iexLight::SetFog(800, 3000, 0);

	// Fade処理
	FadeControl::Setting(FadeControl::FADE_IN, 26);

	view = new iexView();
	view->Set(Vector3(0, 0, -90), Vector3(0, 0, 0));

	images[IMAGE::BACK] = new iex2DObj("DATA/Image/title/Title.png");
	images[IMAGE::CURSOR] = new iex2DObj("DATA/makePoster/cursor4.png");

	start_button.pos = Vector3(27, -25, 0);
	Texture2D *texture = iexTexture::Load("DATA/Poster/tex_start.png");
	start_button.obj = new iex3DObj("DATA/paper object/Poster/posuta-.IEM");
	start_button.obj->SetTexture(texture, 0);
	start_button.obj->SetAngle(PI);
	start_button.obj->SetScale(1.0f);
	start_button.obj->SetPos(start_button.pos);
	start_button.obj->SetMotion(0);

	start_button.rend = false;
	start_button.pointing = false;

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


	//ナンバーエフェクト
	Number_Effect::Update();

	//フェード処理
	FadeControl::Update();

	//if (KEY_Get(KEY_ENTER) == 3)
	//{
	//	//	初期シーン登録
	//	MainFrame->ChangeScene(new SceneMakePoster());
	//}

	//if (KEY(KEY_LEFT) == 1)
	//{
	//	start_button.pos.x -= 1.0f;
	//}
	//if (KEY(KEY_RIGHT) == 1)
	//{
	//	start_button.pos.x += 1.0f;
	//}
	//if (KEY(KEY_UP) == 1)
	//{
	//	start_button.pos.y += 1.0f;
	//}
	//if (KEY(KEY_DOWN) == 1)
	//{
	//	start_button.pos.y -= 1.0f;
	//}

	start_button.obj->SetPos(start_button.pos);

	// 破り中
	if (start_button.rend)
	{
		start_button.obj->Animation();

		if (start_button.obj->GetFrame() >= 47)	MainFrame->ChangeScene(new SceneSelect());
	}
	// 待機中
	else
	{

		start_button.pointing = (
			mouse.pos_x >= min_v.x && mouse.pos_x <= max_v.x&&
			mouse.pos_y >= min_v.y && mouse.pos_y <= max_v.y);

		if ((KeyBoardTRG(MOUSE_LEFT) && start_button.pointing) || KeyBoardTRG(KB_ENTER))
		{
			start_button.obj->SetMotion(1);
			start_button.rend = true;
		}
	}
}



//******************************************************************
//		描画
//******************************************************************

void SceneTitle::Render()
{
	view->Activate();
	view->Clear();

	images[IMAGE::BACK]->Render(0, 0, 1280, 720, 0, 0, 1280, 720, RS_COPY, 0xffffffff, 1.0f);

	start_button.obj->Update();
	start_button.obj->Render();

	if (KEY(KEY_LEFT))
	{
		aaa_pos.x--;
	}
	if (KEY(KEY_RIGHT))
	{
		aaa_pos.x++;
	}
	if (KEY(KEY_UP))
	{
		aaa_pos.y--;
	}
	if (KEY(KEY_DOWN))
	{
		aaa_pos.y++;
	}

	// 内積するで
	Vector3 vec;
	vec = bbb_pos - aaa_pos;
	vec.Normalize();
	float dot = Vector3Dot(vec, aaa_vec);

	aaa->Render((int)aaa_pos.x, (int)aaa_pos.y, 32, 32, 0, 0, 32, 32);
	if (dot <= 0.3f)
	{
		bbb->Render((int)bbb_pos.x, (int)bbb_pos.y, 128, 128, 0, 0, 128, 128);
	}
	
	
	Text::Draw(100, 10, 0xffff00ff, "dot%.2f", dot);
	
	//images[IMAGE::CURSOR]->Render(mouse.pos_x, mouse.pos_y, 64, 64, 0, 64, 64, 64);

	//ナンバーエフェクト
	Number_Effect::Render();

	//フェード処理
	FadeControl::Render();

	if(start_button.pointing)iexPolygon::Rect(min_v.x, min_v.y, max_v.x-min_v.x, max_v.y-min_v.y, RS_COPY, 0x99fffff);

	//Text::Draw(32, 32, 0xffffffff, "%d", mouse.pos_x);
	//Text::Draw(32, 64, 0xffffffff, "%d", mouse.pos_y);
	//Text::Draw(32, 96, 0xffffffff, "%.1f", start_button.pos.z);
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
