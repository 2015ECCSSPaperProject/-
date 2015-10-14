#include	"../system/Framework.h"
//net
#include	"../Net/PaperServer.h"
#include	"SceneMainServer.h"

#include	"../system/Thread.h"
#include	"../Player/BasePlayer.h"
#include	"../Player/NetPlayer.h"

#include	"../stage/Stage.h"

using namespace std;

// �A���~�^�}
iex3DObj *set;

//******************************************************************
//		�������E���
//******************************************************************
SceneMainServer::SceneMainServer()
{
	//	���ݒ�
	iexLight::SetAmbient(0x808080);
	iexLight::SetFog( 800, 3000, 0 );

	// Fade����
	FadeControl::Setting(FadeControl::FADE_IN, 26);

	view = new iexView();
	view->Set(Vector3(0, 600, -60), Vector3(0, 0, 0));
	view->Activate();

	//stage = new iexMesh2("DATA/BG/stage_puroto.imo");
	stage = new Stage;
	stage->Initialize();

	set = new iex3DObj("DATA/CHR/player/run.IEM");

	//�@�v���C���[������
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		player[i] = new NetPlayer(i);
		player[i]->Initialize(set);
	}
	

	//	�X���b�h�J�n
	m_pThread = 0;
	isRun = true;

	m_pThread = new Thread(ThreadFunc, this);
	m_pThread->Run();

}

bool SceneMainServer::Initialize()
{

	////	���ݒ�
	//iexLight::SetAmbient(0x808080);
	//iexLight::SetFog( 800, 3000, 0 );

	//// Fade����
	//FadeControl::Setting(FadeControl::FADE_IN, 26);

	//view = new iexView();
	//view->Set(Vector3(0, 100, -60), Vector3(0, 0, 0));
	//view->Activate();

	//stage = new iexMesh2("DATA/BG/stage_puroto.imo");

	////	�X���b�h�J�n
	//m_pThread = 0;
	//isRun = true;

	//m_pThread = new Thread(ThreadFunc, this);
	//m_pThread->Run();


	return true;

}


//===================================================================================
//    �X���b�h
//===================================================================================
void SceneMainServer::ThreadFunc(void* pData, bool*isEnd)
{
	SceneMainServer* pMulti((SceneMainServer*)pData);

	//�@�T�[�o�[������
	ServerManager::Init();


	/*�t���[�����[�N*/
	DWORD dwFrameTime(clock());
	while (pMulti->isRun)
	{
		/*�X���b�h�I��*/
		if (*isEnd)
			return;

		///*�t���[������*/
		//DWORD CurrentTime = clock() * 10;
		//if (CurrentTime < dwFrameTime + 167) // 0.0167f (1�t���[��)
		//	continue;

		///*�o�ߎ���*/
		//DWORD	dTime = CurrentTime - dwFrameTime;
		//if (dTime > 2000) dwFrameTime = CurrentTime;

		//dwFrameTime += 167;

		/*�l�b�g���[�N�X�V*/
		ServerManager::Update();
	}

	//ServerManager::Release();
}

SceneMainServer::~SceneMainServer()
{
	
	delete view;
	delete stage;
	
	//isRun = false;
	//�@�������ŏ���
	ServerManager::Release();
	delete m_pThread;

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		SAFE_DELETE(player[i]);
	}

	delete set;
}


//******************************************************************
//		����
//******************************************************************
void SceneMainServer::Update()
{

	//�i���o�[�G�t�F�N�g
	Number_Effect::Update();

	//�t�F�[�h����
	FadeControl::Update();

	stage->Render();

	// �T�[�o�[
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		player[i]->Update();
	}
	
}



//******************************************************************
//		�`��
//******************************************************************

void SceneMainServer::Render()
{

	view->Clear();

	stage->Render();

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		player[i]->Render();
		Text::Draw(1000, 50 + (i * 40), 0xffff00ff, "pos.x->%.2f", player[i]->Get_pos().x);

	}

	// �T�[�o�[�`��
	ServerManager::Render();

	//�i���o�[�G�t�F�N�g
	Number_Effect::Render();

	//�t�F�[�h����
	FadeControl::Render();

}