#include	"iextreme.h"

#include	"../system/Framework.h"
#include	"SceneTitle.h"
#include	"SceneSelect.h"
#include	"SceneMain.h"

using namespace std;

//******************************************************************
//		�������E���
//******************************************************************
bool SceneTitle::Initialize()
{

	//	���ݒ�
	iexLight::SetAmbient(0x808080);
	iexLight::SetFog(800, 3000, 0);

	// Fade����
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
//		����
//******************************************************************
void SceneTitle::Update()
{


	//�i���o�[�G�t�F�N�g
	Number_Effect::Update();

	//�t�F�[�h����
	FadeControl::Update();

	if (KEY_Get(KEY_ENTER,0) == 3)
	{
		//	�����V�[���o�^
		MainFrame->ChangeScene(new SceneSelect());
	}
}



//******************************************************************
//		�`��
//******************************************************************

void SceneTitle::Render()
{
	view->Activate();
	view->Clear();

	title->Render(0, 0, 1280, 720, 0, 0, 1280, 720);

	//�i���o�[�G�t�F�N�g
	Number_Effect::Render();

	//�t�F�[�h����
	FadeControl::Render();

}