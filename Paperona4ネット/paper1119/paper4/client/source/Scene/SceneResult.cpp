
#include	"iextreme.h"

//net�͈�ԏ��߂ɃC���N���[�h���Ȃ��ƃG���[���N����
#include	"../Net/Socket.h"
//#include	"../Net/net_config_loader.h"

#include	"../system/Framework.h"
#include	"../system/Thread.h"

#include	"SceneResult.h"
#include	"SceneSelect.h"

#include	"../score/Score.h"

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

	// �|�X�^�[�ɕK�v�ȏ���������
	PosterInit();

	//�@�����̃X�e�b�v�� STEP::START_OK ���o����
	step = STEP::START_NO;

	m_pThread = 0;

	//	�X���b�h�J�n
	//ThreadEND = false;
	m_pThread = new Thread(ThreadFunc, this);
	m_pThread->Run();

	return true;
}

void SceneResult::PosterInit()
{

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

SceneResult::~SceneResult()
{
	//ThreadEND = true;

	delete view;
	delete stage;
	delete Select;

	delete m_pThread;

	SAFE_DELETE(score);

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

//===================================================================================
//   �X���b�h
//===================================================================================
void SceneResult::ThreadFunc(void* pData, bool*isEnd)
{
	SceneResult* pMulti((SceneResult*)pData);

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
		case SceneResult::STEP::START_NO:
			//�@�Z���N�g��ʂ̍X�V
			SOCKET_MANAGER->UpdateTeam(false);
			// ���C���[�X�V
			SOCKET_MANAGER->UpdateLayer();
			break;
		case SceneResult::STEP::START_OK:
			//�@�Z���N�g��ʂ̍X�V
			SOCKET_MANAGER->UpdateTeam(true);
			// ���C���[�X�V
			SOCKET_MANAGER->UpdateLayer();
			break;
		case SceneResult::STEP::GAME:
			//�@�Z���N�g��ʂ̍X�V
			SOCKET_MANAGER->UpdateTeam(2);
			// ���C���[�X�V
			SOCKET_MANAGER->UpdateLayer();
			break;
		}

	}
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

	switch (step)
	{
	case STEP::START_NO:
		//�@A�{�^����������
		if (KEY_Get(KEY_ENTER) == 3)
		{
			step = STEP::START_OK;
		}

		break;
	case STEP::START_OK:
	{
						   //�@�܂������ł��ĂȂ��̂Ŗ߂�܂�
						   if (KEY_Get(KEY_ENTER) == 3)
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
					   MainFrame->ChangeScene(new SceneSelect());
	}

		break;

	default:


		break;
	}

}


//******************************************************************
//		�`��
//******************************************************************

void SceneResult::Render()
{
	view->Activate();

	//�@�}���`�����_�[�^�[�Q�b�g�Ń|�X�^�[��`��
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		PosterRender(i);
	}
	//�o�b�N�o�b�t�@
	iexSystem::Device->SetRenderTarget(0, backbuffer);

	view->Clear();

	stage->Render();

	Select->Render(0, 0, 1280, 720, 0, 0, 1280, 720);

	for (int i = 0; i < PLAYER_MAX; ++i)
	{
		Text::Draw(200, 470 + (i * 32), 0xffff0000, "isReady[%d]->%d", i, SOCKET_MANAGER->GetUser(i).isReady);
	}

	// �X�R�A�`��
	Text::Draw(320, 320, 0xffffffff, "�X�R�A : %d",score->Get(SOCKET_MANAGER->GetID()));


	//�X�e�b�v
	switch (step)
	{
	case STEP::GAME:

		Text::Draw(10, 620, 0xff00ffff, "ENTER�Ői��");

		break;
	}

	// �F�̃|�X�^�[
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		posterScreen[i]->Render(900, (16+i*100), 512 / 6, 512 / 6, 0, 0, 512, 512);
	}

	//�i���o�[�G�t�F�N�g
	Number_Effect::Render();

	//�t�F�[�h����
	FadeControl::Render();

}


//�@�|�X�^�[�̕`��@�����̓v���C���[No
void SceneResult::PosterRender(int i)
{
	posterScreen[i]->RenderTarget();
	view->Clear(0xffffffff);
	//�@��������|�X�^�[�̃e�N�X�`���[�Ɉڂ�G��`��

	//�@���C���[�̉摜
	for (int num = (LAYER_MAX - 1); num >= 0; num--)//���`�揇���O����O�ɗ���悤��
	{
		if (SOCKET_MANAGER->GetLayer(i).layerdata[num].kind != -1)//�@���C���[���󂾂�����`�悵�Ȃ�
		{
			//�@�G��\���@���|�X�^�[�̕\���ꏊ�����炵�Ă���
			StampPicture[SOCKET_MANAGER->GetLayer(i).layerdata[num].kind][SOCKET_MANAGER->GetLayer(i).layerdata[num].num]
				->Render((int)SOCKET_MANAGER->GetLayer(i).layerdata[num].x - 256 - (64), (int)SOCKET_MANAGER->GetLayer(i).layerdata[num].y - 256 - (128), 512, 512, 0, 0, 512, 512);
		}
	}

	// �|�X�^�[�̃t���[�� ���N���C�A���g�̔ԍ��ɂ���ĐF����ŕ�����
	posterFrame[i]->Render(0, 0, 512, 512, 0, 0, 512, 512);

	//�e�N�X�`���[�Ƃ��ăV�F�[�_�[��
	char MapName[64];
	sprintf(MapName, "PosterMap_%d", i);
	shader->SetValue(MapName, posterScreen[i]);

}