#include	"../system/Framework.h"
//net
#include	"../Net/PaperServer.h"
#include	"SceneMainServer.h"

#include	"../system/Thread.h"
#include	"../Player/BasePlayer.h"
#include	"../Player/NetPlayer.h"
#include	"../Player/PlayerManager.h"

#include	"../stage/Stage.h"

using namespace std;

#include "../poster/Poster_manager.h"
#include "../score/Score.h"
#include	"../timer/Timer.h"
#include "../event/Event.h"

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

	// �C�x���g
	event_list = new Event_list;

	//stage = new iexMesh2("DATA/BG/stage_puroto.imo");
	stage = new Stage;
	stage->Initialize();

	//�@�v���C���[������
	player_mng = new PlayerManager;
	player_mng->Initialize();
	
	score = new Score;
	poster_mng = new Poster_manager;
	poster_mng->Initialize(score);

	// �^�C�}�[
	//timer = new Timer;
	timer = nullptr;

	// �|�X�^�[
	PosterInit();

	//	�X���b�h�J�n
	m_pThread = 0;
	isRun = true;

	m_pThread = new Thread(ThreadFunc, this);
	m_pThread->Run();

}

void SceneMainServer::PosterInit()
{
	//�@���̎�ނɖ��O��t����
	enum KIND_NAME{
		OBJECT = 0, FOOD = 1, BACK = 2
	};

	//�@�C���X�g���������X�^���v�蒠������
	for (int i = 0; i < KIND_MAX; i++)
	{
		for (int no = 0; no < NO_MAX; no++)
		{
			StampPicture[i][no] = NULL;
		}
	}

	//�C���X�g�͂����œ���Ă����܂��B
	StampPicture[KIND_NAME::OBJECT][0] = new iex2DObj("DATA/makePoster/stamp/yen.png");
	StampPicture[KIND_NAME::OBJECT][1] = new iex2DObj("DATA/makePoster/stamp/coffee.png");
	StampPicture[KIND_NAME::OBJECT][2] = new iex2DObj("DATA/makePoster/stamp/gentei.png");
	StampPicture[KIND_NAME::OBJECT][3] = new iex2DObj("DATA/makePoster/stamp/hamburger.png");
	StampPicture[KIND_NAME::FOOD][0] = new iex2DObj("DATA/makePoster/stamp/aniki.png");
	StampPicture[KIND_NAME::FOOD][1] = new iex2DObj("DATA/makePoster/stamp/�c�݂˂���.png");
	StampPicture[KIND_NAME::FOOD][2] = new iex2DObj("DATA/makePoster/stamp/�I.png");
	StampPicture[KIND_NAME::FOOD][3] = new iex2DObj("DATA/makePoster/stamp/���炵�˂���.png");
	StampPicture[KIND_NAME::BACK][0] = new iex2DObj("DATA/makePoster/stamp/back/not.png");
	StampPicture[KIND_NAME::BACK][1] = new iex2DObj("DATA/makePoster/stamp/back/tehutehu.png");
	StampPicture[KIND_NAME::BACK][2] = new iex2DObj("DATA/makePoster/stamp/back/block.png");
	StampPicture[KIND_NAME::BACK][3] = new iex2DObj("DATA/makePoster/stamp/back/midori.png");
	StampPicture[KIND_NAME::BACK][4] = new iex2DObj("DATA/makePoster/stamp/back/blue.png");
	StampPicture[KIND_NAME::BACK][5] = new iex2DObj("DATA/makePoster/stamp/back/�Q�C�p���X.png");

	//�X�N���[��
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		posterScreen[i] = new iex2DObj(512, 512, IEX2D_RENDERTARGET);

		//�@�F���t���[��
		char fileName[128];
		sprintf(fileName, "DATA/makePoster/frame/Posterframe%d.png", i);
		posterFrame[i] = new iex2DObj(fileName);
	}

	iexSystem::Device->GetRenderTarget(0, &backbuffer);//���`���


};

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

	SAFE_DELETE(player_mng);

	SAFE_DELETE(poster_mng);
	SAFE_DELETE(score);
	SAFE_DELETE(event_list);
	SAFE_DELETE(timer);

	// �X�^���v
	for (int i = 0; i < KIND_MAX; i++)
	{
		for (int no = 0; no < NO_MAX; no++)
		{
			SAFE_DELETE(StampPicture[i][no]);
		}
	}

	// �X�N���[��
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		SAFE_DELETE(posterScreen[i]);
		SAFE_DELETE(posterFrame[i]);
	}

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

	player_mng->Update();
	
	poster_mng->Update();

	if (timer)
	{
		event_list->Update();
		timer->Check();
	}
}



//******************************************************************
//		�`��
//******************************************************************

void SceneMainServer::Render()
{
	//�@�}���`�����_�[�^�[�Q�b�g�Ń|�X�^�[��`��
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		PosterRender(i);
	}

	//�o�b�N�o�b�t�@
	iexSystem::Device->SetRenderTarget(0, backbuffer);

	view->Clear();

	stage->Render();

	player_mng->Render();

	poster_mng->Render();

	// �F�̃|�X�^�[
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		posterScreen[i]->Render((i * 160), 580, 512 / 4, 512 / 4, 0, 0, 512, 512);
	}

	// �T�[�o�[�`��
	ServerManager::Render();

	//�i���o�[�G�t�F�N�g
	Number_Effect::Render();

	//�t�F�[�h����
	FadeControl::Render();

}


//�@�|�X�^�[�̕`��@�����̓v���C���[No
void SceneMainServer::PosterRender(int i)
{
	posterScreen[i]->RenderTarget();
	view->Clear(0xffffffff);
	//�@��������|�X�^�[�̃e�N�X�`���[�Ɉڂ�G��`��

	//�@���C���[�̉摜
	for (int num = (LAYER_MAX - 1); num >= 0; num--)//���`�揇���O����O�ɗ���悤��
	{
		if (ServerManager::GetLayer(i).layerdata[num].kind != -1)//�@���C���[���󂾂�����`�悵�Ȃ�
		{
			//�@�G��\���@���|�X�^�[�̕\���ꏊ�����炵�Ă���
			StampPicture[ServerManager::GetLayer(i).layerdata[num].kind][ServerManager::GetLayer(i).layerdata[num].num]
				->Render((int)ServerManager::GetLayer(i).layerdata[num].x - 256 - (64), (int)ServerManager::GetLayer(i).layerdata[num].y - 256 - (128), 512, 512, 0, 0, 512, 512);
		}
	}

	// �|�X�^�[�̃t���[�� ���N���C�A���g�̔ԍ��ɂ���ĐF����ŕ�����
	posterFrame[i]->Render(0, 0, 512, 512, 0, 0, 512, 512);

}