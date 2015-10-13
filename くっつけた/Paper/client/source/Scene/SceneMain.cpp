#include	"iextreme.h"

//net�͈�ԏ��߂ɃC���N���[�h���Ȃ��ƃG���[���N����
#include	"../Net/Socket.h"
//#include	"../Net/net_config_loader.h"

#include	"../system/Framework.h"
#include	"../system/Thread.h"

#include	"SceneMain.h"
#include	"../Player/BasePlayer.h"
#include	"../Player/MyPlayer.h"
#include	"../Player/NetPlayer.h"
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

// �A���~�^�}
iexMesh *set;

#include "../camera/Camera.h"
#include "../stage/Stage.h"


using namespace std;

//******************************************************************
//		�������E���
//******************************************************************
bool SceneMain::Initialize()
{

	//	���ݒ�
	iexLight::SetAmbient(0x808080);
	iexLight::SetFog(800, 3000, 0);

	// Fade����
	FadeControl::Setting(FadeControl::FADE_IN, 26);

	//view = new iexView();
	//view->Set(Vector3(0, 10, -60), Vector3(0, 10, 0));
	camera = new Camera;


	//stage = new iexMesh2("DATA/BG/stage_puroto.imo");
	stage = new Stage;
	stage->Initialize();

	sky = new iexMesh("DATA/Skydome/Skydome.IMO");
	sky->SetScale(2.0f);
	sky->SetPos(0, -100, 0);
	sky->Update();

	set = new iexMesh("DATA/a.IMO");

	//�������@����Ǝ����ŕ�����
	for (int i = 0; i <PLAYER_MAX; ++i)
	{
		if (i == SOCKET_MANAGER->GetID()){
			player[i] = new MyPlayer();
			camera->Initialize(player[i]);	// �J�����ݒ�
		}
		else
			player[i] = new NetPlayer();

		player[i]->Initialize(set);
	}

	// �J�n�t���O�𑗂�
	m_pThread = new Thread(ThreadFunc, this);

	//	�������Ƃ�
	SOCKET_MANAGER->InitGame();//������

	//	�l�b�g���[�N���J�n����
	m_pThread->Run();


	return true;


}

SceneMain::~SceneMain()
{
	delete camera;
	//delete view;
	delete stage;
	delete sky;
	delete m_pThread;//�@�Ȃ񂩃X���b�h�����Ȃ��ƃG���[�N����

	delete set;

	for (int i = 0; i <PLAYER_MAX; ++i)
	{
		SAFE_DELETE(player[i]);
	}
}

//===================================================================================
//   �X���b�h
//===================================================================================
void SceneMain::ThreadFunc(void* pData, bool*isEnd)
{
	SceneMain* pMulti((SceneMain*)pData);

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
		static int FLAME = 0;
		FLAME++;
		//if (FLAME > 6)
		{
			FLAME = 0;
			SOCKET_MANAGER->UpdateUser();
		}

	}
}

//******************************************************************
//		����
//******************************************************************
void SceneMain::Update()
{
	//�@�v���C���[
	for (int i = 0; i < PLAYER_MAX; ++i)
	{
		//�@�����̂Ȃ����ԏd�v
		//�@�����Ă��������e�v���C���[�ɃZ�b�g�I�I
		PlayerData sendPlayer = SOCKET_MANAGER->GetPlayer(i);
		player[i]->Set_pos(sendPlayer.pos);
		player[i]->Set_angleY(sendPlayer.angleY);

		player[i]->Update();
	}

	// �J����
	camera->Update();

	//�i���o�[�G�t�F�N�g
	Number_Effect::Update();

	//�t�F�[�h����
	FadeControl::Update();

	//�@���̏���
	static float angle = 0;
	angle += 0.0007f;
	sky->SetAngle(angle);
	sky->Update();

}



//******************************************************************
//		�`��
//******************************************************************

void SceneMain::Render()
{
	//view->Activate();
	//view->Clear();
	camera->Activate();
	camera->Clear();

	stage->Render();
	sky->Render();

	//�@�v���C���[
	for (int i = 0; i < PLAYER_MAX; ++i)
	{
		player[i]->Render();
		Text::Draw(1100, 20 + (i * 32), 0xff00ffff, "pos.x->%.2f", player[i]->Get_pos().x);

		Text::Draw(950, 20 + (i * 32), 0xff00ffff, "���O�F%s", SOCKET_MANAGER->GetUser(i).name);
	}


	Text::Draw(100, 20, 0xff00ffff, "��M����%.2f", bench.Get_time());



	//�i���o�[�G�t�F�N�g
	Number_Effect::Render();

	//�t�F�[�h����
	FadeControl::Render();

}