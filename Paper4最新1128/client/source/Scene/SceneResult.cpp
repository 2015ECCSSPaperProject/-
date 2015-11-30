
#include	"iextreme.h"

//net�͈�ԏ��߂ɃC���N���[�h���Ȃ��ƃG���[���N����
#include	"../Net/Socket.h"
//#include	"../Net/net_config_loader.h"

#include	"../system/Framework.h"
#include	"../system/Thread.h"

#include	"SceneResult.h"
#include	"SceneSelect.h"

//#include	"../score/Score.h"
//#include	"../data/LimitedData.h"

/**********************/
/*	�O���[�o���ϐ�	�@*/
/**********************/

//�@�X���b�h���~�߂�ׂɉ��ō����
//static int ThreadEND = false;


using namespace std;

//******************************************************************
//		�������E���
//******************************************************************
bool SceneResult::Initialize()
{
	//	���ݒ�
	iexLight::SetAmbient(0x808080);
	iexLight::SetFog(800, 3000, 0);

	// Fade����
	FadeControl::Setting(FadeControl::FADE_IN, 26);

	view = new iexView();
	view->Set(Vector3(0, 30, -60), Vector3(0, 0, 0));

	Select = new iex2DObj("DATA/Image/lobby/select.png");

	//if (!SOCKET_MANAGER->Init())
	//{
	//	MessageBox(0, "�T�[�o�[�Ƃ̐ڑ����ł��܂���", null, MB_OK);
	//	exit(0);
	//	return false;
	//}

	//�@���O����
	//SOCKET_MANAGER->SendName();

	// �|�X�^�[�ɕK�v�ȏ���������
	//PosterInit();

	//�@�����̃X�e�b�v�� STEP::START_OK ���o����
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
//		����
//******************************************************************
void SceneResult::Update()
{
	//�i���o�[�G�t�F�N�g
	Number_Effect::Update();

	//�t�F�[�h����
	FadeControl::Update();

	//switch (step)
	//{
	//case STEP::START_NO:
	//	//�@A�{�^����������
	//	if (KEY_Get(KEY_ENTER) == 3)
	//	{
	//		step = STEP::START_OK;
	//	}

	//	break;
	//case STEP::START_OK:
	//{
	//					   //�@�܂������ł��ĂȂ��̂Ŗ߂�܂�
	//					   if (KEY_Get(KEY_ENTER) == 3)
	//					   {
	//						   step = STEP::START_NO;
	//					   }

	//					   //�@�ǋL�F��������
	//					   // �FisResdy==2��������Q�[����ʂցI�I
	//					   //  enum { READY = 2 };
	//					   //int count(0);
	//					   //int active(0);
	//					   //for (int i = 0; i < PLAYER_MAX; ++i)
	//					   //{
	//					   // if (SOCKET_MANAGER->GetUser(i).com == UserData::ACTIVE_USER)
	//					   //  ++active;

	//					   // // isReady���S��OK�ɂȂ��Ă���@�J�E���g�����
	//					   // if (SOCKET_MANAGER->GetUser(i).isReady == UserData::READY_MUTCH_ALL)
	//					   //  ++count;
	//					   //}
	//					   ////�@�S��OK����������
	//					   //if (active == count)
	//					   //{
	//					   // MainFrame->ChangeScene(new SceneMain());
	//					   //}

	//					   enum { READY = 2 };
	//					   if (SOCKET_MANAGER->GetUser(SOCKET_MANAGER->GetID()).isReady == READY)
	//					   {
	//						   //	�����V�[���o�^
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
//		�`��
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

	// �X�R�A�`��
	//Text::Draw(320, 320, 0xffffffff, "�X�R�A : %d",limited_data->Get_score(SOCKET_MANAGER->GetID()));


	//�X�e�b�v
	switch (step)
	{
	case STEP::GAME:

		Text::Draw(10, 620, 0xff00ffff, "ENTER�Ői��");

		break;
	}

	//�i���o�[�G�t�F�N�g
	Number_Effect::Render();

	//�t�F�[�h����
	FadeControl::Render();

}
