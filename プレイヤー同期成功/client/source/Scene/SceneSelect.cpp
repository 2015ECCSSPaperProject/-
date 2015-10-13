
#include	"iextreme.h"

//net�͈�ԏ��߂ɃC���N���[�h���Ȃ��ƃG���[���N����
#include	"../Net/Socket.h"
//#include	"../Net/net_config_loader.h"

#include	"../system/Framework.h"
#include	"../system/Thread.h"

#include	"SceneSelect.h"
#include	"SceneMain.h"

/**********************/
/*	�O���[�o���ϐ�	�@*/
/**********************/

//�@�X���b�h���~�߂�ׂɉ��ō����
//static int ThreadEND = false;


using namespace std;

//******************************************************************
//		�������E���
//******************************************************************
bool SceneSelect::Initialize()
{

	//	���ݒ�
	iexLight::SetAmbient(0x808080);
	iexLight::SetFog(800, 3000, 0);

	// Fade����
	FadeControl::Setting(FadeControl::FADE_IN, 26);

	view = new iexView();
	view->Set(Vector3(0, 30, -60), Vector3(0, 0, 0));

	stage = new iexMesh("DATA/BG/stage_puroto.imo");

	Select = new iex2DObj("DATA/select.png");

	if (!SOCKET_MANAGER->Init())
	{
		MessageBox(0, "�T�[�o�[�Ƃ̐ڑ����ł��܂���", null, MB_OK);
		exit(0);
		return false;
	}

	//�@���O����
	SOCKET_MANAGER->SendName();

	//�@�����̃X�e�b�v�� STEP::START_OK ���o����
	step = STEP::START_NO;

	m_pThread = 0;

	//	�X���b�h�J�n
	//ThreadEND = false;
	m_pThread = new Thread(ThreadFunc, this);
	m_pThread->Run();

	return true;
}

SceneSelect::~SceneSelect()
{
	//ThreadEND = true;

	delete view;
	delete stage;
	delete Select;

	delete m_pThread;
}

//===================================================================================
//   �X���b�h
//===================================================================================
void SceneSelect::ThreadFunc(void* pData, bool*isEnd)
{
	SceneSelect* pMulti((SceneSelect*)pData);

	/*�t���[�����[�N*/
	DWORD dwFrameTime(clock());
	while (true)
	{
		/*�X���b�h�I��*/
		if (*isEnd)
			return;

		/*�t���[������*/
		DWORD CurrentTime = clock() * 10;
		if (CurrentTime < dwFrameTime + 167)
			continue;

		/*�o�ߎ���*/
		DWORD	dTime = CurrentTime - dwFrameTime;
		if (dTime > 2000) dwFrameTime = CurrentTime;

		dwFrameTime += 167;

		/*�l�b�g���[�N�X�V*/
		switch (pMulti->step)
		{
		case SceneSelect::STEP::START_NO:
			//�@�Z���N�g��ʂ̍X�V
			SOCKET_MANAGER->UpdateTeam(false);

			break;
		case SceneSelect::STEP::START_OK:
			//�@�Z���N�g��ʂ̍X�V
			SOCKET_MANAGER->UpdateTeam(true);

			break;
		case SceneSelect::STEP::GAME:
			//�@�Z���N�g��ʂ̍X�V
			SOCKET_MANAGER->UpdateTeam(2);

			break;
		}
		
	}
}



//******************************************************************
//		����
//******************************************************************
void SceneSelect::Update()
{


	//�i���o�[�G�t�F�N�g
	Number_Effect::Update();

	//�t�F�[�h����
	FadeControl::Update();

	switch (step)
	{
	case STEP::START_NO:
		//�@A�{�^����������
		if (KEY_Get(KEY_ENTER, 0) == 3)
		{
			step = STEP::START_OK;
		}

		break;
	case STEP::START_OK:
	{
		   //�@�܂������ł��ĂȂ��̂Ŗ߂�܂�
		   if (KEY_Get(KEY_ENTER, 0) == 3)
		   {
			   step = STEP::START_NO;
		   }

		   //�@�ǋL�F��������
		   // �FisResdy==2��������Q�[����ʂցI�I
		 //  enum { READY = 2 };
		   //int count(0);
		   //int active(0);
		   //for (int i = 0; i < PLAYER_MAX; ++i)
		   //{
			  // if (SOCKET_MANAGER->GetUser(i).com == UserData::ACTIVE_USER)
				 //  ++active;

			  // // isReady���S��OK�ɂȂ��Ă���@�J�E���g�����
			  // if (SOCKET_MANAGER->GetUser(i).isReady == UserData::READY_MUTCH_ALL)
				 //  ++count;
		   //}
		   ////�@�S��OK����������
		   //if (active == count)
		   //{
			  // MainFrame->ChangeScene(new SceneMain());
		   //}

		   enum { READY = 2 };
		   if (SOCKET_MANAGER->GetUser(SOCKET_MANAGER->GetID()).isReady == READY)
		   {
		   		//	�����V�[���o�^
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
//		�`��
//******************************************************************

void SceneSelect::Render()
{
	view->Activate();
	view->Clear();

	stage->Render();

	Select->Render(0, 0, 1280, 720, 0, 0, 1280, 720);

	for (int i = 0; i < PLAYER_MAX; ++i)
	{
		Text::Draw(800, 40 + (i * 32), 0xffffffff, "Clientaddr[%d]->%ld", i, SOCKET_MANAGER->GetUser(i).com);
		Text::Draw(1000, 40 + (i * 32), 0xffffffff, "name[%d]->%s", i, SOCKET_MANAGER->GetUser(i).name);
		Text::Draw(900, 470 + (i * 32), 0xff00ffff, "isReady[%d]->%d", i, SOCKET_MANAGER->GetUser(i).isReady);
	}

	//�X�e�b�v
	switch (step)
	{
	case STEP::GAME:

		Text::Draw(10, 620 , 0xff00ffff, "ENTER�Ői��");

		break;
	}


	//�i���o�[�G�t�F�N�g
	Number_Effect::Render();

	//�t�F�[�h����
	FadeControl::Render();

}