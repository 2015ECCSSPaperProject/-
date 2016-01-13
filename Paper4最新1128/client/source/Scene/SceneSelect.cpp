
#include	"iextreme.h"

//net�͈�ԏ��߂ɃC���N���[�h���Ȃ��ƃG���[���N����
#include	"../Net/Socket.h"
//#include	"../Net/net_config_loader.h"

#include	"../system/Framework.h"
#include	"../system/Thread.h"

#include	"SceneSelect.h"
#include	"SceneMain.h"

#include	"../../IEX/OKB.h"
#include	"../Animation/AnimationRipple.h"


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
	if (!SOCKET_MANAGER->Init())
	{
		MessageBox(0, "�T�[�o�[�Ƃ̐ڑ����ł��܂���", null, MB_OK);
		exit(0);
		return false;
	}

	//	���ݒ�
	iexLight::SetAmbient(0x808080);
	iexLight::SetFog(800, 3000, 0);

	// Fade����
	FadeControl::Setting(FadeControl::FADE_IN, 26);

	view = new iexView();
	view->Set(Vector3(0, 0, -90), Vector3(0, 0, 0));

	// 2D�摜������
	image[IMAGE::BACK] = new iex2DObj("DATA/Image/lobby/back2.png");
	image[IMAGE::P1] = new iex2DObj("DATA/Image/lobby/red.png");
	image[IMAGE::P2] = new iex2DObj("DATA/Image/lobby/blue.png");
	image[IMAGE::P3] = new iex2DObj("DATA/Image/lobby/yellow.png");
	image[IMAGE::P4] = new iex2DObj("DATA/Image/lobby/green.png");
	image[IMAGE::P5] = new iex2DObj("DATA/Image/lobby/purple.png");
	image[IMAGE::P6] = new iex2DObj("DATA/Image/lobby/pink.png");
	image[IMAGE::WAIT] = new iex2DObj("DATA/Image/lobby/junbi_chu.png");
	image[IMAGE::OK] = new iex2DObj("DATA/Image/lobby/junbi_ok.png");
	image[IMAGE::LIST] = new iex2DObj("DATA/Image/lobby/list.png");
	image[IMAGE::ACTION] = new iex2DObj("DATA/UI/action/1.png");
	// ���c�ǉ�
	image[IMAGE::NOPLAYER] = new iex2DObj("DATA/Image/lobby/noPlayer.png");
	image[IMAGE::WANTED] = new iex2DObj("DATA/Image/lobby/Wanted.png");
	image[IMAGE::TEN] = new iex2DObj("DATA/Image/lobby/ten.png");
	image[IMAGE::BACK2] = new iex2DObj("DATA/Image/lobby/�w�i.png");

	// �����̃A�j���[�V����
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		isActivePlayer[i] = false;
		moveX[i] = +300;
		alpha[i] = 64;
	}
	// Rip�p
	IconRip[0] = new AnimationRipple("DATA/Image/lobby/red.png", 15, 0.1f);
	IconRip[1] = new AnimationRipple("DATA/Image/lobby/blue.png", 15, 0.1f);
	IconRip[2] = new AnimationRipple("DATA/Image/lobby/yellow.png", 15, 0.1f);
	IconRip[3] = new AnimationRipple("DATA/Image/lobby/green.png", 15, 0.1f);
	IconRip[4] = new AnimationRipple("DATA/Image/lobby/purple.png", 15, 0.1f);
	IconRip[5] = new AnimationRipple("DATA/Image/lobby/pink.png", 15, 0.1f);


	// �L�����N�^�[
	chara.pos = Vector3(20, -19, 0);
	chara.angle = PI;
	Texture2D *texture;
	switch (SOCKET_MANAGER->GetID())
	{
	case 1:
		texture = iexTexture::Load("DATA/CHR/player/player_blue.png");
		break;
	case 2:
		texture = iexTexture::Load("DATA/CHR/player/player_yellow.png");
		break;
	case 3:
		texture = iexTexture::Load("DATA/CHR/player/player_green.png");
		break;
	case 4:
		texture = iexTexture::Load("DATA/CHR/player/player_purple.png");
		break;
	case 5:
		texture = iexTexture::Load("DATA/CHR/player/player_pink.png");
		break;
	default:
		texture = iexTexture::Load("DATA/CHR/player/player_red.png");
		break;
	}

	chara.obj = new iex3DObj("DATA/CHR/player/run.IEM");
	//chara.obj->SetTexture(texture, 0);
	chara.obj->SetAngle(chara.angle);
	chara.obj->SetScale(1.0f);
	chara.obj->SetPos(chara.pos);
	chara.obj->SetMotion(1);

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

void SceneSelect::PosterInit()
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

SceneSelect::~SceneSelect()
{
	//ThreadEND = true;

	delete view;
	for (int i = 0; i < IMAGE::MAX; i++)delete image[i];
	delete chara.obj;
	delete m_pThread;

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

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		SAFE_DELETE(IconRip[i]);
	}
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
			// ���C���[�X�V
			SOCKET_MANAGER->UpdateLayer();
			break;
		case SceneSelect::STEP::START_OK:
			//�@�Z���N�g��ʂ̍X�V
			SOCKET_MANAGER->UpdateTeam(true);
			// ���C���[�X�V
			SOCKET_MANAGER->UpdateLayer();
			break;
		case SceneSelect::STEP::GAME:
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
void SceneSelect::Update()
{
	int AxisX = 0, AxisY = 0;
	if (KEY(KEY_LEFT) == 1) AxisX += -1;
	if (KEY(KEY_RIGHT) == 1) AxisX += 1;
	if (KEY(KEY_UP) == 1) AxisY += 1;
	if (KEY(KEY_DOWN) == 1) AxisY += -1;

	int motion_no = 1;
	if (AxisY > 0) motion_no = 0;
	else if (AxisX < 0) motion_no = 11;
	else if (AxisX > 0) motion_no = 10;
	else if (AxisY < 0) motion_no = 7;
	if (chara.obj->GetMotion() != motion_no)chara.obj->SetMotion(motion_no);
	chara.obj->Animation();


	//=============================================================
	//	�}�E�X�X�V
	POINT p;
	GetCursorPos(&p);

	//�@��ʂ̐^�񒆂ֈړ�
	RECT rc;
	GetWindowRect(iexSystem::Window, &rc);
	p.x -= rc.left + iexSystem::ScreenWidth / 2;
	p.x = (LONG)((float)p.x * .8f);
	if (GetActiveWindow() == IEX_GetWindow())
	{
		ShowCursor(FALSE);
		SetCursorPos(rc.left + iexSystem::ScreenWidth / 2, rc.top + iexSystem::ScreenHeight / 2);
	}
	float	work;
	work = (float)p.x *0.001f;
	if (work > 0.1f) work = 0.1f;
	chara.obj->SetAngle((chara.angle += work));// Angle�ɉ��Z
	//==============================================================

	// �g��@�ǉ�
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		IconRip[i]->Update();
	}
	if (KEY(KEY_SPACE) == 3)
	{
		for (int i = 0; i < PLAYER_MAX; i++)
		{
			IconRip[i]->Action();
		}
	}

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

	////�@�}���`�����_�[�^�[�Q�b�g�Ń|�X�^�[��`��
	//for (int i = 0; i < PLAYER_MAX; i++)
	//{
	//	PosterRender(i);
	//}
	////�o�b�N�o�b�t�@
	//iexSystem::Device->SetRenderTarget(0, backbuffer);

	view->Clear();

	// �w�i
	static float uvMove = 0;
	uvMove += 0.2f;
	image[IMAGE::BACK2]->RenderBack(0, 0, 1280, 720, (int)uvMove, 0, 1280, 720);

	// �L�����N�^�[
	chara.obj->Update();
	chara.obj->Render();

	// �Q���҃��X�g
	image[IMAGE::BACK]->Render(0, 0, 1280, 720, 0, 0, 1280, 720, RS_COPY);
	image[IMAGE::LIST]->Render(12, 28, 256, 64, 0, 0, 256, 64);

	// �A�N�V����UI
	image[IMAGE::ACTION]->Render(1060, 500, 256, 256, 0, 0, 256, 256);


	// �_�̃A�j���p�ϐ�
	static int tenFlame = 0;
	static int tenAnime = 0;
	tenFlame++;
	if (tenFlame >= 24)
	{
		tenFlame = 0;
		tenAnime++;
		if (tenAnime >= 3){ tenAnime = 0; }
	}



	for (int i = 0; i < PLAYER_MAX; i++)
	{
		// �����A�j���X�C�b�`
		if (isActivePlayer[i] == false)
		{	// �A�N�e�B�u�ɏ��߂ĂȂ�����
			if (SOCKET_MANAGER->GetUser(i).com == UserData::ACTIVE_USER)
			{
				IconRip[i]->Action();		// �g�䁡
				isActivePlayer[i] = true;
			}
		}
		// �����A�j���X�C�b�`
		if (isActivePlayer[i] == true)
		{	// �o�čs������E�E
			if (SOCKET_MANAGER->GetUser(i).com != UserData::ACTIVE_USER)
			{
				// ������
				isActivePlayer[i] = false;
				moveX[i] = 300;
				alpha[i] = 64;
			}
		}


		// �����A�j���X�V
		if (isActivePlayer[i] == true)
		{
			// ����
			moveX[i]-=24;
			if (moveX[i]<=0)	moveX[i] = 0;
			// ����
			alpha[i] += 8;
			if (alpha[i] >= 255)alpha[i] = 255;
			

		}
		else
		{
			// �����l�ɖ߂�
			moveX[i] = 300;
			alpha[i] = 32;
			//moveX[i] -= 6;
			//if (moveX[i] <= 0)	moveX[i] = 0;
		}


		// �A�N�e�B�u�ȃv���C���[�ȊO�͕`�悵�Ȃ�
		if (SOCKET_MANAGER->GetUser(i).com == UserData::ACTIVE_USER)
		{

			// �E�̃��[�U�[�́��@���[�U�[����
			image[IMAGE::P1 + i]->Render(64, 136 + i * 96, 64, 64, 0, 0, 64, 64);
			// �ǉ��@�g�䁡
			IconRip[i]->Render(64, 136 + i * 96,RS_COPY);

			// ���O
			DWORD col = ARGB((BYTE)alpha[i], 0, 0, 0);
			Text::Draw(140 + moveX[i], 154 + i * 96, col, "%s", SOCKET_MANAGER->GetUser(i).name);

			// �������H����OK
			image[(!SOCKET_MANAGER->GetUser(i).isReady) ? IMAGE::WAIT : IMAGE::OK]->SetARGB(alpha[i], 255, 255, 255);
			image[(!SOCKET_MANAGER->GetUser(i).isReady) ? IMAGE::WAIT : IMAGE::OK]->Render(396 + moveX[i], 136 + i * 96, 128, 64, 0, 0, 128, 64);

		}
		else // �Q�����Ă��Ȃ�������
		{
			// �E�̃��[�U�[�́�
			image[IMAGE::NOPLAYER]->Render(104, 136 + i * 96, 64, 64, 0, 0, 64, 64);
			// �ʐM�ҋ@��
			image[IMAGE::WANTED]->Render(190, 138 + i * 96, 256, 128, 0, 0, 256, 128);
			// �_
			image[IMAGE::TEN]->SetARGB(100, 100, 100, 100);
			image[IMAGE::TEN]->Render(430, 138 + i * 96, 64, 64, tenAnime * 64, 0, 64, 64);
		}

	}

	//for (int i = 0; i < PLAYER_MAX; ++i)
	//{
	//	Text::Draw(800, 40 + (i * 32), 0xffffffff, "Clientaddr[%d]->%ld", i, SOCKET_MANAGER->GetUser(i).com);
	//	Text::Draw(1000, 40 + (i * 32), 0xffffffff, "name[%d]->%s", i, SOCKET_MANAGER->GetUser(i).name);
	//	Text::Draw(900, 470 + (i * 32), 0xff00ffff, "isReady[%d]->%d", i, SOCKET_MANAGER->GetUser(i).isReady);
	//}

	//�X�e�b�v
	switch (step)
	{
	case STEP::GAME:

		Text::Draw(10, 620 , 0xff00ffff, "ENTER�Ői��");

		break;
	}

	// �F�̃|�X�^�[
	//for (int i = 0; i < PLAYER_MAX; i++)
	//{
	//	posterScreen[i]->Render((i * 160), 580, 512 / 4, 512 / 4, 0, 0, 512, 512);
	//	image[IMAGE::P1 + i]->Render(104, 136 + i * 96, 64, 64, 0, 0, 64, 64);
	//}

	////�i���o�[�G�t�F�N�g
	//Number_Effect::Render();

	//�t�F�[�h����
	FadeControl::Render();

}


//�@�|�X�^�[�̕`��@�����̓v���C���[No
void SceneSelect::PosterRender(int i)
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
	shaderD->SetValue(MapName, posterScreen[i]);

}