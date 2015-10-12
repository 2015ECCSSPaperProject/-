#include	"iextreme.h"

#include	"../system/Framework.h"
#include	"SceneTitle.h"
#include	"SceneSelect.h"
#include	"SceneMain.h"

using namespace std;

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
	view->Set(Vector3(0, 30, -60), Vector3(0, 0, 0));

	title = new iex2DObj("DATA/Title.png");

	return true;
}

SceneTitle::~SceneTitle()
{
	delete view;
	delete title;

}


//******************************************************************
//		処理
//******************************************************************
void SceneTitle::Update()
{


	//ナンバーエフェクト
	Number_Effect::Update();

	//フェード処理
	FadeControl::Update();

	if (KEY_Get(KEY_ENTER,0) == 3)
	{
		//	初期シーン登録
		MainFrame->ChangeScene(new SceneSelect());
	}
}



//******************************************************************
//		描画
//******************************************************************

void SceneTitle::Render()
{
	view->Activate();
	view->Clear();

	title->Render(0, 0, 1280, 720, 0, 0, 1280, 720);

	//ナンバーエフェクト
	Number_Effect::Render();

	//フェード処理
	FadeControl::Render();

}